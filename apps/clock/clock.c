#include <klikaos.h>
#include <windows.h>
#include <stdlib.h>
#include <types.h>
#include <time.h>
#include <gfx.h>

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)
#define MSG_USER_BTN (WINDOW_USER_MESSAGE + 2)
#define MSG_USER_LABEL (WINDOW_USER_MESSAGE + 3)

message_t msg;
window_t *window;
window_t *label;

int lastTimestamp = 0;

void set_time_label()
{
    unsigned long timestamp = time(NULL);
    struct tm datetime;
    char buffer[100];

    mkdatetime(timestamp, &datetime);
    strftime(buffer, 100, &datetime);

    lastTimestamp = timestamp;
    label_set_text(label, buffer);
}

int main()
{
    int layout_y = WINDOW_BAR_HEIGHT + 10;
    window = window_create(120, 120, 175, 106, "Clock", MSG_USER_WIN, WINDOW_ATTR_NONE, WINDOW_FRAME_DEFAULT);
    button_create(window, 10, layout_y + 20, 150, 20, "Refresh time", MSG_USER_BTN);
    label = label_create(window, 10, layout_y, 150, 20, "", MSG_USER_LABEL);
    set_time_label();

    while (window_get_message(window, &msg))
    {
        switch (msg.message)
        {
        case MSG_USER_BTN:
            set_time_label();
            break;
        }

        window_dispatch(window, &msg);
    }

    return 0;
}