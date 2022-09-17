#include "main.h"

/** 
 * @brief Blits a menu frame
 */
void sector_menu() {

    static Sector op = SectorScreenshot;                           // current selected option (in menu)
    static u8 co = 0;                                              // color iterator padding
    color ci = init_color(38, 0, 0, (co = (co + 1) % 76) >> 1, 0); // create a color iterator for each frame, padded with last one

    // Navigate through the menu 
    if      (kDown & HidNpadButton_AnyUp)   op -= 2; 
    else if (kDown & HidNpadButton_AnyDown) op += 2;

    // Loop menu option
    if      (op > SectorExit)       op = SectorScreenshot;
    else if (op < SectorScreenshot) op = SectorExit;

    // Validate option
    if (kDown & HidNpadButton_Plus) current_sector = SectorExit;
    if (kDown & HidNpadButton_A)    current_sector = op;
    
    // Print rainbow banner (diagonal rainbow which forwards at each frame)
    putsb("\n                    ");
    for (u16 i = 0; i < sizeof(title); i++) {
        print("\x1b[38;2;%hhu;%hhu;%hhum%c", ci.r, ci.g, ci.b, title[i]);
        if (title[i] == '\n') putsb("                    ");
        next_color(&ci);
    }
    
    // Print Album stats
    //if (!FrameClock) getAlbum(); // check album for new entries every 256 frames
    if (mount[0]) {
        cprint(CONSOLE_FG(0, 180, 128), "\n\n    Nand:    %4lu screens + %3lu movies   ", filesNandSs, filesNandMv)
        printSize((filesNandSs + filesNandMv) ? drive_size[0]: 0); // Can be non-zero even if no files
    } else cprint(CONSOLE_FG(255, 140, 0), "\n\n    Nand:    unmounted");
    
    if (mount[1]) { 
        cprint(CONSOLE_FG(0, 180, 128),     "    SD card: %4lu screens + %3lu movies   ", filesSdSs,   filesSdMv)
        printSize((filesSdSs + filesSdMv) ? drive_size[1]: 0); // Can be non-zero even if no files
    } else cprint(CONSOLE_FG(255, 140, 0),     "    SD card: unmounted");

    // Print menu
    reset_fg();
    print("\n\n  Main Menu\n\n\n");

    char sfg[22];
    snprintf(sfg, 22, "\x1b[38;2;%hhu;%hhu;%hhum\x10", selectionCI.r, selectionCI.r, selectionCI.r); // make the selected option glowing

    cprint((op == SectorScreenshot) ? SelectedBG: DefaultBG, "    %s  Copy screenshots to sdcard://switch/Album                               \n\n", (op == SectorScreenshot) ? sfg: "\x1b[38;2;255;255;255m ");
    cprint((op == SectorMovie)      ? SelectedBG: DefaultBG, "    %s  Copy movies to sdcard://switch/Album                                    \n\n", (op == SectorMovie)      ? sfg: "\x1b[38;2;255;255;255m ");
    cprint((op == SectorBoth)       ? SelectedBG: DefaultBG, "    %s  Copy screenshot & movies to sdcard://switch/Album                       \n\n", (op == SectorBoth)       ? sfg: "\x1b[38;2;255;255;255m ");
    cprint((op == SectorExit)       ? SelectedBG: DefaultBG, "    %s  Exit                                                                    \n\n", (op == SectorExit)       ? sfg: "\x1b[38;2;255;255;255m ");
    reset_bg();

}

/**
 * @brief Performs dumps (according to current_sector)
 * @note each call will dump 1 file + extra calls
 * @note number of call per section: Initialize (x1) > Dump (x'numOfFilesToDump') > Finish | Abort | Error (x'waitUser') > Exit (x1)
 */
