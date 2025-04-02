#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "argus.h"
#include "parser.h"



bool execute_instruction(Instruction *instruction) {
	switch (instruction->type) {
	case INSTR_QUIT: return false;
	case INSTR_SET_TITLE:
		argus_set_title(instruction->param1);
		fprintf(stdout, "[ARGUS]: info: window title was set to \"%s\".\n", instruction->param1);
		break;
	case INSTR_SHOW:
		argus_show();
		break;
	case INSTR_NONE: 
	}
	free(instruction->param1);
	free(instruction->param2);
	instruction->param1 = NULL;
	instruction->param2 = NULL;
	return true;
}



int main() {
    char *input = NULL;
	bool run = true;

	argus_init();
	if (!argus_is_init()) return -1;
	printf("[ARGUS] Welcome !\n");

	using_history();
	while (run) {
		input = readline("> ");
		if (!input) continue;
		add_history(input);
		Instruction instruction = parse_line(input);
		run = execute_instruction(&instruction);
		if (!argus_is_init()) return -1;
		free(input);
	}

	argus_quit();
	return 0;
}