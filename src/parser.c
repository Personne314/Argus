#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>



/// @enum ParserState
/// @brief Describe all the parser states. This is used to detect multi-keyword instructions.
typedef enum {
	PS_NONE,		///< State for the detection of instructions.
	PS_SCREENSHOT,	///< State for the 'screenshot' instructions.
	PS_GRAPH,		///< State for the 'graph' instructions.
	PS_AXIS,		///< State for the 'axis' instructions.
	PS_CURVE		///< State for the 'curve' instructions.
} ParserState;



/// @enum TokenType
/// @brief This enum lists all the existing tokens.
typedef enum {
	TK_EOF,			///< Type for an End Of File token.
	TK_UNKNOWN,		///< Type for an unknown token in case of an error.
	TK_TITLE,		///< Type for the 'title' keyword.
	TK_SHOW,		///< Type for the 'show' keyword.
	TK_QUIT,		///< Type for the 'quit' keyword.
	TK_SIZE,		///< Type for the 'size' keyword.
	TK_SCREENSHOT,	///< Type for the 'screenshot' keyword.
	TK_GRAPH,		///< Type for the 'graph' keyword.
	TK_GRID,		///< Type for the 'grid' keyword.
	TK_PATH,		///< Type for the 'path' keyword.
	TK_STRING,		///< Type for a string litteral.
	TK_NUMBER		///< Type for a number litteral.
} TokenType;



