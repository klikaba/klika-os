#pragma once

#include <klikaos.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <kv.h>

typedef struct {
	char* display_name;
	char* executable_path;
	bool visible;
	char* icon_path;
} app_info_t;

char* app_names[64];
int app_name_count = 0;
app_info_t* apps[64];
int app_count = 0;

void load_apps_from_directory(char* dirname) {
	DIR *dp;
	DIRENT *ep;     
	dp = opendir(dirname);

	if (dp != NULL)
	{
		while (ep = readdir (dp)) {
			if (ep->name[0] != '.') {
				// Apps are expected to be inside separate directories
				if (ep->attr & ATTR_DIRECTORY) {
					app_names[app_name_count++] = file_name(ep);
				}
			}
		}

		closedir (dp);
	}
	else
		DEBUG("Couldn't open the directory (%s)\n", dirname);
}

app_info_t* prep_app_info(int app_index) {
	app_info_t* app_info = malloc(sizeof(app_info_t));
	app_info->display_name = strdup(app_names[app_index]);
	char exe_path[64];
	sprintf(exe_path, "/apps/%s/%s", app_names[app_index], app_names[app_index]);
	app_info->executable_path = strdup(exe_path);
	app_info->icon_path = strdup("/apps/desktop/appicon.bmp");
	app_info->visible = true;
	return app_info;
}

char* resolve_file_path(int app_index, char* read_path) {
	if (*read_path == '/') {
		// This is absolute path
		return strdup(read_path);
	} else {
		// This is relative path
		char resolved_path[64];
		sprintf(resolved_path, "/apps/%s/%s", app_names[app_index], read_path);
		return strdup(resolved_path);
	}
}

void load_default_icon(int app_index, app_info_t* app_info) {
	char default_icon_path[64];
	sprintf(default_icon_path, "/apps/%s/appicon.bmp", app_names[app_index]);
	FILE* icon_file = fopen(default_icon_path, "r");
	if (icon_file != NULL) {
		fclose(icon_file);
		free(app_info->icon_path);
		app_info->icon_path = strdup(default_icon_path);
	}
}

void load_icon_info(int app_index, char* icon_path, app_info_t* app_info) {
	icon_path = resolve_file_path(app_index, icon_path);
	FILE* icon_file = fopen(icon_path, "r");
	if (icon_file == NULL) {
		load_default_icon(app_index, app_info);
		free(icon_path);
	} else {
		fclose(icon_file);
		free(app_info->icon_path);
		app_info->icon_path = strdup(icon_path);
		free(icon_path);
	}
}

void load_exe_info(int app_index, char* exe_path, app_info_t* app_info) {
	exe_path = resolve_file_path(app_index, exe_path);
	FILE* exe_file = fopen(exe_path, "r");
	if (exe_file != NULL) {
		fclose(exe_file);
		free(app_info->executable_path);
		app_info->executable_path = strdup(exe_path);
		free(exe_path);
	}
}

void load_kv_for_app(int app_index) {
	app_info_t* app_info = prep_app_info(app_index);

	kv_file_t* kv_file = malloc(sizeof(kv_file_t));
	char kv_path[64];
	int i;
	// TODO sprintf does not add null terminator properly (if target buffer already has some value)
	for (i = 0; i < 64; i++) kv_path[i] = 0;
	sprintf(kv_path, "/apps/%s/info.kv", app_names[app_index]);
	open_kv_file(kv_path, kv_file);

	bool icon_set_up = false;
	for (i = 0; i < kv_file->entry_count; i++) {
		if (!strcmp(kv_file->keys[i], "icon")) {
			icon_set_up = true;
			load_icon_info(app_index, kv_file->values[i], app_info);
		}
		if (!strcmp(kv_file->keys[i], "name")) {
			free(app_info->display_name);
			app_info->display_name = strdup(kv_file->values[i]);
		}
		if (!strcmp(kv_file->keys[i], "executable")) {
			load_exe_info(app_index, kv_file->values[i], app_info);
		}
		if (!strcmp(kv_file->keys[i], "visible")) {
			app_info->visible = (*kv_file->values[i] - '0');
		}
	}

	if (!icon_set_up) load_default_icon(app_index, app_info);
	apps[app_count++] = app_info;
	close_kv_file(kv_file);
}

void populate_app_info() {
	load_apps_from_directory("/apps");
	int i;
	for (i = 0; i < app_name_count; i++) {
		load_kv_for_app(i);
	}
}
