#pragma once

#include <stdbool.h>



/// @struct Vector
/// @brief A float dynamic array.
struct Vector {
	float *data;	///< The content of the vector.
	int size;	///< The size of the vector.
	int cap;	///< The maximum capacity of the vector.
};
typedef struct Vector Vector;


// Creates a vector of initial capacity cap.
Vector *vector_create(int cap);

// Frees a vector.
void vector_free(Vector *vector);

// Adds an element at the end of the vector.
void vector_push_back(Vector *vector, float val);

// Set the value of an element in a vector.
void vector_set(Vector *vector, int id, float val);

// Gets a value from a vector.
float vector_at(Vector *vector, int id);

// Gets the last element of a vector.
float vector_back(Vector *vector);

// Returns the vector size.
int vector_size(Vector *vector);

// Returns the vector capacity.
int vector_cap(Vector *vector);

// Check if the vector is empty.
bool vector_empty(Vector *vector);

// Prints the vector into stdout.
void vector_print(Vector *vector);

// Clears the vector.
void vector_clear(Vector *vector);

// Deletes an element from a vector.
void vector_erase(Vector *vector, int id);
