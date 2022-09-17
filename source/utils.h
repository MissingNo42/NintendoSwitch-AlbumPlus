#pragma once

// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

// Include the main libnx system header, for Switch development
#include <switch.h>

// Include local
#include "console.h"


#define filesNandSs files_count[0]
#define filesNandMv files_count[1]
#define filesSdSs   files_count[2]
#define filesSdMv   files_count[3]

#define StorageNand 0
#define StorageSd   1

#define align(size, on) ((size) + ((on) - (size) % (on))) // round up a value to a multiple of *on*

typedef struct {
	void * buf; // buffer
	u64 size;   // used size (<= real size) 
} buffer;

extern bool mount[2];      // { StorageNand, StorageSd }
extern u64 drive_size[2];  // { Nand, Sd }
extern u64 files_count[4]; // { ndss, ndmv, sdss, sdmv }


void getAlbum();

void printSize(u64 size);

buffer loadMovie(CapsAlbumFileId *);
buffer loadScreenShot(CapsAlbumFileId *);

bool dumpFile(CapsAlbumFileId);