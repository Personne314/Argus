#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include "structs.h"
#include "enums.h"



/// @enum ParserState
/// @brief Describe all the parser states. This is used to detect multi-keyword instructions.
typedef enum {
	PS_NONE,		///< State for the detection of instructions.
	PS_SCREENSHOT,	///< State for the 'screenshot' instructions.
	PS_GRAPH,		///< State for the 'graph' instructions.
	PS_GRAPH_TITLE,	///< State for the 'graph title' instructions.
	PS_GRAPH_TEXT,	///< State for the 'graph text' instructions.
	PS_GRAPH_BACKGROUND,	///< State for the 'graph background' instructions.
	PS_GRAPH_X,		///< State for the 'x' instructions.
	PS_GRAPH_Y,		///< State for the 'y' instructions.
	PS_AXIS,		///< State for the 'axis' instructions.
	PS_CURVE		///< State for the 'curve' instructions.
} ParserState;



/// @enum TokenType
/// @brief This enum lists all the existing tokens.
typedef enum {
	TK_EOS,			///< Type for an End Of String token.
	TK_UNKNOWN,		///< Type for an unknown token in case of an error.
	TK_TITLE,		///< Type for the 'title' keyword.
	TK_SHOW,		///< Type for the 'show' keyword.
	TK_QUIT,		///< Type for the 'quit' keyword.
	TK_SIZE,		///< Type for the 'size' keyword.
	TK_SCREENSHOT,	///< Type for the 'screenshot' keyword.
	TK_GRAPH,		///< Type for the 'graph' keyword.
	TK_GRID,		///< Type for the 'grid' keyword.
	TK_PATH,		///< Type for the 'path' keyword.
	TK_COLOR,		///< Type for the 'color' keyword.
	TK_TEXT,		///< Type for the 'text' keyword.
	TK_BACKGROUND,	///< Type for the 'background' keyword.
	TK_X,			///< Type for the 'x' keyword.
	TK_Y,			///< Type for the 'y' keyword.
	TK_CURVE,		///< Type for the 'curve' keyword.
	TK_ADD,			///< Type for the 'add' keyword.
	TK_REMOVE,		///< Type for the 'remove' keyword.
	TK_ADAPT,		///< Type for the 'adapt' keyword.
	TK_NONE,		///< Type for the 'none' keyword.
	TK_EXTEND,		///< Type for the 'extend' keyword.
	TK_FIT,			///< Type for the 'fit' keyword.
	TK_SLIDE,		///< Type for the 'slide' keyword.
	TK_LITT_STRING,	///< Type for a string litteral.
	TK_LITT_NUMBER,	///< Type for a number litteral.
	TK_LITT_COLOR	///< Type for a color litteral.
} TokenType;



/// @struct Token
/// @brief This represent a token with a type and a value.
typedef struct {
	TokenType type;	///< The token type.
	void *value;	///< The token value in case of a litteral.
	size_t start;	///< The token position in the line for debugging purpose.
} Token;



/// @brief Calculates the int value of an hex digit.
/// @param c The hex digit to use.
/// @return The int value of the hex digit.
int parse_hex_to_int(char c) {
	if (c >= '0' && c <= '9') return c-'0';
	if (c >= 'a' && c <= 'f') return c-'a'+10;
	if (c >= 'A' && c <= 'F') return c-'A'+10;
	return 0;
}

/// @brief Calculates the float value of an 2-hex-digit number.
/// @param str The string to use.
/// @return The float value.
/// @note This is a linear function from [0x00,0xFF] into [0.0f,1.0f]
float parse_hex_to_float(const char *str) {
	return ((parse_hex_to_int(str[0])<<4)+parse_hex_to_int(str[1]))/255.0f;
}

