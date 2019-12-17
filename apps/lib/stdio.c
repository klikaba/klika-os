#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <syscalls.h>
#include <klikaos.h>

FILE* fopen(char* filename, char* mode) {
  FILE file;

  if (!strcmp(mode, "r")) {
    if (syscall(SYSCall_file_open, filename, FILE_MODE_READ, file.scratch_sector, &file.file_info)) {
      DEBUG("stdio.c: error opening file %s\n", filename);
      return NULL;
    }
  } else if (!strcmp(mode, "w")){
    if (syscall(SYSCall_file_open, filename, FILE_MODE_WRITE, file.scratch_sector, &file.file_info)) {
      DEBUG("stdio.c: error opening file %s\n", filename);
      return NULL;
    }
  } else {
    DEBUG("stdio.c: Unsupported mode. only \"r\" and \"w\" supported!\n");
    return NULL;
	}

  // TODO: Can this be more cleaner?? 
  FILE *ret_file = malloc(sizeof(FILE));
  *ret_file = file;
  return ret_file;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  uint32_t read_count;

  syscall(SYSCall_file_read, &stream->file_info, stream->scratch_sector, ptr, &read_count, size * nmemb);

  return read_count / size;
}

size_t fwrite(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	uint32_t written_count;

	syscall(SYSCall_file_write, &stream->file_info, stream->scratch_sector, ptr, &written_count, size * nmemb);

	return written_count / size;
}

int fclose(FILE *stream) {
  free(stream);
  return 0;
}

uint32_t fsize(FILE *stream) {
  return stream->file_info.filelen;
}
