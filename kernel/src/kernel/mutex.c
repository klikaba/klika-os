#include <mutex.h>
#include <process.h>
 
void mutex_acquire(mutex_t* mutex) {
	while(!__sync_bool_compare_and_swap(&mutex, 0, 1)) {
		schedule();
	}
}
 
void mutex_release(mutex_t* mutex) {
	*mutex = 0;
}