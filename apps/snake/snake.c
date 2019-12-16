#include <klikaos.h>
#include <windows.h>
#include <stdlib.h>
#include <gfx.h>

#include "draw.h"
#include "util.h"

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)

window_t  *window;
context_t *context;
message_t msg;

bool game_over;
bool menu = true;
bool sub_menu = false;
bool game_started = false;
const int width = 20;
const int height = 20;
int x, y, fruit_x, fruit_y, score, hiscore = 0;
int tail_x[100], tail_y[100];
int obstacle_x[10], obstacle_y[10];
int tail_counter, obstacle_counter, direction;
char score_label[64];
char hiscore_label[64];
int mode;

void rand_fruit();
void rand_obstacles();
void setup();
void input_check();
bool is_key_press(message_t *msg);
bool is_mouse_click(message_t *msg);
void handle_input(message_t *msg);
void move_player();
void set_highscore();
void handle_game_over();
void no_walls_transition();
void draw();

void setup() {
	game_over = false;
	game_started = true;
	direction = -1;
	x = width / 2;
	y = height / 2;
	score = 0;

	tail_counter = 3;
	tail_x[0] = 11;
	tail_x[1] = 12;
	tail_y[0] = 10;
	tail_y[1] = 10;

	if (mode == 2) {
		obstacle_counter = 10;
		rand_obstacles();
	}

	rand_fruit();

	sprintf(score_label, "SCORE: %04d", score);
	sprintf(hiscore_label, "BEST:  %04d", hiscore);
}

void draw() {
	draw_background(context, window);
	draw_footer(context, window);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (game_over) {
				draw_object(context, j, i, GAME_OVER_COLOR);
			} else {
				if (!menu && !sub_menu) {
					if (i == y && j == x) {
						draw_object(context, j, i, SNAKE_HEAD_COLOR);
					} else if (i == fruit_y && j == fruit_x) {
						draw_object(context, j, i, FRUIT_COLOR);
					} else {
						for (int k = 0; k < tail_counter - 1; k++) {
							if (tail_x[k] == j && tail_y[k] == i) {
								draw_object(context, j, i, SNAKE_BODY_COLOR);
							}
						}
						if (mode == 2) {
							for (int k = 0; k < obstacle_counter - 1; k++) {
								if (obstacle_x[k] == j && obstacle_y[k] == i) {
									draw_object(context, j, i, OBSTACLE_COLOR);
								}
							}
						}
					}
				}
			}
		}
	}

	draw_scores(context, score_label, hiscore_label);
	draw_menu(context, sub_menu);

	window_present(window);

	if (game_over) {
		// TODO: replace with timer func to display gameOver screen for specific ms duration
		for (int c = 1; c <= 4000; c++)
			for (int d = 1; d <= 4000; d++)
			{}
		game_over = false;
	}
}

bool is_key_press(message_t *msg) {
	return msg->message == MESSAGE_KEY_PRESS || msg->message == MESSAGE_KEY_RELEASE;
}

bool is_mouse_click(message_t *msg) {
	return msg->message == MESSAGE_MOUSE_PRESS || msg->message == MESSAGE_MOUSE_RELEASE;
}

void input_check() {
	window_peek_message(window, &msg);

	if (msg.message != 0) {
		// TODO: make generic handler for click events on non-button elements
		if (is_mouse_click(&msg) && msg.message == MESSAGE_MOUSE_PRESS) {
			int click_x = msg.x - window->x;
			int click_y = msg.y - window->y;

			if ((click_x < 120 && click_x > 15) && (click_y < 485 && click_y > 455)) {
				sub_menu = true;
			} else if ((click_x < 270 && click_x > 135) && (click_y < 230 && click_y > 200) && sub_menu) {
				mode = 0;
				setup();
				menu = false;
				sub_menu = false;
			} else if ((click_x < 270 && click_x > 135) && (click_y < 270 && click_y > 240) && sub_menu) {
				mode = 1;
				setup();
				menu = false;
				sub_menu = false;
			} else if ((click_x < 270 && click_x > 135) && (click_y < 310 && click_y > 280) && sub_menu) {
				mode = 2;
				setup();
				menu = false;
				sub_menu = false;
			}
		}
		if (is_key_press(&msg) && msg.message == MESSAGE_KEY_PRESS) {
			switch (msg.key) {
			case KEY_LEFT:
				if (direction != 1)
					direction = -1;
				break;
			case KEY_RIGHT:
				if (direction != -1)
					direction = 1;
				break;
			case KEY_UP:
				if (direction != 2)
					direction = -2;
				break;
			case KEY_DOWN:
				if (direction != -2)
					direction = 2;
				break;
			}
		}

		window_dispatch(window, &msg);
	}
}

