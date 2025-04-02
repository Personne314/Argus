#pragma once

#include <stddef.h>


typedef enum {
	INSTR_NONE,
	INSTR_SET_TITLE,
	INSTR_SHOW,
	INSTR_SET_SIZE,
	INSTR_QUIT
} InstructionType;



typedef struct {
	InstructionType type;
	void *param1;
	void *param2;
} Instruction;


Instruction parse_line(const char *line);
