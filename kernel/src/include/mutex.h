#pragma once

typedef volatile int mutex_t;
 
void mutex_acquire(mutex_t* mutex);
void mutex_release(mutex_t* mutex);