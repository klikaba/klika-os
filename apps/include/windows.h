#pragma once

#include <windows/messaging.h>
#include <windows/window.h>

void window_dispatch_message(window_t* win, struct message_struct* msg);