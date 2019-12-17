#pragma once

#include <stdint.h>
#include <stdio.h>

typedef struct {
		char** keys;
		char** values;
		int entry_count;
} kv_file_t;

kv_file_t* create_empty_kv();
void add_kv_entry(kv_file_t* kv_file, char* key, char* value);
void open_kv_file(char* filename, kv_file_t* kv_file_out);
void create_kv_file(char* filename, kv_file_t* kv_file_data);
void close_kv_file(kv_file_t* file);
