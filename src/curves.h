#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "curve.h"


/// @struct Curves
/// @brief A list of curves. Used to store the curves of a graph.
typedef struct {
	Curve **data;	//< The buffer where the curves are stored.
	size_t size;	//< The size of the list (the number of curves in the buffer).
	size_t cap;		//< The capacity of the list (the length of the buffer).
} Curves;


// Creates a list of curves.
Curves *curves_create();

// Frees the memory allocated for a Curves.
void curves_free(Curves **curves);


// Adds a new empty curve at the end of the list. 
bool curves_push_back_curve(Curves *curves);

// Deletes a curve from the list.
bool curves_delete_curve(Curves *curves, size_t id);

// Returns the number of curve stored.
size_t curves_size(Curves *curves);
