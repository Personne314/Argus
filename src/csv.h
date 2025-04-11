#pragma once

#include <stddef.h>


typedef struct {
	double *data;
	size_t line;
	size_t column;
} CSV;



// Loads a CSV file from the disk.
CSV *csv_load(const char *path);

// Frees the memory allocated for a CSV.
void csv_free(CSV **p_csv);


// Gets a value from a CSV.
double csv_get(CSV *csv, size_t column, size_t line);

// Sets a value from a CSV.
void csv_set(CSV *csv, size_t column, size_t line, double value);
