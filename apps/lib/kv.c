#include <kv.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <syscalls.h>
#include <klikaos.h>

#define READ_MODE_KEY 0
#define READ_MODE_VALUE 1
#define READ_MODE_COMMENT 2
#define READ_MODE_NEWLINE 3

#define MAX_PAIRS 64

char buf_getc(uint8_t* ptr, int index, int size) {
	return index >= size ? -1 : ptr[index];
}

kv_file_t* create_empty_kv() {
	kv_file_t* kv = malloc(sizeof(kv_file_t));
	kv->entry_count = 0;
	kv->keys = malloc(sizeof(char*) * MAX_PAIRS);
	kv->values = malloc(sizeof(char*) * MAX_PAIRS);
	return kv;
}

void add_kv_entry(kv_file_t* kv_file, char* key, char* value) {
	if (kv_file->entry_count >= MAX_PAIRS) return;
	kv_file->keys[kv_file->entry_count] = strdup(key);
	kv_file->values[kv_file->entry_count] = strdup(value);
	kv_file->entry_count++;
}

void open_kv_file(char* filename, kv_file_t* kv_file_out) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		return;
	}
	uint32_t size = fsize(file);

	uint8_t *buffer = malloc(size + 512);

	fread(buffer, size, 1, file);

	int entry_count = 0;
	int line_count = 0;
	int char_count = 0;
	uint32_t i;
	int mode = READ_MODE_NEWLINE;
	char** keys_holder = malloc(sizeof(char*) * MAX_PAIRS);
	char** values_holder = malloc(sizeof(char*) * MAX_PAIRS);
	char** keys = keys_holder;
	char** values = values_holder;
	char* key_holder = malloc(sizeof(char) * 64);
	char* value_holder = malloc(sizeof(char) * 64);
	char* key = key_holder;
	char* value = value_holder;
	for (i = 0; i <= size; i++) {
		switch(buf_getc(buffer, i, size)) {
			case '\n':
			case -1:
				if (key != key_holder && value != value_holder) {
					*key = '\0';
					*value = '\0';
					*keys++ = strdup(key_holder);
					*values++ = strdup(value_holder);
					key = key_holder;
					value = value_holder;
					entry_count++;
					if (entry_count == MAX_PAIRS) {
						// We reached max pairs, leave
						DEBUG("kv.c: Max KV pairs reached, cancelling");
						return;
					}
				} else if (key == key_holder) {
					// Ignore, probably an empty line
				} else if (mode != READ_MODE_NEWLINE && mode != READ_MODE_COMMENT) {
					DEBUG("kv.c: Something went wrong while reading a KV file (no key)");
					DEBUG("Stopped at line %d, character %d", line_count, char_count);
					return;
				}
				mode = READ_MODE_NEWLINE;
				line_count++;
				char_count = 0;
				break;
			case '=':
				if (mode == READ_MODE_KEY && key != key_holder) {
					mode = READ_MODE_VALUE;
				} else if (mode != READ_MODE_COMMENT) {
					// ERROR!
					DEBUG("kv.c: Key is missing, but '=' sign is reached. Exiting!");
					DEBUG("Stopped at line %d, character %d", line_count, char_count);
					return;
				}
				break;
			case '#':
				if (mode == READ_MODE_NEWLINE || (mode == READ_MODE_VALUE && value != value_holder)) {
					mode = READ_MODE_COMMENT;
				} else if (mode != READ_MODE_COMMENT) {
					// EXIT WITH ERROR
					DEBUG("kv.c: Reached comment sign in middle of parsing! Exiting!");
					DEBUG("Stopped at line %d, character %d", line_count, char_count);
					return;
				}
				break;
			default:
				// Ignore spaces completely, unless its in value part
				if (mode == READ_MODE_VALUE) {
						*value++ = buffer[i];
				} else if (!isspace(buffer[i])) {
					if (mode == READ_MODE_NEWLINE) mode = READ_MODE_KEY;
					if (mode == READ_MODE_KEY) {
						*key++ = buffer[i];
					} 
				}
				break;
		}
		char_count++;
	}

	// All went well
	kv_file_out->entry_count = entry_count;
	kv_file_out->keys = keys_holder;
	kv_file_out->values = values_holder;

	fclose(file);
	free(buffer);
	free(key_holder);
	free(value_holder);
}

void create_kv_file(char* filename, kv_file_t* kv_file_data) {
	FILE *file = fopen(filename, "w");
	if (file == NULL) {
		return;
	}
	uint32_t size = 0;
	int i;
	for (i = 0; i < kv_file_data->entry_count; i++) {
		// key=value\n
		size += strlen(kv_file_data->keys[i]) + 1 + strlen(kv_file_data->values[i]) + 1;
	}
	uint8_t *buffer = malloc(sizeof(char) * size + 1);
	uint8_t *iter = buffer;
	for (i = 0; i < kv_file_data->entry_count; i++) {
		char next;
		char* key = kv_file_data->keys[i];
		char* value = kv_file_data->values[i];
		while((next = *key++) != '\0') *iter++ = next;
		*iter++ = '=';
		while((next = *value++) != '\0') *iter++ = next;
		*iter++ = '\n';
		*iter = '\0';
	}
	fwrite(buffer, sizeof(char), size, file);
	fclose(file);
}

void close_kv_file(kv_file_t* file) {
	if (file != NULL) {
		int i;
		for (i = 0; i < file->entry_count; i++) {
			if (file->keys != NULL) free(file->keys[i]);
			if (file->values != NULL) free(file->values[i]);
		}
		free(file->keys);
		free(file->values);
		free(file);
	}
}