void sector_dump() {
    static CapsAlbumEntry * entries;            // stores all files entries during initialization for all subsequent calls

    static u64 seek, size;                      // number of entries + entry index to process
    static Section section = SectionInitialize; // defines the current section (Initialize > Dump > Finish | Abort | Error > Exit)

    static struct tm tm;                        // stores time
    static time_t start_t, elapsed_t;  
    static color ci = {0};                      // calls-shared color iterator
    if (!ci.sampling) ci = init_color(84, 0, 0, 0, 0);

    // Print rainbow banner (uniform color which change each frame)
    print("\x1b[38;2;%hhu;%hhu;%hhum\n                    ", ci.r, ci.g, ci.b);
    for (u16 i = 0; i < sizeof(title); i++) if (putchar(title[i]) == '\n') putsb("                    ");
    next_color(&ci);
    reset_fg();
        
    switch (section) {
        case SectionInitialize: {
            if (ci.sampling == 84) { // Artificially speed up the color iter
                ci.sampling >>= 1;
                ci.seek >>= 1;
                ci.end >>= 1;
            }

            getAlbum(); // Get last info from HOS Album

            elapsed_t = seek = 0;
            start_t = time(NULL);
            tm = *gmtime(&elapsed_t);

            entries = malloc(sizeof(CapsAlbumEntry) * (size = (current_sector & SectorScreenshot ? filesNandSs + filesSdSs: 0) +
                                                              (current_sector & SectorMovie      ? filesNandMv + filesSdMv: 0))); // alloc entries arrays for all

            if (entries) { 
                for (u64 cumsize = 0, sz = 0, i = 0; i < 4; i++) if (files_count[i] && current_sector & (1 << (i & 1) << 1)) {
                    capsaGetAlbumFileListEx0((CapsAlbumStorage)(i >> 1), 1 + (i & 1), &sz, entries + cumsize, files_count[i]);
                    cumsize += sz;
                }

                print("\n\n\n\n  Dumping...\n\n\n    Elapsed time: 00:00:00                0 / %lu files \n\n\n\n\n", size);
                progressbar(70, 4, 0, 1);
                puts("\n\n  Press B to abort");

                section = SectionDump;

             } else section = SectionError;
            return;

        } case SectionDump: {
            if (kDown & HidNpadButton_B) section = SectionAbort;
            else if (seek < size) {

                print("\n\n\n\n  Dumping...\n\n\n    Elapsed time: %02d:%02d:%02d                %lu / %lu files \n\n\n\n", tm.tm_hour, tm.tm_min, tm.tm_sec, seek + 1, size);
                
                if (!dumpFile(entries[seek].file_id)) section = SectionError;
                progressbar(70, 4, ++seek, size);
                puts("\n\n  Press B to abort");

            } else section = SectionFinish;

            elapsed_t = difftime(time(NULL), start_t);
            tm = *gmtime(&elapsed_t);
            return;

        } case SectionFinish: {
            cprint(CONSOLE_FG(0, 180, 128), "\n\n\n\n  Dumping finished in %02d:%02d:%02d          %lu / %lu files \n\n\n\n\n\n\n\n", tm.tm_hour, tm.tm_min, tm.tm_sec, size, size);
            break;

        } case SectionError: {
            cprint(CONSOLE_FG(255, 30, 0), "\n\n\n\n  An error occured at %02d:%02d:%02d          %lu / %lu files \nReason: %s\n\n\n\n\n\n\n", tm.tm_hour, tm.tm_min, tm.tm_sec, seek, size, entries ? "Failed to write a file, sdcard may be full": "Not enough memory to load files list");
            break;

        } case SectionAbort: {
            cprint(CONSOLE_FG(255, 140, 0), "\n\n\n\n  Dump aborted at %02d:%02d:%02d              %lu / %lu files \n\n\n\n\n\n\n\n", tm.tm_hour, tm.tm_min, tm.tm_sec, seek, size);
            break;

        } case SectionExit: {
            if (entries) {
                free(entries);
                entries = NULL;
            }
            section = SectionInitialize;
            current_sector = SectorMenu;
            return;
        }
    }

    // Only for Finish | Error | Abort
    if (ci.sampling != 84) { // Restores default color iter speed
        ci.sampling <<= 1;
        ci.seek <<= 1;
        ci.end <<= 1;
    }
    
    progressbar(70, 4, seek, size);
    print("\n\n  \x1b[38;2;%hhu;%hhu;%hhumPress any key to continue...", selectionCI.r, selectionCI.r, selectionCI.r);
    if (kDown) section = SectionExit;
}