/// @struct Token
/// @brief This represent a token with a type and a value.
typedef struct {
	TokenType type;	///< The token type.
	void *value;	///< The token value in case of a litteral.
	size_t start;	///< The token position in the line for debugging purpose.
} Token;



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
	size_t end = 1;
	line += start;

	// Check for a known pattern depending of the first character.
	char c = line[0];
	if (!c) return (Token){TK_EOF, NULL, start};
	switch (c) {

	// Scan for possible keywords.
	case 'g':
		SCAN_KEYWORD("graph", TK_GRAPH);
		SCAN_KEYWORD("grid", TK_GRID);
		break;
	case 'p':
		SCAN_KEYWORD("path", TK_PATH);
		break;
	case 't':
		SCAN_KEYWORD("title", TK_TITLE);
		break;
	case 's':
		SCAN_KEYWORD("size", TK_SIZE);
		SCAN_KEYWORD("screenshot", TK_SCREENSHOT);
		SCAN_KEYWORD("show", TK_SHOW);
		break;
	case 'q':
		SCAN_KEYWORD("quit", TK_QUIT)
		break;

	// Scans a string.
	case '"':

		// Go to the end of the string.
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
		return (Token){TK_STRING, string, start};
		break;

	// Scans a number.
	default:
		if (isdigit(c)) {

			// Gets the integer part.
			double res = c-'0';
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
			if (line[end] == 'e') {
				++end;
				if (!isdigit(line[end]) && line[end] != '-' && line[end] != '+') {
					*p_pos += end;
					fprintf(stderr, "[ARGUS]: an integer was expected after the e!\n"); 
					return (Token){TK_UNKNOWN, NULL, start+end};
				}
				double n = line[end] == '-' ? -1 : 1;
				if (line[end] == '-' || line[end] == '+') ++end;
				if (!isdigit(line[end])) {
					*p_pos += end;
					fprintf(stderr, "[ARGUS]: an integer was expected after the e!\n"); 
					return (Token){TK_UNKNOWN, NULL, start+end};
				}
				n *= line[end]-'0';
				++end;
				while (isdigit(line[end])) n = 10.0*n + line[end]-'0';
				res *= pow(10.0, n);
			}

			// Checks if the number ends.
			if (line[end] && !isspace(line[end])) {
				if (!isdigit(line[end])) {
					*p_pos += end;
					fprintf(stderr, "[ARGUS]: unexpected character '%c'!\n", line[end]); 
					return (Token){TK_UNKNOWN, NULL, start+end};
				}
			}

			// Creates the token.
			double *value = malloc(sizeof(double));
			if (!value) {
				*p_pos += end;
				fprintf(stderr, "[ARGUS]: unable to malloc a buffer for a number!\n"); 
				return (Token){TK_UNKNOWN, NULL, start};
			}
			*value = res;
			*p_pos += end;
			return (Token){TK_NUMBER, value, start};
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
	while (line[pos]) {
		token = scan_token(line, &pos);
		switch (instruction.type) {

		// This case analyses the first token(s) to determine the command 
		// before getting its arguments.
		case INSTR_NONE:
			switch (token.type) {
			
			// Activates the quit and show instructions detection.
			case TK_QUIT: instruction.type = INSTR_QUIT; break;
			case TK_SHOW: instruction.type = INSTR_SHOW; break;

			// This activates 'title' instructions.
			case TK_TITLE: instruction.type = INSTR_SET_TITLE; break;

			// This activates 'size' instructions.
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

			// Activates the grid resize instruction detection.
			case TK_GRID: instruction.type = INSTR_GRID_SET_SIZE; break;

			// Activates the screenshot instructions detection.
			case TK_SCREENSHOT:
				if (state != PS_NONE) return parser_unexpected_token(&token, line, NULL);
				else state = PS_SCREENSHOT;
				break;

			// This activates 'path' instructions.
			case TK_PATH:
				if (state == PS_SCREENSHOT) instruction.type = INSTR_SCREENSHOT_SET_PATH;
				else return parser_unexpected_token(&token, line, NULL);
				break;

			// Activates the graph instructions detection.
			case TK_GRAPH:
				if (state != PS_NONE) return parser_unexpected_token(&token, line, NULL);
				else state = PS_GRAPH;
				break;

			// Parse a number if possible. This can only append after the graph token, 
			// bacause it can be used as an instruction or as a part of a multiple keyword instruction. 
			case TK_NUMBER:
				if (state == PS_GRAPH) {
					instruction.type = INSTR_SET_CURRENT_GRAPH;
					instruction.param1 = token.value;
				} else return parser_unexpected_token(&token, line, NULL);
				break;

			// Unexpected token : there nothing we can deduce from this.
			case TK_UNKNOWN: case TK_STRING: return parser_unexpected_token(&token, line, NULL);

			// EOF, there nothing left to do.
			case TK_EOF: break;
			}
			break;

		// Case of quit and show that dont take any arguments.
		case INSTR_QUIT: case INSTR_SHOW:
			if (token.type != TK_EOF) return parser_unexpected_token(&token, line, NULL);
			break;

		// Gets a string to use as the window title.
		case INSTR_SET_TITLE:
			if (instruction.param1 && token.type != TK_EOF) return parser_unexpected_token(&token, line, NULL);
			if (token.type != TK_STRING) 
				return parser_unexpected_token(&token, line, "[ARGUS]: A string was expected!");
			instruction.param1 = token.value;
			break;

		// Gets two numbers to use as the window dimensions.
		case INSTR_SET_SIZE:
			if (instruction.param2 && token.type != TK_EOF) return parser_unexpected_token(&token, line, NULL);
			if (token.type != TK_NUMBER) 
				return parser_unexpected_token(&token, line, "[ARGUS]: A number was expected!");
			if (instruction.param1) instruction.param2 = token.value;
			else instruction.param1 = token.value;
			break;

		// Gets a string to use as the screenshot folder path.
		case INSTR_SCREENSHOT_SET_PATH:
			if (instruction.param1 && token.type != TK_EOF) return parser_unexpected_token(&token, line, NULL);
			if (token.type != TK_STRING) 
				return parser_unexpected_token(&token, line, "[ARGUS]: A string was expected!");
			instruction.param1 = token.value;
			state = PS_NONE;
			break;

		// Gets two numbers to use as the screenshot dimensions.
		case INSTR_SCREENSHOT_SET_SIZE:
			if (instruction.param2 && token.type != TK_EOF) return parser_unexpected_token(&token, line, NULL);
			if (token.type != TK_NUMBER) 
				return parser_unexpected_token(&token, line, "[ARGUS]: A number was expected!");
			if (instruction.param1) {
				instruction.param2 = token.value;
				state = PS_NONE;
			} else instruction.param1 = token.value;
			break;

		// Gets two numbers to use as the graph grid dimensions.
		case INSTR_GRID_SET_SIZE:
			if (instruction.param2 && token.type != TK_EOF) return parser_unexpected_token(&token, line, NULL);
			if (token.type != TK_NUMBER) 
				return parser_unexpected_token(&token, line, "[ARGUS]: A number was expected!");
			if (instruction.param1) {
				instruction.param2 = token.value;
				state = PS_NONE;
			} else instruction.param1 = token.value;
			break;

		// Gets two numbers to set the current graph.
		case INSTR_SET_CURRENT_GRAPH:
			if (instruction.param2 && token.type != TK_EOF) return parser_unexpected_token(&token, line, NULL);
			if (token.type != TK_NUMBER) 
				return parser_unexpected_token(&token, line, "[ARGUS]: A number was expected!");
			instruction.param2 = token.value;
			state = PS_NONE;
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
