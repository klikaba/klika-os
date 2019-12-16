#pragma once

#include <stdint.h>
#include <stdio.h>

typedef struct {
		char** keys;
		char** values;
		int entry_count;
} kv_file_t;

void open_kv_file(char* filename, kv_file_t* kv_file_out);
void close_kv_file(kv_file_t* file);