int main() {
    
    consoleInit(NULL); // Setup console
	capsaInitialize(); // Init caps:a Album Accessor Service

    capsaResetAlbumMountStatus(CapsAlbumStorage_Nand); // Make sure Nand-drive is mounted in Album
    capsaResetAlbumMountStatus(CapsAlbumStorage_Sd);   // Make sure Sdmc-drive is mounted in Album
    
    padConfigureInput(1, HidNpadStyleSet_NpadStandard); // Setup user input sensor
    padInitializeDefault(&pad);

    getAlbum();                   // Update last known info from Album 
    mkdir("/switch/Album", 0777); // Create output folder

    selectionCI = init_color(360, 80, 100, 0, ColorIterOption_Mirror); // Init a color iter used to make text glowing
    
    // Main loop
    while (appletMainLoop() && current_sector != SectorExit) {
        next_color(&selectionCI);

        padUpdate(&pad); // Read input from controllers
        consoleClear();  // In this homebrew, screen (console) is used once per frame, clearing emulate this behavior

        reset_fg(); // Reset console foreground color
        reset_bg(); // Reset console background color

        kDown = padGetButtonsDown(&pad); // Get inputs

        switch (current_sector) {
            case SectorMenu: { sector_menu(); break; }
            case SectorExit: break;
            default: sector_dump();
        }
            
        consoleUpdate(NULL); // Render console & send frame to screen
    }

    consoleExit(NULL); // Destroy console
    capsaExit();       // Unload caps:a
}


// inject jpg/mp4 failed test
/*void sector_debug() {
    CapsAlbumFileId fi0, fi1, fi2;
    struct stat st;
    mkdir("/switch/Album/in", 0777);
    FILE * f0 = fopen("/switch/Album/in/720p.jpg", "rb"),
         * f1 = fopen("/switch/Album/in/4k.jpg", "rb"),
         * f2 = fopen("/switch/Album/in/ext.mp4", "rb");
    
    fstat(fileno(f0), &st);
    s64 sz0 = st.st_size;
    fstat(fileno(f1), &st);
    s64 sz1 = st.st_size;
    fstat(fileno(f2), &st);
    s64 sz2 = st.st_size;
    
    void * bf0 = malloc(sz0),
         * bf1 = malloc(sz1),
         * bf2 = malloc(sz2);
    fread(bf0, 1, sz0, f0);
    fread(bf1, 1, sz0, f1);
    fread(bf2, 1, sz0, f2);
    fclose(f0);
    fclose(f1);
    fclose(f2);

    print("%p %p %p | %p %p %p\n%s\n%s\n%s\n", f0, f1, f2, bf0, bf1, bf2, bf0, bf1, bf2);
    consoleUpdate(NULL);

    debug(capscInitialize());
    debug(capscInitialize());
    print(">> %d\n", serviceIsActive(capscGetServiceSession()));
    consoleUpdate(NULL);
    Service sx={0};
    consoleUpdate(NULL);
    debug(smGetService(&sx, "caps:a"));
    print(">> %d\n", serviceIsActive(capscGetServiceSession()));
    consoleUpdate(NULL);
    debug(smGetService(&sx, "caps:c"));
    *capscGetServiceSession() = sx;
    debug(LoadShimLibrary());
    print(">> %d\n", serviceIsActive(capscGetServiceSession()));
    consoleUpdate(NULL);
    debug(capscGenerateCurrentAlbumFileId(100000000001000, CapsAlbumFileContents_ScreenShot, &fi0));
    debug(capscGenerateCurrentAlbumFileId(100000000001000, CapsAlbumFileContents_ExtraScreenShot, &fi1));
    debug(capscGenerateCurrentAlbumFileId(100000000001000, CapsAlbumFileContents_Movie, &fi2));
    //debug(capscSaveAlbumScreenShotFileX(&fi0, bf0, sz0));
    debug(capscSaveAlbumScreenShotFileEx(&fi1, 1, 0, 0, bf1, sz1));
    u64 s;
    LibnxError_AlreadyInitialized
    debug(capscOpenAlbumMovieWriteStream(&s, &fi2));
    debug(capscStartAlbumMovieWriteStreamDataSection(s));
    debug(capscSetAlbumMovieWriteStreamDataSize(s, sz2));
    debug(capscWriteDataToAlbumMovieWriteStream(s, 0, bf2, sz2));
    debug(capscEndAlbumMovieWriteStreamDataSection(s));
    debug(capscFinishAlbumMovieWriteStream(s));
    debug(capscCommitAlbumMovieWriteStream(s));
    free(bf0);
    free(bf1);
    free(bf2);
    
    // wait
    consoleUpdate(NULL);
    for (u64 i = 800000000; i>0;i--) if (!(i % 4000000)) {
        printf("-");
        consoleUpdate(NULL);
    }
    current_sector = SectorMenu;
}*/