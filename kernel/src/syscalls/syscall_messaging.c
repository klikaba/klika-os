#include <window_manager.h>
#include <messaging.h>
#include <process.h>
#include <kernel.h>
#include <string.h>
#include <isr.h>
#include <x86.h>

#define __UNUSED__ __attribute__((unused))

long syscall_messaging_get(isr_ctx_t *regs) {
	message_t* msg = (message_t*)regs->rdi;
	window_t* win = window_find(regs->rsi);
	if (win != NULL) {
		// If no events - go to WAIT state
		if(!window_pop_message(msg, win)) {
			task_list_current->state = PROCESS_STATE_WAIT;
			schedule();
		}
	}
	return true;
}
