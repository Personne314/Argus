#pragma once

#include <stddef.h>


typedef struct {
	float *data;
	size_t cap;
	size_t size;
	size_t start;
} RingBuffer;


RingBuffer *ringbuffer_create(size_t cap);
void ringbuffer_free(RingBuffer **p_buffer);

void ringbuffer_clear(RingBuffer *buffer);
void ringbuffer_push_back(RingBuffer *buffer, float val);

float ringbuffer_at(RingBuffer *buffer, size_t id);
