#include "ring_buffer.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


/// @brief Allocate a RingBuffer with a maximal of cap.
/// @param cap The capacity of the buffer.
/// @return The initilized buffer.
RingBuffer *ringbuffer_create(size_t cap) {
	if (cap <= 0) {
		fprintf(stderr, "[ARGUS]: error: RingBuffer capacity must be greater than 0. Given capacity: %ld\n", cap);
		return NULL;
	}

	// Malloc the RingBuffer struct. 
	RingBuffer *buffer = malloc(sizeof(RingBuffer));
	if (!buffer) {
		fprintf(stderr, "[ARGUS]: error: failed to allocate memory for the RingBuffer structure.\n");
		return NULL;
	}
	buffer->size = 0;
	buffer->start = 0;
	buffer->cap = cap;

	// Malloc the inner buffer.
	buffer->data = malloc(sizeof(float) * cap);
	if (!buffer->data) {
		fprintf(stderr, "[ARGUS]: error: failed to allocate memory for the RingBuffer's data buffer.\n");
		free(buffer);
		return NULL;
	}
	return buffer;
}

/// @brief Frees the memory allocated for a RingBuffer.
/// @param p_buffer A pointer to the pointer of the RingBuffer to be freed. Cannot be NULL.
/// @note After freeing, the pointer *p_buffer is set to NULL to avoid double-free.
void ringbuffer_free(RingBuffer **p_buffer) {
	RingBuffer *buffer = *p_buffer;
	if (!buffer) return;
	free(buffer->data);
	free(buffer);
	*p_buffer = NULL;
}


/// @brief Clears the content of the buffer.
/// @param buffer The buffer to clear.
void ringbuffer_clear(RingBuffer *buffer) {
	buffer->start = 0;
	buffer->size = 0;
}

/// @brief Push a value at the end of the buffer.
/// @param buffer The buffer where to store the value.
/// @param val The value to store.
/// @note If the buffer is full, the oldest stored value will removed.
void ringbuffer_push_back(RingBuffer *buffer, float val) {
	size_t end = (buffer->start + buffer->size) % buffer->cap;
	buffer->data[end] = val;
	if (buffer->size < buffer->cap) ++buffer->size;
	else ++buffer->start;
}

/// @brief Gets a value in the buffer.
/// @param buffer The buffer that contains the value.
/// @param id The id of the value in the buffer. Must be inferior to buffer->size.
/// @return The contained value.
float ringbuffer_at(RingBuffer *buffer, size_t id) {
	if (id >= buffer->size) {
		fprintf(stderr, "[ARGUS]: error: The index %ld isn't in the RingBuffer !\n", id);
		return NAN;
	}
	size_t val_id = (buffer->start + id) % buffer->cap;
	return buffer->data[val_id];
}
