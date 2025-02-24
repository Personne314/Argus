#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/// @brief Creates a vector of initial capacity cap.
/// @param cap The initial capacity of the vector. cap >= 1.
/// @return The created vector.
Vector *vector_create(int cap) {

	// Malloc the vector structure.
	Vector *vector = malloc(sizeof(Vector));
	if (!vector) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a vector struct !\n");
		return NULL;
	}

	// Malloc the inner buffer and initialize the vector.
	vector->size = 0;
	vector->cap = cap;
	vector->data = malloc(sizeof(float)*cap);
	if (!vector->data) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a vector struct inner buffer !\n");
		free(vector);
		return NULL;
	}
	return vector;
}

/// @brief Frees a vector.
/// @param vector The vector to free.
/// @note vector can be NULL.
void vector_free(Vector *vector) {
	if (!vector) return;
	if (vector->data) free(vector->data);
	free(vector);
}

/// @brief Adds an element at the end of the vector.
/// @param vector The vector where to add the value.
/// @param val The value to add.
/// @note If size == capacity, the capacity will be doubled.
void vector_push_back(Vector *vector, float val) {
	if (vector->cap == vector->size)
		vector->data = realloc(vector->data, sizeof(void*)*(vector->cap *= 2));
	vector->data[vector->size++] = val;
}

/// @brief Set the value of an element in a vector.
/// @param vector The vector where to modify the element value.
/// @param id The id of the element to modify.
/// @param val The value to give to the element.
void vector_set(Vector *vector, int id, float val) {
	if (id < 0 || id >= vector->size) {
		fprintf(stderr, "[ARGUS]: error: The index %d isn't in the vector ! The vector won't be changed. \n", id);
	}	
	vector->data[id] = val;
}

/// @brief Gets a value from a vector.
/// @param vector The vector where to get the value.
/// @param id The id of the value to return.
/// @return The value at the index id of the vector.
float vector_at(Vector *vector, int id) {
	if (id < 0 || id >= vector->size) {
		fprintf(stderr, "[ARGUS]: error: The index %d isn't in the vector ! 0 will be returned.\n", id);
		return 0;
	}
	return vector->data[id];
}

/// @brief Gets the last element of a vector.
/// @param vector The vector from where to get the value. This vector must not be empty.
/// @return The value of the last element of the vector.
float vector_back(Vector *vector) {
	if (!vector->size) {
		fprintf(stderr, "[ARGUS]: error: an empty vector don't have a last element ! 0 will be returned.\n");
		return 0;
	}
	return vector->data[vector->size-1];
}

/// @brief Returns the vector size.
/// @param vector The vector to get the size from.
/// @return The size of the vector.
int vector_size(Vector *vector) {
	return vector->size;
}

/// @brief Returns the vector capacity.
/// @param vector The vector to get the capacity from.
/// @return The capacity of the vector.
int vector_cap(Vector *vector) {
	return vector->cap;
}

/// @brief Check if the vector is empty.
/// @param vector The vector to check.
/// @return true if the vector is empty.
bool vector_empty(Vector *vector) {
	return !vector->size;
}

/// @brief Prints the vector into stdout.
/// @param vector The vector to print.
void vector_print(Vector *vector) {
	putchar('{');
	for (int i = 0; i < vector->size; ++i) {
		if (i == vector->size-1) 
			printf("%f", vector->data[i]);
		else printf("%f, ", vector->data[i]);
	}
	putchar('}');
	putchar('\n');
}

/// @brief Clears the vector.
/// @param vector The vector to clears.
/// @note The memory used isn't freed. The size is set to 0.
void vector_clear(Vector *vector) {
	vector->size = 0;
}

/// @brief Deletes an element from a vector.
/// @param vector The vector where to delete an element.
/// @param id The identifier of the element to delete.
void vector_erase(Vector *vector, int id) {
	if (id < 0 || id >= vector->size) return;
	--vector->size;
	for (int j = id; j < vector->size; ++j)
		vector->data[j] = vector->data[j+1];
}
