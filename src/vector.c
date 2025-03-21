#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>



/// @brief Creates a vector with the specified initial capacity.
/// @param cap The initial capacity of the vector. Must be greater than 0.
/// @note If the capacity is invalid (<= 0), an error is logged and NULL is returned.
/// @note If memory allocation for the vector structure or its data fails, an error is logged and NULL is returned.
/// @return A pointer to the created vector, or NULL if an error occurred.
Vector *vector_create(size_t cap) {
	if (cap <= 0) {
		fprintf(stderr, "[ARGUS]: error: vector capacity must be greater than 0. Given capacity: %ld\n", cap);
		return NULL;
	}
	Vector *vector = malloc(sizeof(Vector));
	if (!vector) {
		fprintf(stderr, "[ARGUS]: error: failed to allocate memory for the vector structure.\n");
		return NULL;
	}
	vector->size = 0;
	vector->cap = cap;
	vector->data = malloc(sizeof(float) * cap);
	if (!vector->data) {
		fprintf(stderr, "[ARGUS]: error: failed to allocate memory for the vector's data buffer.\n");
		free(vector);
		return NULL;
	}
	return vector;
}

/// @brief Frees the memory allocated for a vector and its data buffer.
/// @param p_vector A pointer to the pointer of the vector to be freed.
/// @note If the pointer to the vector (p_vector) or the vector itself (*p_vector) is NULL, this function does nothing.
/// @note After freeing, the pointer *p_vector is set to NULL to avoid double-free.
void vector_free(Vector **p_vector) {
	if (!p_vector) return;
	Vector *vector = *p_vector;
	if (!vector) return;
	free(vector->data);
	free(vector);
	*p_vector = NULL;
}

/// @brief Adds an element at the end of the vector.
/// @param vector The vector where to add the value.
/// @param val The value to add.
/// @note Assumes that vector != NULL.
/// @note If size == capacity, the capacity will be doubled.
/// @return false in case of a realloc failure.
bool vector_push_back(Vector *vector, float val) {
	if (vector->cap == vector->size) {
		size_t new_cap = 2*vector->cap;
		float *new_data = realloc(vector->data, sizeof(float)*new_cap);
		if (!new_data) {
			fprintf(stderr, "[ARGUS]: error: unable to realloc a vector struct inner buffer!\n");
			return false;
		}
		vector->cap = new_cap;
		vector->data = new_data;
	}
	vector->data[vector->size++] = val;
	return true;
}

/// @brief Sets the value of an element in the vector.
/// @param vector The vector whose element should be modified. Cannot be NULL.
/// @param id The index of the element to modify. Must be within bounds (0 <= id < vector->size).
/// @param val The new value to assign to the element.
/// @note Assumes that vector != NULL.
/// @note If the index is invalid, no modification is performed.
/// @return false if the index is out of bounds.
bool vector_set(Vector *vector, size_t id, float val) {
	if (id >= vector->size) {
		fprintf(stderr, "[ARGUS]: error: The index %ld isn't in the vector ! The vector won't be changed.\n", id);
		return false;
	}	
	vector->data[id] = val;
	return true;
}

/// @brief Retrieves a value from a vector at a given index.
/// @param vector Pointer to the vector from which to retrieve the value. Cannot be NULL.
/// @param id The index of the value to retrieve. Must be within bounds (0 <= id < vector->size).
/// @note Assumes that vector != NULL.
/// @note If an error occurs, NAN is returned.
/// @return The value at the specified index, or NAN if the index is out of bounds.
float vector_at(Vector *vector, size_t id) {
	if (id >= vector->size) {
		fprintf(stderr, "[ARGUS]: error: The index %ld isn't in the vector ! 0 will be returned.\n", id);
		return NAN;
	}
	return vector->data[id];
}

/// @brief Retrieves the last element of a vector.
/// @param vector Pointer to the vector from which to retrieve the last element. Cannot be NULL.
/// @note Assumes that vector != NULL.
/// @note If the vector is empty, NAN is returned.
/// @return The value of the last element, or NAN if the vector is empty.
float vector_back(Vector *vector) {
	if (!vector->size) {
		fprintf(stderr, "[ARGUS]: error: an empty vector don't have a last element ! 0 will be returned.\n");
		return NAN;
	}
	return vector->data[vector->size-1];
}

/// @brief Returns the vector size.
/// @param vector The vector to get the size from.
/// @note Assumes that vector != NULL.
/// @return The size of the vector.
size_t vector_size(Vector *vector) {
	return vector->size;
}

/// @brief Returns the vector capacity.
/// @param vector The vector to get the capacity from.
/// @note Assumes that vector != NULL.
/// @return The capacity of the vector.
size_t vector_cap(Vector *vector) {
	return vector->cap;
}

/// @brief Checks if the vector is empty.
/// @param vector The vector to check.
/// @note Assumes that vector != NULL.
/// @return true if the vector is empty.
bool vector_empty(Vector *vector) {
	return !vector->size;
}

/// @brief Prints the vector into stdout.
/// @param vector The vector to print.
/// @note Assumes that vector != NULL.
void vector_print(Vector *vector) {
	putchar('{');
	for (size_t i = 0; i < vector->size; ++i) {
		if (i == vector->size-1) printf("%f", vector->data[i]);
		else printf("%f, ", vector->data[i]);
	}
	putchar('}');
	putchar('\n');
}

/// @brief Clears the vector.
/// @param vector The vector to clears.
/// @note Assumes that vector != NULL.
/// @note The memory used isn't freed. The size is set to 0.
void vector_clear(Vector *vector) {
	vector->size = 0;
}

/// @brief Deletes an element from a vector.
/// @param vector The vector where to delete an element.
/// @note Assumes that vector != NULL.
/// @param id The identifier of the element to delete.
void vector_erase(Vector *vector, size_t id) {
	if (id >= vector->size) return;
	--vector->size;
	for (size_t j = id; j < vector->size; ++j)
		vector->data[j] = vector->data[j+1];
}
