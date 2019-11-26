#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <syscalls.h>
#include <klika_os.h>

FILE* fopen(char* filename, char* mode) {
  FILE file;

  if (!strcmp(mode, "r")) {
    if (syscall(SYSCall_file_open, filename, FILE_MODE_READ, file.scratch_sector, &file.file_info)) {
      DEBUG("stdio.c: error opening file %s\n", filename);
      return NULL;
    }
  }
  else {
    DEBUG("stdio.c: Unsupported mode. only \"r\" supported!\n");
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

  return read_count;
}

int fclose(FILE *stream) {
  free(stream);
  return 0;
}

uint32_t fsize(FILE *stream) {
  return stream->file_info.filelen;
}