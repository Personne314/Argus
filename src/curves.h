#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "curve.h"


typedef struct {
	Curve **data;
	size_t size;
	size_t cap;
} Curves;


Curves *curves_create();
void curves_free(Curves **curves);

bool curves_push_back_curve(Curves *curves);
bool curves_delete_curve(Curves *curves, size_t id);

size_t curves_size(Curves *curves);
