#include "curves.h"

#include <stdio.h>
#include <stdlib.h>


/// @brief Creates a list of curves.
/// @return The created list.
Curves *curves_create() {
	Curves *curves = malloc(sizeof(Curves));
	if (!curves) {
		fprintf(stderr, "[ARGUS]: error: failed to allocate memory for the curves structure.\n");
		return NULL;
	}
	curves->size = 0;
	curves->cap = 1;
	curves->data = malloc(sizeof(Curve*));
	if (!curves->data) {
		fprintf(stderr, "[ARGUS]: error: failed to allocate memory for the curves's data buffer.\n");
		free(curves);
		return NULL;
	}
	return curves;
}

/// @brief Frees the memory allocated for a Curves.
/// @param p_curves A pointer to the pointer of the Curves to be freed. Cannot be NULL.
/// @note After freeing, the pointer *p_curves is set to NULL to avoid double-free.
void curves_free(Curves **p_curves) {
	Curves *curves = *p_curves;
	if (!curves) return;
	for (size_t i = 0; i < curves->size; ++i) curve_free(curves->data+i);
	free(curves->data);
	free(curves);
	*p_curves = NULL;
}


/// @brief Adds a new empty curve at the end of the list. 
/// @param curves The list where to add a curve.
/// @return false if there was an error.
bool curves_push_back_curve(Curves *curves) {
	if (curves->cap == curves->size) {
		size_t new_cap = 2*curves->cap;
		Curve **new_data = realloc(curves->data, sizeof(Curve)*new_cap);
		if (!new_data) {
			fprintf(stderr, "[ARGUS]: error: unable to realloc a curves struct inner buffer!\n");
			return false;
		}
		curves->cap = new_cap;
		curves->data = new_data;
	}
	curves->data[curves->size] = curve_create();
	if (!curves->data[curves->size]) {
		fprintf(stderr, "[ARGUS]: error: unable to create a new curve in a graph curves structure!\n");
		return false;
	}
	++curves->size;
	return true;
}

/// @brief Deletes a curve from the list.
/// @param curves The list of curves where to delete a curve.
/// @param id The id of the curve to delete. Must be inferior to curves->size.
/// @return false if there was an error.
bool curves_delete_curve(Curves *curves, size_t id) {
	if (!curves->size) {
		fprintf(stderr, "[ARGUS]: error: unable to remove an element "
			"from an empty curves list ! The curves won't be changed.\n");
		return false;
	}
	if (id >= curves->size) {
		fprintf(stderr, "[ARGUS]: error: The index %ld isn't in the "
			"curves list ! The curves won't be changed.\n", id);
		return false;
	}
	curve_free(curves->data+id);
	for (size_t i = id; i < curves->size-2; ++i) {
		curves->data[i] = curves->data[i+1];
	}
	--curves->size;
	return true;
}


/// @brief Returns the number of curve stored.
/// @param curves The list of curves from which to get the size of.
/// @return The number of curves.
size_t curves_size(Curves *curves) {
	return curves->size;
}