void move_player() {
	int prev_x = tail_x[0];
	int prev_y = tail_y[0];
	int prev_2x, prev_2y;
	tail_x[0] = x;
	tail_y[0] = y;

	for (int i = 1; i < tail_counter; i++) {
		prev_2x = tail_x[i];
		prev_2y = tail_y[i];
		tail_x[i] = prev_x;
		tail_y[i] = prev_y;
		prev_x = prev_2x;
		prev_y = prev_2y;
	}

	switch (direction) {
	case -1:
		x--;
		break;
	case 1:
		x++;
		break;
	case -2:
		y--;
		break;
	case 2:
		y++;
		break;
	}

	switch (mode) {
	case 0:
		if (x > width - 1 || x == -1 || y > height - 1 || y == -1) {
			handle_game_over();
		}
		break;
	case 1:
		no_walls_transition();
		break;
	case 2:
		no_walls_transition();
		for (int i = 0; i < obstacle_counter; i++) {
			if (obstacle_x[i] == x && obstacle_y[i] == y) {
				handle_game_over();
				break;
			}
		}
		break;
	}

	for (int i = 0; i < tail_counter; i++) {
		if (tail_x[i] == x && tail_y[i] == y) {
			handle_game_over();
		}
	}

	if (x == fruit_x && y == fruit_y) {
		score += 10;
		tail_counter++;
		sprintf(score_label, "SCORE: %04d", score);
		rand_fruit();
	}
}

void no_walls_transition() {
	if (x > width - 1) {
		x = 0;
	} else if (x == -1) {
		x = width - 1;
	}

	if (y > height - 1) {
		y = 0;
	} else if (y == -1) {
		y = height - 1;
	}
}

void set_highscore() {
	if (score > hiscore) {
		hiscore = score;
		sprintf(hiscore_label, "BEST:  %04d", hiscore);
	}
}

void rand_fruit() {
	// TODO: replace with timer func to get actual random values
	int temp_x = rand() % width;
	int temp_y = rand() % height;

	for (int i = 0; i < tail_counter; i++) {
		if (tail_x[i] == temp_x && tail_y[i] == temp_y) {
			return rand_fruit();
		}
	}

	for (int i = 0; i < obstacle_counter; i++) {
		if (obstacle_x[i] == temp_x && obstacle_y[i] == temp_y) {
			return rand_fruit();
		}
	}

	fruit_x = temp_x;
	fruit_y = temp_y;
}

void rand_obstacles() {
	for (int i = 0; i < obstacle_counter; i++) {
		// TODO: replace with timer func to get actual random values
		int tx = rand() % width;
		int ty = rand() % height;

		if (ty == 10 || (fruit_x == tx && fruit_y == ty)) {
			obstacle_x[i] = obstacle_x[i - 1];
			obstacle_y[i] = obstacle_y[i - 1];
		} else {
			obstacle_x[i] = tx;
			obstacle_y[i] = ty;
		}
	}
}

void handle_game_over() {
	game_over = true;
	game_started = false;
	set_highscore();
}

int main() {
	window = window_create(312, 183, 407, 500, "Snek Classic", MSG_USER_WIN, WINDOW_ATTR_NONE, WINDOW_FRAME_DEFAULT);
	context = WINDOW_EXT(window)->context;

	setup();

	for (;;) {
		input_check();

		if (!menu && !sub_menu && game_started) {
			move_player();

			// TODO: replace with timer func to reduce speed
			for (int c = 1; c <= 3500; c++)
				for (int d = 1; d <= 3500; d++)
				{}
		}

		draw();
	}

	return 0;
}

