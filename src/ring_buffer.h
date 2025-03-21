#pragma once

#include <stddef.h>
#include "vector.h"


typedef struct {
	float *data;
	size_t cap;
	size_t size;
	size_t start;
	size_t end;
} RingBuffer;


RingBuffer *ringbuffer_create(size_t cap);
void ringbuffer_free(RingBuffer **p_buffer);

void ringbuffer_copy_vector(Vector *vector);
