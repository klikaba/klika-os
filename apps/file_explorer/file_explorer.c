#include <klikaos.h>
#include <windows.h>
#include <malloc.h>
#include <stdlib.h>
#include <gfx.h>
#include <dirent.h>

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)
#define MSG_USER_BUTTON (WINDOW_USER_MESSAGE + 2)

typedef struct {
	window_t* button;
	char* full_path;
	bool can_open;
} file_node_t;

message_t msg;
window_t  *window;

file_node_t* nodes[20] = { 0 };
int node_counter = 0;

char* get_parent_path(char* current_path) {
	if (strlen(current_path) <= 1) return strdup(current_path);
	char* last_separator = current_path;
	char* iter = current_path;
	char character;
	while ((character = *++iter) != '\0') {
		if (character == '/') last_separator = iter;
	}
	char* result = malloc(sizeof(char) * (last_separator - current_path + 1));
	char* result_iter = result;
	iter = current_path;
	while (iter < last_separator) {
		*result_iter++ = *iter++;
	}
	*result_iter = '\0';
	return result;
}

file_node_t* build_file_node() {
	file_node_t* ptr = malloc(sizeof(file_node_t));
	ptr->button = NULL;
	ptr->full_path = NULL;
	ptr->can_open = false;
	return ptr;
}

void load_directory_entries(char* dirname) {
	int i;
	// Hack to remove buttons
	window->next->next->next = NULL;
	for (i = 0; i < node_counter; i++) {
		free(nodes[i]->full_path);
		free(nodes[i]->button);
		free(nodes[i]);
		nodes[i] = NULL;
	}
	node_counter = 0;

	free(window->title);
	window->title = strdup(dirname);

	int layout_y = WINDOW_BAR_HEIGHT + 10;
	DIR *dp;
	DIRENT *ep;     
	dp = opendir(dirname);

	if (dp != NULL)
	{
		while (ep = readdir(dp)) {
			if (ep->name[0] != '.' || ep->name[1] == '.') {
				char* full_name = file_name(ep);
				nodes[node_counter] = build_file_node();
				if (ep->name[1] == '.') {
					nodes[node_counter]->full_path = get_parent_path(dirname);
				} else {
					char full_path[64] = { 0 };
					sprintf(full_path, "%s/%s", dirname, full_name);
					nodes[node_counter]->full_path = strdup(full_path);
				}
				nodes[node_counter]->can_open = ep->attr & ATTR_DIRECTORY;
				nodes[node_counter]->button = button_create(window, 10, layout_y + 30 * node_counter, 100, 20, full_name, MSG_USER_BUTTON + node_counter);
				node_counter++;
				free(full_name);
			}
		}

		closedir(dp);
	}
	else
		DEBUG("Couldn't open the directory (%s)\n", dirname);

	window_invalidate(window);
}

int main() {
	window = window_create(250, 100, 300, 500, "/", MSG_USER_WIN, WINDOW_ATTR_NONE, WINDOW_FRAME_DEFAULT);

	load_directory_entries("");

	while(window_get_message(window, &msg)) { 
		int btn_index = msg.message - MSG_USER_BUTTON;
		if (btn_index >= 0 && btn_index < node_counter) {
			if (nodes[btn_index]->can_open) {
				char* dir_to_open = strdup(nodes[btn_index]->full_path);
				load_directory_entries(dir_to_open);
				free(dir_to_open);
			}
		}
		window_dispatch(window, &msg);
	}
	return 0;
}
