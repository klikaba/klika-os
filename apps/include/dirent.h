#pragma once

#include <stdint.h>
#include <stdio.h>

#define ATTR_READ_ONLY	0x01
#define ATTR_HIDDEN		0x02
#define ATTR_SYSTEM		0x04
#define ATTR_VOLUME_ID	0x08
#define ATTR_DIRECTORY	0x10
#define ATTR_ARCHIVE	0x20
#define ATTR_LONG_NAME	(ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)

typedef struct _tagDIRINFO {
	uint32_t currentcluster;	// current cluster in dir
	uint8_t currentsector;		// current sector in cluster
	uint8_t currententry;		// current dir entry in sector
	uint8_t *scratch;			// ptr to user-supplied scratch buffer (one sector)
	uint8_t flags;				// internal DOSFS flags
}  __attribute__ ((__packed__)) DIRINFO, *PDIRINFO;

typedef struct _tagDIRENT {
	uint8_t name[11];			// filename
	uint8_t attr;				// attributes (see ATTR_* constant definitions)
	uint8_t reserved;			// reserved, must be 0
	uint8_t crttimetenth;		// create time, 10ths of a second (0-199 are valid)
	uint8_t crttime_l;			// creation time low byte
	uint8_t crttime_h;			// creation time high byte
	uint8_t crtdate_l;			// creation date low byte
	uint8_t crtdate_h;			// creation date high byte
	uint8_t lstaccdate_l;		// last access date low byte
	uint8_t lstaccdate_h;		// last access date high byte
	uint8_t startclus_h_l;		// high word of first cluster, low byte (FAT32)
	uint8_t startclus_h_h;		// high word of first cluster, high byte (FAT32)
	uint8_t wrttime_l;			// last write time low byte
	uint8_t wrttime_h;			// last write time high byte
	uint8_t wrtdate_l;			// last write date low byte
	uint8_t wrtdate_h;			// last write date high byte
	uint8_t startclus_l_l;		// low word of first cluster, low byte
	uint8_t startclus_l_h;		// low word of first cluster, high byte
	uint8_t filesize_0;			// file size, low byte
	uint8_t filesize_1;			//
	uint8_t filesize_2;			//
	uint8_t filesize_3;			// file size, high byte
}  __attribute__ ((__packed__)) DIRENT, *PDIRENT;

typedef struct {
  DIRINFO dir_info;
} DIR;


int closedir(DIR* stream);
DIR* opendir(char* dirname);
DIRENT* readdir(DIR* stream);
char* file_name(DIRENT* dirent);
