#include "csv.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "parser.h"



/// @enum CSVParserState 
/// @brief List of the parser states.
typedef enum {
	CPS_COMMA,	///< Last token was a comma.
	CPS_VALUE	///< Last token was a value.
} CSVParserState;

/// @enum CSVTokenType
/// @brief List of the csv token types.
typedef enum {
	CSV_TK_EOL,		///< Type for the End Of Line token.
	CSV_TK_VALUE,	///< Type for a numerical value token.
	CSV_TK_COMMA,	///< Type for a ',' token.
	CSV_TK_UNKNOWN	///< Type for everything else.
} CSVTokenType;

/// @struct CSVToken
/// @brief A structure used to store a csv token.
typedef struct {
	double value;		///< The value of the token. 0 if type != CSV_TK_VALUE.
	CSVTokenType type;	///< The type of the token.
} CSVToken;



/// @brief This scan a token in line, updates offset and returns the detected token.
/// @param line The line to parse.
/// @param offset The offset in line indicating where to start reading.
/// @return The detected token.
CSVToken csv_scan_token(const char *line, size_t *offset) {
	while (line[*offset] && isspace(line[*offset])) ++*offset;
	line += *offset;
	size_t end = 0;

	// Check for a known pattern depending of the first character.
	char c = line[end];
	if (!c) return (CSVToken){0, CSV_TK_EOL};
	switch (c) {

	// Parses a separator.
	case ',':
		++*offset;
		return (CSVToken){0, CSV_TK_COMMA};

	// Scans a number.
	default:
		if (isdigit(c) || (c == '-' && isdigit(line[1]))) {
			double res = parse_double(line, &end);
			*offset += end;
			if (res == NAN) return (CSVToken){0, CSV_TK_UNKNOWN};
			return (CSVToken){res, CSV_TK_VALUE};
		}
	}
	return (CSVToken){0, CSV_TK_UNKNOWN};
}



/// @brief This parse a line of a csv file and store its data.

/// @return true if there was an error.
/// @note if csv == NULL, doesn't check the line size and doesn't store the values.


bool csv_parse_line(const char *line, CSV *csv, size_t *len, size_t line_num) {
	size_t line_offset = 0;
	*len = 0;
	size_t nb = 0;
	size_t line_len = csv ? csv->column : 0;
	CSVParserState state = CPS_COMMA;
	while (line[line_offset]) {
		CSVToken token = csv_scan_token(line, &line_offset);
		switch (token.type) {
		case CSV_TK_VALUE:
			if (state == CPS_VALUE) {
				fprintf(stderr, "[ARGUS]: error: two values must be separated by a comma: '%s'\n", line);
				return true;
			}
			state = CPS_VALUE;
			if (csv) {
				if (nb >= line_len) {
					fprintf(stderr, "[ARGUS]: error: this line is too long compared to the first line: '%s'\n", line);
					return true;
				}
				csv_set(csv, nb, line_num, token.value);
			}
			++nb;
			break;
		case CSV_TK_COMMA: 
			if (state == CPS_COMMA) {
				if (csv) {
					if (nb >= line_len) {
						fprintf(stderr, "[ARGUS]: error: this line is too long compared to the first line: '%s'\n", line);
						return true;
					}
					csv_set(csv, nb, line_num, NAN);
				}
				++nb;
			}
			state = CPS_COMMA; 
			break;
		case CSV_TK_EOL: break;
		default:
			fprintf(stderr, "[ARGUS]: error: unexpected token in line '%s'!\n", line);
			return true;
		}
	}
	
	if (state == CPS_COMMA) {
		if (csv) {
			if (nb >= line_len) {
				fprintf(stderr, "[ARGUS]: error: this line is too long compared to the first line: '%s'\n", line);
				return true;
			}
			csv_set(csv, nb, line_num, NAN);
		}
		++nb;
	}

	if (csv) while (nb < line_len) {
		csv_set(csv, nb, line_num, NAN);
		++nb;
	}
	*len = nb;
	return true;
}



/// @brief Loads a CSV file from the disk.
/// @param path The path of the file.
CSV *csv_load(const char *path) {

	// Opens the CSV file.
	FILE *file = fopen(path, "r");
	if (!file) {
		fprintf(stderr, "[ARGUS]: error: unable to open the csv file '%s'!\n", path);
		return NULL;
	}

	// Counts the number of lines and columns in the file.
	size_t lines = 0, len = 0;
	char buffer[1024];
	while (fgets(buffer, sizeof(buffer), file)) {
		buffer[strcspn(buffer, "\n")] = '\0';
		if (!lines) csv_parse_line(buffer, NULL, &len, 0);
		++lines;
	}
	fseek(file, 0,0);
	if (!lines) {
		fprintf(stderr, "[ARGUS]: error: the csv file '%s' is empty!\n", path);
		return NULL;
	}
	if (!len) {
		fprintf(stderr, "[ARGUS]: error: the csv file '%s' first line can't be empty!\n", path);
		return NULL;
	}

	// Creates the CSV structure.
	CSV *csv = malloc(sizeof(CSV));
	if (!csv) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a CSV struct!\n");
		return NULL;
	}
	csv->line = lines;
	csv->column = len;
	csv->data = malloc(lines*len*sizeof(double));
	if (!csv->data) {
		fprintf(stderr, "[ARGUS]: error: unable to malloc a CSV struct buffer!\n");
		return NULL;
	}

	// Gets the data.
	lines = 0;	
	while (fgets(buffer, sizeof(buffer), file)) {
		buffer[strcspn(buffer, "\n")] = '\0';
		if (!csv_parse_line(buffer, csv, &len, lines)) {
			fprintf(stderr, "[ARGUS]: error: unable to parse the CSV '%s'!\n", path);
			csv_free(&csv);
			return NULL;
		}
		++lines;
	}
	fclose(file);
	return csv;
}

/// @brief Frees the memory allocated for a CSV.
/// @param p_csv A pointer to the pointer of the CSV to be freed. Cannot be NULL.
/// @note After freeing, the pointer *p_csv is set to NULL to avoid double-free.
void csv_free(CSV **p_csv) {
	CSV *csv = *p_csv;
	if (!csv) return;
	free(csv->data);
	free(csv);
	*p_csv = NULL;
}



/// @brief Gets a value from a CSV.
/// @param csv The CSV to use.
/// @param line The id of the line.
/// @param column The id of the column.
/// @return The value stored at line,column.
double csv_get(CSV *csv, size_t column, size_t line) {
	if (line >= csv->line || column >= csv->column) {
		fprintf(stderr, "[ARGUS]: error: %ld,%ld isn't in the csv file!\n", line, column);
		return NAN;
	}
	return csv->data[line*csv->column+column];
}

/// @brief Sets a value from a CSV.
/// @param csv The CSV to use.
/// @param line The id of the line.
/// @param column The id of the column.
/// @param value The value to store at line,column.
void csv_set(CSV *csv, size_t column, size_t line, double value) {
	if (line >= csv->line || column >= csv->column) {
		fprintf(stderr, "[ARGUS]: error: %ld,%ld isn't in the csv file!\n", line, column);
		return;
	}
	csv->data[line*csv->column+column] = value;
}
