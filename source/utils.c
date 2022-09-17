#include "utils.h"


bool mount[2];      		// { StorageNand, StorageSd }
u64 drive_size[2] = {0, 0}; // { Nand, Sd }
u64 files_count[4];			// { ndss, ndmv, sdss, sdmv }


void getAlbum() {
	for (u8 i = 0; i < 4; i++) {
		if (i < 2) capsaIsAlbumMounted((CapsAlbumStorage)i, &mount[i]);
		if (mount[i >> 1]) capsaGetAlbumFileCountEx0((CapsAlbumStorage)(i >> 1), 1 + (i & 1), &files_count[i]);
		else files_count[i] = 0;
	}
	capsaGetRequiredStorageSpaceSizeToCopyAll(CapsAlbumStorage_Nand, CapsAlbumStorage_Sd, drive_size + 1);
	capsaGetRequiredStorageSpaceSizeToCopyAll(CapsAlbumStorage_Sd, CapsAlbumStorage_Nand, drive_size);
}


void printSize(u64 size) {
	console_fg(255, 140, 0);
	if      (size >= 1024 << 20) print("%*.1f GiB\n", 5, (double)(size) / (1024 << 20));
	else if (size >= 1024 << 10) print("%*.1f MiB\n", 5, (double)(size) / (1024 << 10));
	else                         print("%*.1f KiB\n", 5, (double)(size) / (1024 << 00));
}


buffer loadMovie(CapsAlbumFileId * file) {
	u64 stream, size;
	buffer bf;

	capsaOpenAlbumMovieStream(&stream, file);    // Open movie stream handler
	capsaGetAlbumMovieStreamSize(stream, &size); // Get minimal buffer size to load file
	
	if ((bf.buf = malloc(size = align(size, 0x40000)))) capsaReadMovieDataFromAlbumMovieReadStream(stream, 0, bf.buf, size, &bf.size); // Load buffer (buffer size must be align to 0x40000)
	capsaCloseAlbumMovieStream(stream); // Close handler

	return bf;
}


buffer loadScreenShot(CapsAlbumFileId * file) {
    u64 size;
	buffer bf;

    capsaGetAlbumFileSize(file, &size); // Get minimal buffer size to load file

	if ((bf.buf = malloc(size))) capsaLoadAlbumFile(file, &bf.size, bf.buf, size); // Load buffer

	return bf;
}


bool dumpFile(CapsAlbumFileId file) {
    char fn[512]; // compute out filename
    snprintf(fn, 512, "/switch/Album/%04hu-%02hhu-%02hhu-%02hhu%02hhu%02hhu-%02hhx-%016lx.%s", file.datetime.year, file.datetime.month, file.datetime.day,
    file.datetime.hour, file.datetime.minute, file.datetime.second, file.datetime.id, file.application_id, (file.content & CapsAlbumFileContents_Movie) ? "mp4": "jpg");
    
    cprint(CONSOLE_FG(180, 180, 180), "    sdcard:/%s ...\n", fn);

    buffer bf = (file.content & CapsAlbumFileContents_Movie) ? loadMovie(&file) : loadScreenShot(&file); // Load file from Album
	if (!bf.buf) return false;

    FILE * f = fopen(fn, "wb");
	bool r = f;
	
    if (r) {
		r = bf.size == fwrite(bf.buf, 1, bf.size, f);		
    	fclose(f);
	}
    
    free(bf.buf);
    return r;
}
