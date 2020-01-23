#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__

#define GAME_STATES_COUNT   2
#define STATE_MAIN_MENU     0
#define STATE_NEW_GAME      1
#define STATE_GAME_OVER     2
#define STATE_QUIT_GAME    -1

typedef struct {
   int (*id) (void);
   void (*init) (void);
   int (*update) (void);
   void (*draw) (void);
   void (*deinit) (void);
} GameState;

#endif
