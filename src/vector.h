#pragma once

#include <stdbool.h>
#include <stddef.h>


/// @struct Vector
/// @brief A dynamic array of float.
typedef struct {
	float *data;	///< The content of the vector.
	size_t size;	///< The size of the vector.
	size_t cap;		///< The maximum capacity of the vector.
} Vector;


/// Creates a vector with the specified initial capacity.
Vector *vector_create(size_t cap);

/// Frees the memory allocated for a vector and its data buffer.
void vector_free(Vector **p_vector);

// Adds an element at the end of the vector.
bool vector_push_back(Vector *vector, float val);

// Sets the value of an element in the vector.
bool vector_set(Vector *vector, size_t id, float val);

// Retrieves a value from a vector at a given index.
float vector_at(Vector *vector, size_t id);

// Retrieves the last element of a vector.
float vector_back(Vector *vector);

// Returns the vector size.
size_t vector_size(Vector *vector);

// Returns the vector capacity.
size_t vector_cap(Vector *vector);

// Checks if the vector is empty.
bool vector_empty(Vector *vector);

// Prints the vector into stdout.
void vector_print(Vector *vector);

// Clears the vector.
void vector_clear(Vector *vector);

// Deletes an element from a vector.
void vector_erase(Vector *vector, size_t id);
