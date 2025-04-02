#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>


typedef enum {
	TK_EOF,
	TK_UNKNOWN,
	TK_TITLE,
	TK_SHOW,
	TK_QUIT,
	TK_SIZE,
	TK_STRING,
	TK_NUMBER
} TokenType;

typedef struct {
	TokenType type;
	void *value;
	size_t start;
} Token;



#define SCAN_KEYWORD(keyword, constant) do { \
	if (!strncmp(line, keyword, sizeof(keyword)-1) && \
		(isspace(line[sizeof(keyword)-1]) || !line[sizeof(keyword)-1])) { \
		Token token = {constant, NULL, start}; \
		*p_pos += sizeof(keyword)-1; \
		return token; \
	} \
} while(0);



Token scan_token(const char *line, size_t *p_pos) {
	while (line[*p_pos] && isspace(line[*p_pos])) ++*p_pos;
	size_t start = *p_pos;
	size_t end = 1;
	line += start;

	char c = line[0];
	if (!c) return (Token){TK_EOF, NULL, start};
	switch (c) {
	case 't':
		SCAN_KEYWORD("title", TK_TITLE);
		break;
	case 's':
		SCAN_KEYWORD("size", TK_SIZE);
		SCAN_KEYWORD("show", TK_SHOW);
		break;
	case 'q':
		SCAN_KEYWORD("quit", TK_QUIT)
		break;
	case '"':
		while (line[end]) {
			if (line[end] == '"') break; 
			++end;
		}	
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
	default:
		if (isdigit(c)) {
			double res = c-'0';
			while (isdigit(line[end])) {
				res = 10.0*res + line[end]-'0';
				++end;
			}
			if (line[end] == '.') {
				++end;
				double dec = 0.1;
				while (isdigit(line[end])) {
					res += dec*(line[end]-'0');
					dec *= 0.1;
					++end;
				}
			}
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
			if (line[end] && !isspace(line[end])) {
				if (!isdigit(line[end])) {
					*p_pos += end;
					fprintf(stderr, "[ARGUS]: unexpected character '%c'!\n", line[end]); 
					return (Token){TK_UNKNOWN, NULL, start+end};
				}
			}
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
	while (line[*p_pos] && !isspace(line[*p_pos])) ++*p_pos;
	return (Token){TK_UNKNOWN, NULL, start};
}


#define PARSOR_UNEXPECTED_TOKEN_ERROR(...) do { \
    fprintf(stderr, "[ARGUS]: unexpected token:\n%s\n", line); \
    const size_t line_len = strlen(line); \
    for (size_t i = 0; i < token.start; ++i) putchar('~'); \
    putchar('^'); \
    for (size_t i = token.start+1; i < line_len; ++i) putchar('~'); \
    putchar('\n'); \
    const char* msg = ""; \
    if (sizeof(#__VA_ARGS__) > 1) { \
        msg = #__VA_ARGS__; \
        fprintf(stderr, "%s\n", msg); \
    } \
	free(token.value); \
    instruction.type = INSTR_NONE; \
    return instruction; \
} while(0);


Instruction parse_line(const char *line) {
	Instruction instruction = {INSTR_NONE, NULL, NULL};
	size_t pos = 0;
	while (line[pos]) {
		Token token = scan_token(line, &pos);
		switch (instruction.type) {
		case INSTR_NONE:
			switch (token.type) {
			case TK_TITLE: instruction.type = INSTR_SET_TITLE; break;
			case TK_QUIT: instruction.type = INSTR_QUIT; break;
			case TK_SHOW: instruction.type = INSTR_SHOW; break;
			case TK_SIZE: instruction.type = INSTR_SET_SIZE; break;
			case TK_UNKNOWN: case TK_STRING: case TK_NUMBER: PARSOR_UNEXPECTED_TOKEN_ERROR()
			case TK_EOF: break;
			}
			break;
		case INSTR_QUIT: case INSTR_SHOW:
			if (token.type != TK_EOF) PARSOR_UNEXPECTED_TOKEN_ERROR()
			break;
		case INSTR_SET_TITLE:
			if (instruction.param1 && token.type != TK_EOF) PARSOR_UNEXPECTED_TOKEN_ERROR()
			if (token.type != TK_STRING) PARSOR_UNEXPECTED_TOKEN_ERROR("[ARGUS]: A string was expected!")
			instruction.param1 = token.value;
			break;
		case INSTR_SET_SIZE:
			if (instruction.param2 && token.type != TK_EOF) PARSOR_UNEXPECTED_TOKEN_ERROR()
			if (token.type != TK_NUMBER) PARSOR_UNEXPECTED_TOKEN_ERROR("[ARGUS]: A string was expected!")
			if (instruction.param1) instruction.param2 = token.value;
			else instruction.param1 = token.value;
			break;
		}
	}
	return instruction;
}
