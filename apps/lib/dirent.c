#include <dirent.h>
#include <malloc.h>
#include <string.h>
#include <syscalls.h>
#include <klikaos.h>

DIR* opendir(char* dirname) {
  DIR* dir = malloc(sizeof(DIR));
  dir->dir_info.scratch = malloc(sizeof(uint8_t) * SECTOR_SIZE);

  if (syscall(SYSCall_dir_open, dirname, &dir->dir_info)) {
	  DEBUG("dirent.c: error opening dir %s\n", dirname);
	  closedir(dir);
	  return NULL;
  }

  return dir;
}

DIRENT* readdir(DIR* stream) {
  // Based on standard dirent.h returned DIRENT should not be managed by user
  static DIRENT dirent;

  if (syscall(SYSCall_dir_read_next, &stream->dir_info, &dirent)) {
	  DEBUG("dirent.c: error reading from dir\n");
	  return NULL;
  }

  return &dirent;
}

int closedir(DIR* stream) {
  if (stream != NULL) free(stream->dir_info.scratch);
  free(stream);
  return 0;
}

char* file_name(DIRENT* dirent) {
  char* name = malloc(sizeof(char) * 13);
  char* name_start = name;
  char* dirent_name = (char*) dirent->name;
  char* dirent_name_end = dirent_name + 8;
  char* dirent_end = dirent_name + 11;
  while (dirent_name < dirent_end) {
    if (dirent_name == dirent_name_end && *dirent_name_end != ' ') *name++ = '.';
    if (*dirent_name != ' ') *name++ = *dirent_name;
    dirent_name++;
  }
  *name = '\0';
  return name_start;
}
