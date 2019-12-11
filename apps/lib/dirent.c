#include <dirent.h>
#include <malloc.h>
#include <string.h>
#include <syscalls.h>
#include <klikaos.h>

DIR* opendir(char* dirname) {
  DIR dir;

	if (syscall(SYSCall_dir_open, dirname, &dir.dir_info)) {
		DEBUG("stdio.c: error opening dir %s\n", dirname);
		return NULL;
	}

  DIR *ret_dir = malloc(sizeof(DIR));
  *ret_dir = dir;
  return ret_dir;
}

DIRENT* readdir(DIR* stream) {
	DIRENT dirent;

	if (syscall(SYSCall_dir_read_next, &stream->dir_info, &dirent)) {
		DEBUG("stdio.c: error reading from dir %s\n", stream->ent.name);
		return NULL;
	}

  DIRENT *ret_dirent = malloc(sizeof(DIRENT));
  *ret_dirent = dirent;
  return ret_dirent;
}

int closedir(DIR* stream) {
	free(stream);
	return 0;
}
