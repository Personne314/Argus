#pragma once

#include <stddef.h>


/// @struct RingBuffer.
/// @brief A buffer that can store up to cap value.
/// @note When full, the buffer loops and the oldest values will be replaced.
typedef struct {
	float *data;	///< The inner buffer that contains the data.
	size_t cap;		///< The capacity of the buffer (the length of data).
	size_t size;	///< The size of the buffer (the number of value stored).
	size_t start;	///< The id of the first value in the buffer.
} RingBuffer;


// Allocate a RingBuffer with a maximal of cap.
RingBuffer *ringbuffer_create(size_t cap);

// Frees the memory allocated for a RingBuffer.
void ringbuffer_free(RingBuffer **p_buffer);


// Clears the content of the buffer.
void ringbuffer_clear(RingBuffer *buffer);

// Push a value at the end of the buffer.
void ringbuffer_push_back(RingBuffer *buffer, float val);

// Gets a value in the buffer.
float ringbuffer_at(RingBuffer *buffer, size_t id);
