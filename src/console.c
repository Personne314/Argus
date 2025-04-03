#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "argus.h"
#include "parser.h"



bool execute_instruction(Instruction *instruction) {
	Color color;
	switch (instruction->type) {
	case INSTR_QUIT: return false;
	case INSTR_SET_TITLE:
		printf("[ARGUS]: info: setting window title to \"%s\".\n", (char*)instruction->param1);
		argus_set_title(instruction->param1);
		break;
	case INSTR_SET_SIZE:
		printf("[ARGUS]: info: setting window size to (%d,%d).\n", 
			(int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		argus_set_size((int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		break;
	case INSTR_SHOW:
		argus_show();
		break;
	case INSTR_SCREENSHOT_SET_PATH:
		printf("[ARGUS]: info: setting screenshot folder to '%s'.\n", (char*)instruction->param1);
		argus_set_screenshot_path(instruction->param1);
		break;
	case INSTR_SCREENSHOT_SET_SIZE:
		printf("[ARGUS]: info: setting screenshot size to (%d,%d).\n", 
			(int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		argus_set_screenshot_size((int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		break;
	case INSTR_SET_GRID_SIZE:
		printf("[ARGUS]: info: setting grid size to (%d,%d).\n", 
			(int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		argus_set_grid_size((int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		break;
	case INSTR_SET_CURRENT_GRAPH:
		printf("[ARGUS]: info: setting current graph to (%d,%d).\n", 
			(int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		argus_set_current_graph((int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		break;
	case INSTR_SET_BACKGROUND_COLOR:
		color = *(Color*)instruction->param1;
		printf("[ARGUS]: info: setting window background color to (%f,%f,%f).\n", color.r, color.g, color.b);
		argus_set_background_color(color);
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
	system("clear");
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