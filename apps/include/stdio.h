#pragma once

#include <stdint.h>

#define SECTOR_SIZE		512		// sector size in bytes

#define FILE_MODE_READ		1			// read-only
#define FILE_MODE_WRITE		2			// write-only


typedef struct _tagVOLINFO {
	uint8_t unit;				// unit on which this volume resides
	uint8_t filesystem;			// formatted filesystem

	uint8_t label[12];			// volume label ASCIIZ
	uint32_t startsector;		// starting sector of filesystem
	uint8_t secperclus;			// sectors per cluster
	uint16_t reservedsecs;		// reserved sectors
	uint32_t numsecs;			// number of sectors in volume
	uint32_t secperfat;			// sectors per FAT
	uint16_t rootentries;		// number of root dir entries

	uint32_t numclusters;		// number of clusters on drive

	// The fields below are PHYSICAL SECTOR NUMBERS.
	uint32_t fat1;				// starting sector# of FAT copy 1
	uint32_t rootdir;			// starting sector# of root directory (FAT12/FAT16) or cluster (FAT32)
	uint32_t dataarea;			// starting sector# of data area (cluster #2)
}  __attribute__ ((__packed__)) VOLINFO, *PVOLINFO;


typedef struct _tagFILEINFO {
	PVOLINFO volinfo;			// VOLINFO used to open this file
	uint32_t dirsector;			// physical sector containing dir entry of this file
	uint8_t diroffset;			// # of this entry within the dir sector
	uint8_t mode;				// mode in which this file was opened
	uint32_t firstcluster;		// first cluster of file
	uint32_t filelen;			// byte length of file

	uint32_t cluster;			// current cluster
	uint32_t pointer;			// current (BYTE) pointer
}  __attribute__ ((__packed__)) FILEINFO, *PFILEINFO;


typedef struct {
  uint8_t scratch_sector[SECTOR_SIZE];
  FILEINFO file_info;  
} FILE;


FILE* fopen(char* filename, char* mode);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(void *ptr, size_t size, size_t nmemb, FILE *stream);
int fclose(FILE *stream);
uint32_t fsize(FILE *stream);