/// @brief This parse a string into a float.
/// @param line 
/// @param offset 
/// @return 
double parse_double(const char *line, size_t *offset) {
	size_t end = *offset;
	bool minus = line[end] == '-';
	if (minus) ++end;

	// Gets the integer part.
	double res = 0;
	while (isdigit(line[end])) {
		res = 10.0*res + line[end]-'0';
		++end;
	}

	// Gets the decimal part.
	if (line[end] == '.') {
		++end;
		double dec = 0.1;
		while (isdigit(line[end])) {
			res += dec*(line[end]-'0');
			dec *= 0.1;
			++end;
		}
	}

	// Scans an exponent.
	if (line[end] == 'e' || line[end] == 'E') {
		++end;
		if (!isdigit(line[end]) && line[end] != '-' && line[end] != '+') {
			*offset = end;
			fprintf(stderr, "[ARGUS]: an integer was expected after the e!\n"); 
			return NAN;
		}
		double n = line[end] == '-' ? -1 : 1;
		if (line[end] == '-' || line[end] == '+') ++end;
		if (!isdigit(line[end])) {
			*offset = end;
			fprintf(stderr, "[ARGUS]: an integer was expected after the e!\n"); 
			return NAN;
		}
		n *= line[end]-'0';
		++end;
		while (isdigit(line[end])) n = 10.0*n + line[end]-'0';
		res *= pow(10.0, n);
	}

	// Checks if the number ends.
	*offset = end;
	if (line[end] && !isspace(line[end])) {
		if (isalnum(line[end])) {
			fprintf(stderr, "[ARGUS]: unexpected character '%c'!\n", line[end]); 
			return NAN;
		}
	}

	// Creates the token.
	double *value = malloc(sizeof(double));
	if (!value) {
		fprintf(stderr, "[ARGUS]: unable to malloc a buffer for a number!\n"); 
		return NAN;
	}
	return res * (minus ? -1 : 1);
}



// This macro is used to scan a keyword and return the corresponding token
// if this keyword was found, or do nothing in the opposit case.
#define SCAN_KEYWORD(keyword, constant) do { \
	if (!strncmp(line, keyword, sizeof(keyword)-1) && \
		(isspace(line[sizeof(keyword)-1]) || !line[sizeof(keyword)-1])) { \
		Token token = {constant, NULL, start}; \
		*p_pos += sizeof(keyword)-1; \
		return token; \
	} \
} while(0);



