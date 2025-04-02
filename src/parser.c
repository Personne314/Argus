#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

typedef enum {
	TK_EOF,
	TK_UNKNOWN,
	TK_TITLE,
	TK_SHOW,
	TK_QUIT,
	TK_SIZE,
	TK_STRING
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
	size_t string_end = 1;
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
		while (line[string_end]) {
			if (line[string_end] == '"') break; 
			++string_end;
		}	
		char *string = malloc(string_end);
		memcpy(string, line+1, string_end-1);
		string[string_end-1] = '\0';
		*p_pos += string_end+1;
		return (Token){TK_STRING, string, start};
		break;
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
			case TK_UNKNOWN: case TK_STRING: PARSOR_UNEXPECTED_TOKEN_ERROR()
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
		}
	}
	return instruction;
}
