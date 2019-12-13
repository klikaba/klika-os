#pragma once

#include <stdint.h>

typedef struct {
	uint32_t id;
	uint32_t stack_size;
	uint8_t *stack;
} thread_t;

typedef void* (*thread_function_t)();

thread_t* thread_create(thread_function_t thread_function, uint32_t stack_size);