/// @brief This functions scans a token in line and move *p_pos just after it.
/// @param line The line being scanned.
/// @param p_pos The current position of the cursor in line.
/// @return The token that was found.
Token scan_token(const char *line, size_t *p_pos) {
	while (line[*p_pos] && isspace(line[*p_pos])) ++*p_pos;
	size_t start = *p_pos;
	size_t end = 0;
	line += start;

	// Check for a known pattern depending of the first character.
	char c = line[0];
	if (!c) return (Token){TK_EOS, NULL, start};
	switch (c) {
	// Scan for possible keywords.
	case 'a':
		SCAN_KEYWORD("add", TK_ADD);
		SCAN_KEYWORD("adapt", TK_ADAPT);
		break;
	case 'b':	
		SCAN_KEYWORD("background", TK_BACKGROUND);
		break;
	case 'c':
		SCAN_KEYWORD("color", TK_COLOR);
		SCAN_KEYWORD("curve", TK_CURVE);
		break;
	case 'e':
		SCAN_KEYWORD("extend", TK_EXTEND);
		break;
	case 'f':
		SCAN_KEYWORD("fit", TK_FIT);
		break;
	case 'g':
		SCAN_KEYWORD("graph", TK_GRAPH);
		SCAN_KEYWORD("grid", TK_GRID);
		break;
	case 'n':
		SCAN_KEYWORD("none", TK_NONE);
		break;
	case 'p':
		SCAN_KEYWORD("path", TK_PATH);
		break;
	case 'q':
		SCAN_KEYWORD("quit", TK_QUIT)
		break;
	case 'r':
		SCAN_KEYWORD("remove", TK_REMOVE)
		break;
	case 's':
		SCAN_KEYWORD("size", TK_SIZE);
		SCAN_KEYWORD("slide", TK_SLIDE);
		SCAN_KEYWORD("screenshot", TK_SCREENSHOT);
		SCAN_KEYWORD("show", TK_SHOW);
		break;
	case 't':
		SCAN_KEYWORD("title", TK_TITLE);
		SCAN_KEYWORD("text", TK_TEXT);
		break;
	case 'x':
		SCAN_KEYWORD("x", TK_X)
		break;
	case 'y':
		SCAN_KEYWORD("y", TK_Y)
		break;

	// Scans a string.
	case '"':
		++end;
		while (line[end]) {
			if (line[end] == '"') break; 
			++end;
		}

		// Detects the terminator of the string.
		if (line[end] != '"') {
			fprintf(stderr, "[ARGUS]: expected a closing '\"'!\n"); 
			return (Token){TK_UNKNOWN, NULL, start+end-1};
		}

		// Creates the token.
		char *string = malloc(end);
		if (!string) {
			fprintf(stderr, "[ARGUS]: unable to malloc a buffer for a number!\n"); 
			return (Token){TK_UNKNOWN, NULL, start};
		}
		memcpy(string, line+1, end-1);
		string[end-1] = '\0';
		*p_pos += end+1;
		return (Token){TK_LITT_STRING, string, start};
		break;

	// Scans a color.
	case '#':
		++end;
		while (isxdigit(line[end])) ++end;
		if (end != 7 || (line[end] && !isspace(line[end]))) {
			fprintf(stderr, "[ARGUS]: a color RGB code must be 6 characters long!\n"); 
			return (Token){TK_UNKNOWN, NULL, start};
		}

		// Creates the token.
		Color *color = malloc(sizeof(Color));
		if (!color) {
			fprintf(stderr, "[ARGUS]: unable to malloc a buffer for a color!\n"); 
			return (Token){TK_UNKNOWN, NULL, start};
		}
		color->r = parse_hex_to_float(line+1);
		color->g = parse_hex_to_float(line+3);
		color->b = parse_hex_to_float(line+5);
		*p_pos += 7;
		return (Token){TK_LITT_COLOR, color, start};
		break;

	// Scans a number.
	default:
		if (isdigit(c) || (c == '-' && isdigit(line[1]))) {
			printf("parsing double\n");
			double res = parse_double(line, &end);
			*p_pos += end;
			if (res == NAN) return (Token){TK_UNKNOWN, NULL, start};
			
			// Creates the token.
			double *value = malloc(sizeof(double));
			if (!value) {
				*p_pos += end;
				fprintf(stderr, "[ARGUS]: unable to malloc a buffer for a number!\n"); 
				return (Token){TK_UNKNOWN, NULL, start};
			}
			*value = res;
			return (Token){TK_LITT_NUMBER, value, start};	
		}
	}

	// If there was an error, go to the next non-space character to 
	// avoid scanning the same token indefinitely.
	while (line[*p_pos] && !isspace(line[*p_pos])) ++*p_pos;
	return (Token){TK_UNKNOWN, NULL, start};
}



/// @brief This prints an error message and returns an empty instruction.
/// @param token The token where the error occured.
/// @param line The line currently being parsed.
/// @param str The additionnal message to print.
/// @return A None instruction.
Instruction parser_unexpected_token(Token *token, const char *line, const char *str) {
    fprintf(stderr, "[ARGUS]: unexpected error:\n%s\n", line);
    const size_t line_len = strlen(line);
    for (size_t i = 0; i < token->start; ++i) putchar('~');
    putchar('^');
    for (size_t i = token->start+1; i < line_len; ++i) putchar('~');
    putchar('\n');
    if (str) fprintf(stderr, "%s\n", str);
	free(token->value);
    return (Instruction){INSTR_NONE, NULL, NULL};
}

