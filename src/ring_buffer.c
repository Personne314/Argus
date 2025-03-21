#include "ring_buffer.h"

#include <stdlib.h>
#include <stdio.h>


RingBuffer *ringbuffer_create(size_t cap) {
	if (cap <= 0) {
		fprintf(stderr, "[ARGUS]: error: RingBuffer capacity must be greater than 0. Given capacity: %ld\n", cap);
		return NULL;
	}
	RingBuffer *buffer = malloc(sizeof(RingBuffer));
	if (!buffer) {
		fprintf(stderr, "[ARGUS]: error: failed to allocate memory for the RingBuffer structure.\n");
		return NULL;
	}
	buffer->size = 0;
	buffer->start = 0;
	buffer->end = 0;
	buffer->cap = cap;
	buffer->data = malloc(sizeof(float) * cap);
	if (!buffer->data) {
		fprintf(stderr, "[ARGUS]: error: failed to allocate memory for the RingBuffer's data buffer.\n");
		free(buffer);
		return NULL;
	}
	return buffer;
}

void ringbuffer_free(RingBuffer **p_buffer) {
	RingBuffer *buffer = *p_buffer;
	if (!buffer) return;
	free(buffer->data);
	free(buffer);
	*p_buffer = NULL;
}


void ringbuffer_copy_vector(Vector *vector) {
	
}
