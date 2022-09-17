#pragma once

// Include the most common headers from the C standard library
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>

// Include the main libnx system header, for Switch development
#include <switch.h>


// Include local
#include "utils.h"
#include "console.h"

   
typedef enum {
    SectorMenu       = 0b000001,
    SectorScreenshot = 0b000010,
    SectorMovie      = 0b000100,
    SectorBoth       = 0b000110, // flag
    SectorExit       = 0b001000,
} Sector;

typedef enum {
    SectionInitialize,
    SectionDump,
    SectionFinish,
    SectionAbort,
    SectionExit,
    SectionError,
} Section;

Sector current_sector = SectorMenu;
PadState pad;
u64 kDown = 0;
color selectionCI;

const char title[] = "\xda\xc4\xc4\xc4\xc4\xc4\xbf\xda\xc4\xbf  \xda\xc4\xbf                   \xda\xc4\xbf  \n\xb3 \xda\xc4\xbf \xb3\xb3 \xb3  \xb3 \xb3                 \xda\xc4\xd9 \xc0\xc4\xbf\n\xb3 \xb3 \xb3 \xb3\xb3 \xb3  \xb3 \xc0\xc4\xc4\xbf\xda\xc4\xbf \xda\xc4\xbf\xda\xc4\xbf \xda\xc4\xbf\xc0\xc4\xbf \xda\xc4\xd9\n\xb3 \xc0\xc4\xd9 \xb3\xb3 \xb3  \xb3 \xda\xbf \xb3\xb3 \xb3 \xb3 \xb3\xb3 \xc0\xc4\xd9 \xb3  \xc0\xc4\xd9  \n\xb3 \xda\xc4\xbf \xb3\xb3 \xc0\xc4\xbf\xb3 \xc0\xd9 \xb3\xb3 \xc0\xc4\xd9 \xb3\xb3 \xb3 \xb3 \xb3       \n\xc0\xc4\xd9 \xc0\xc4\xd9\xc0\xc4\xc4\xc4\xd9\xc0\xc4\xc4\xc4\xc4\xd9\xc0\xc4\xc4\xc4\xc4\xc4\xd9\xc0\xc4\xc1\xc4\xc1\xc4\xd9       \n";
const char * const DefaultBG  = CONSOLE_BG(51, 51, 51);
const char * const SelectedBG = CONSOLE_BG(60, 60, 60);

void sector_menu();
void sector_dump();