/// @brief This parse a line into an instruction.
/// @param line The line to parse. Thi must not be NULL.
Instruction parse_line(const char *line) {
	Instruction instruction = {INSTR_NONE, NULL, NULL};
	ParserState state = PS_NONE;
	
	// Parse each token depending of the current state and the instruction type. 
	size_t pos = 0;
	Token token;
	while (token.type != TK_EOS) {
		token = scan_token(line, &pos);
		switch (instruction.type) {

		// This case analyses the first token(s) to determine the command 
		// before getting its arguments.
		case INSTR_NONE:
			switch (token.type) {
			
			// Activates the quit and show instructions detection.
			case TK_QUIT: 
				if (state == PS_NONE) instruction.type = INSTR_QUIT; 
				else parser_unexpected_token(&token, line, NULL);
				break;
			case TK_SHOW:
			if (state == PS_NONE) instruction.type = INSTR_SHOW; 
			else parser_unexpected_token(&token, line, NULL);
				break;

			// This activates the 'color' instructions detection.
			case TK_COLOR: 
				if (state == PS_NONE) instruction.type = INSTR_SET_BACKGROUND_COLOR;
				else if (state == PS_GRAPH) instruction.type = INSTR_GRAPH_SET_COLOR;
				else if (state == PS_GRAPH_TEXT) instruction.type = INSTR_GRAPH_SET_TEXT_COLOR;
				else if (state == PS_GRAPH_BACKGROUND) instruction.type = INSTR_GRAPH_SET_BACKGROUND_COLOR;
				else if (state == PS_GRAPH_TITLE) instruction.type = INSTR_GRAPH_SET_TITLE_COLOR;
				else parser_unexpected_token(&token, line, NULL);
				break;

			// This activates the 'title' instructions detection.
			case TK_TITLE: 
				if (state == PS_NONE) instruction.type = INSTR_SET_TITLE; 
				else if (state == PS_GRAPH_X) {
					instruction.type = INSTR_GRAPH_SET_X_TITLE;
					state = PS_NONE;
				}
				else if (state == PS_GRAPH_Y) {
					instruction.type = INSTR_GRAPH_SET_Y_TITLE;
					state = PS_NONE;
				} else if (state == PS_GRAPH) state = PS_GRAPH_TITLE;
				else parser_unexpected_token(&token, line, NULL);
				break;

			// This activates the 'background' instructions detection.
			case TK_BACKGROUND:
				if (state == PS_GRAPH) state = PS_GRAPH_BACKGROUND;
				else parser_unexpected_token(&token, line, NULL);
				break;

			// This activates the 'text' instructions detection.
			case TK_TEXT:
				if (state == PS_GRAPH) state = PS_GRAPH_TEXT;
				else parser_unexpected_token(&token, line, NULL);
				break;

			// This activates 'size' instructions detection.
			case TK_SIZE: 
				switch (state) {
				case PS_NONE: instruction.type = INSTR_SET_SIZE; break;
				case PS_SCREENSHOT: instruction.type = INSTR_SCREENSHOT_SET_SIZE; break;
				default: return parser_unexpected_token(&token, line, NULL);
				}
				if (state == PS_NONE) instruction.type = INSTR_SET_SIZE; 
				else if (state == PS_SCREENSHOT) instruction.type = INSTR_SCREENSHOT_SET_SIZE;
				else if (state == PS_SCREENSHOT) instruction.type = INSTR_SCREENSHOT_SET_SIZE;
				else return parser_unexpected_token(&token, line, NULL);
				break;

			// Activates the grid resize instructions detection.
			case TK_GRID: instruction.type = INSTR_SET_GRID_SIZE; break;

			// Activates the screenshot instructions detection.
			case TK_SCREENSHOT:
				if (state != PS_NONE) return parser_unexpected_token(&token, line, NULL);
				else state = PS_SCREENSHOT;
				break;

			// This activates 'path' instructions detection.
			case TK_PATH:
				if (state == PS_SCREENSHOT) instruction.type = INSTR_SCREENSHOT_SET_PATH;
				else return parser_unexpected_token(&token, line, NULL);
				break;

			// Activates the 'graph' instructions detection.
			case TK_GRAPH:
				if (state != PS_NONE) return parser_unexpected_token(&token, line, NULL);
				else state = PS_GRAPH;
				break;

			// Activates the 'graph adapt' instructions detection.
			case TK_ADAPT:
				if (state == PS_GRAPH) instruction.type = INSTR_GRAPH_ADAPT;
				else if (state == PS_GRAPH_X) instruction.type = INSTR_GRAPH_X_ADAPT;
				else if (state == PS_GRAPH_Y) instruction.type = INSTR_GRAPH_Y_ADAPT;
				else return parser_unexpected_token(&token, line, NULL);
				state = PS_NONE;
				break;

			// Parse a number if possible. This can only append after the graph token, 
			// bacause it can be used as an instruction or as a part of a multiple keyword instruction. 
			case TK_LITT_NUMBER:
				if (state == PS_GRAPH) {
					instruction.type = INSTR_SET_CURRENT_GRAPH;
					instruction.param1 = token.value;
				} else if (state == PS_CURVE) {
					instruction.param1 = token.value;
					instruction.type = INSTR_SET_CURVE;
					state = PS_NONE;
				} else return parser_unexpected_token(&token, line, NULL);
				break;

			// Parse a string if possible. This can only append after the graph token, 
			// bacause it can be used as an instruction or as a part of a multiple keyword instruction. 
			case TK_LITT_STRING:
				if (state == PS_GRAPH_TITLE) {
					instruction.type = INSTR_GRAPH_SET_TITLE;
					instruction.param1 = token.value;
					state = PS_NONE;
				} else return parser_unexpected_token(&token, line, NULL);
				break;

			// This activates 'x' instructions detection.
			case TK_X:
				if (state == PS_GRAPH) state = PS_GRAPH_X;
				else parser_unexpected_token(&token, line, NULL);
				break;

			// This activates 'y' instructions detection.
			case TK_Y:
				if (state == PS_GRAPH) state = PS_GRAPH_Y;
				else parser_unexpected_token(&token, line, NULL);
				break;

			// This activates 'curve' instructions detection.
			case TK_CURVE:
				if (state == PS_NONE) state = PS_CURVE;
				else parser_unexpected_token(&token, line, NULL);
				break;

			// This detects and 'curve add' instruction. 
			case TK_ADD:
				if (state == PS_CURVE) {
					state = PS_NONE;
					instruction.type = INSTR_CURVE_ADD;
				}
				else parser_unexpected_token(&token, line, NULL);
				break;

			// This detects and 'curve remove' instruction.
			case TK_REMOVE:
				if (state == PS_CURVE) {
					state = PS_NONE;
					instruction.type = INSTR_CURVE_REMOVE;
				}
				else parser_unexpected_token(&token, line, NULL);
				break;
				
			// EOF, there nothing left to do.
			case TK_EOS: break;

			// Unexpected token : there nothing we can deduce from this.
			default: return parser_unexpected_token(&token, line, NULL);
			}
			break;

		// Ensure that the instruction isn't followed by anything.
		case INSTR_QUIT: 
		case INSTR_SHOW:
		case INSTR_GRAPH_SET_TITLE:
		case INSTR_CURVE_ADD:
		case INSTR_CURVE_REMOVE:
		case INSTR_SET_CURVE:
			if (token.type != TK_EOS) return parser_unexpected_token(&token, line, NULL);
			break;

		// Gets a string for set title and set path instructions.
		case INSTR_SET_TITLE:
		case INSTR_SCREENSHOT_SET_PATH:
		case INSTR_GRAPH_SET_X_TITLE:
		case INSTR_GRAPH_SET_Y_TITLE:
			if (instruction.param1) {
				if (token.type != TK_EOS) return parser_unexpected_token(&token, line, NULL);
				else break;
			}
			if (token.type != TK_LITT_STRING) {
				return parser_unexpected_token(&token, line, "[ARGUS]: A string was expected!");
			}
			instruction.param1 = token.value;
			state = PS_NONE;
			break;

		// Gets two numbers to use as the window dimensions.
		case INSTR_SET_SIZE:
			if (instruction.param2) {
				if (token.type != TK_EOS) return parser_unexpected_token(&token, line, NULL);
				else break;
			}
			if (token.type != TK_LITT_NUMBER) {
				return parser_unexpected_token(&token, line, "[ARGUS]: A number was expected!");
			}
			if (instruction.param1) instruction.param2 = token.value;
			else instruction.param1 = token.value;
			break;

		// Gets two numbers to use as the screenshot dimensions.
		case INSTR_SCREENSHOT_SET_SIZE:
			if (instruction.param2) {
				if (token.type != TK_EOS) return parser_unexpected_token(&token, line, NULL);
				else break;
			}
			if (token.type != TK_LITT_NUMBER) {
				return parser_unexpected_token(&token, line, "[ARGUS]: A number was expected!");
			}
			if (instruction.param1) {
				instruction.param2 = token.value;
				state = PS_NONE;
			} else instruction.param1 = token.value;
			break;

		// Gets two numbers to use as the graph grid dimensions.
		case INSTR_SET_GRID_SIZE:
			if (instruction.param2) {
				if (token.type != TK_EOS) return parser_unexpected_token(&token, line, NULL);
				else break;
			}
			if (token.type != TK_LITT_NUMBER) {
				return parser_unexpected_token(&token, line, "[ARGUS]: A number was expected!");
			}
			if (instruction.param1) {
				instruction.param2 = token.value;
				state = PS_NONE;
			} else instruction.param1 = token.value;
			break;

		// Gets two numbers to set the current graph.
		case INSTR_SET_CURRENT_GRAPH:
			if (instruction.param2) {
				if (token.type != TK_EOS) return parser_unexpected_token(&token, line, NULL);
				else break;
			}
			if (token.type != TK_LITT_NUMBER) {
				return parser_unexpected_token(&token, line, "[ARGUS]: A number was expected!");
			}
			instruction.param2 = token.value;
			state = PS_NONE;
			break;

		// Gets a color for the set color instructions.
		case INSTR_SET_BACKGROUND_COLOR:
		case INSTR_GRAPH_SET_COLOR:
		case INSTR_GRAPH_SET_BACKGROUND_COLOR:
		case INSTR_GRAPH_SET_TEXT_COLOR:
		case INSTR_GRAPH_SET_TITLE_COLOR:
			if (instruction.param1) {
				if (token.type != TK_EOS) return parser_unexpected_token(&token, line, NULL);
				else break;
			}
			if (token.type != TK_LITT_COLOR) {
				return parser_unexpected_token(&token, line, "[ARGUS]: A color was expected!");
			}
			instruction.param1 = token.value;
			state = PS_NONE;
			break;

		// Gets an axis adapt mode for the adapt instructions.
		case INSTR_GRAPH_ADAPT:
		case INSTR_GRAPH_X_ADAPT:
		case INSTR_GRAPH_Y_ADAPT:
			if (instruction.param1) {
				if (token.type != TK_EOS) return parser_unexpected_token(&token, line, NULL);
				else break;
			}
			AxisAdaptMode *mode = malloc(sizeof(AxisAdaptMode));
			if (!mode) {
				fprintf(stderr, "[ARGUS]: error: unable to malloc a buffer for an axis mode!\n");
				instruction.type = INSTR_NONE;
				break;
			}
			switch (token.type) {
			case TK_NONE:	*mode = ADAPTMODE_NONE;				break;
			case TK_FIT:	*mode = ADAPTMODE_AUTO_FIT;			break;
			case TK_EXTEND:	*mode = ADAPTMODE_AUTO_EXTEND;		break;
			case TK_SLIDE:	*mode = ADAPTMODE_SLIDING_WINDOW;	break;
			default: 
				free(mode);	
				return parser_unexpected_token(&token, line, "[ARGUS]: none/fit/extend/slide was expected!");
			}
			instruction.param1 = mode;
			break;
		}
	}

	// If the instruction ends and state wasn't reset, it means that there was something
	// missing in the command such as a keyword or an argument.
	if (state != PS_NONE) {
		token.start = strlen(line)-1;
		return parser_unexpected_token(&token, line, "[ARGUS]: Missing argument!");
	}
	return instruction;
}
