#include <klikaos.h>
#include <windows.h>
#include <stdlib.h>
#include <types.h>
#include <time.h>
#include <gfx.h>

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)
#define MSG_USER_LABEL (WINDOW_USER_MESSAGE + 2)

message_t msg;
window_t *window;
window_t *label;

void draw_clock()
{
    // Get system time and set label
    unsigned long timestamp = time(NULL);
    struct tm datetime;
    char buffer[100];

    mkdatetime(timestamp, &datetime);
    strftime(buffer, 100, &datetime);
    label_set_text(label, buffer);

    // Redraw window
    window_invalidate(window);
}

int main()
{
    int layout_y = WINDOW_BAR_HEIGHT + 10;
    window = window_create(120, 120, 175, 76, "Clock", MSG_USER_WIN, WINDOW_ATTR_NONE, WINDOW_FRAME_DEFAULT);
    label = label_create(window, 10, layout_y, 150, 15, "", MSG_USER_LABEL);

    while (window_get_message(window, &msg))
    {
        switch (msg.message)
        {
        case WINDOW_LIB_MESSAGE_DRAW:
            draw_clock();
            break;
        }

        window_dispatch(window, &msg);
    }

    return 0;
}