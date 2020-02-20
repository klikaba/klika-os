#ifndef __UTIL_H
#define __UTIL_H

#define RGB(r,g,b) (((r) << 16) | ((g) << 8) | ((b) & 0xFF))
#define KEY_LEFT 150
#define KEY_DOWN 152
#define KEY_UP 151
#define KEY_RIGHT 153
#define SPACE 32

#define BACKGROUND_COLOR RGB(133, 181, 189)
#define SNAKE_HEAD_COLOR RGB(51, 110, 123)
#define SNAKE_BODY_COLOR RGB(200, 247, 197)
#define FRUIT_COLOR RGB(236, 100, 75)
#define OBSTACLE_COLOR RGB(56, 53, 33)
#define GAME_OVER_COLOR RGB(210, 77, 87)
#define FOOTER_COLOR RGB(71, 86, 90)
#define BUTTON_COLOR RGB(86, 110, 115)
#define LABEL_COLOR RGB(255, 255, 255)

#endif