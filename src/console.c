#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "argus.h"
#include "parser.h"



/// @brief This function executes an instruction.
/// @param instruction The instruction to execute.
/// @return false if the program must stop its execution.
bool execute_instruction(Instruction *instruction) {
	Color color;
	switch (instruction->type) {

	// The quit instruction stops the program.
	case INSTR_QUIT: return false;

	// Changes the title of the window.
	case INSTR_SET_TITLE:
		printf("[ARGUS]: info: setting window title to \"%s\".\n", (char*)instruction->param1);
		argus_set_title(instruction->param1);
		break;

	// Sets the size of the window.
	case INSTR_SET_SIZE:
		printf("[ARGUS]: info: setting window size to (%d,%d).\n", 
			(int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		argus_set_size((int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		break;

	// Show the window.
	case INSTR_SHOW:
		argus_show();
		break;

	// Sets the screenshots save folder path.
	case INSTR_SCREENSHOT_SET_PATH:
		printf("[ARGUS]: info: setting screenshot folder to '%s'.\n", (char*)instruction->param1);
		argus_set_screenshot_path(instruction->param1);
		break;

	// Sets the screenshots size.
	case INSTR_SCREENSHOT_SET_SIZE:
		printf("[ARGUS]: info: setting screenshot size to (%d,%d).\n", 
			(int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		argus_set_screenshot_size((int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		break;
	
	// Sets the graph grid size.
	case INSTR_SET_GRID_SIZE:
		printf("[ARGUS]: info: setting grid size to (%d,%d).\n", 
			(int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		argus_set_grid_size((int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		break;

	// Sets the current graph.
	case INSTR_SET_CURRENT_GRAPH:
		printf("[ARGUS]: info: setting current graph to (%d,%d).\n", 
			(int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		argus_set_current_graph((int)*(double*)instruction->param1, (int)*(double*)instruction->param2);
		break;

	// Sets the window background color.
	case INSTR_SET_BACKGROUND_COLOR:
		color = *(Color*)instruction->param1;
		printf("[ARGUS]: info: setting window background color to (%f,%f,%f).\n", color.r, color.g, color.b);
		argus_set_background_color(color);
		break;

	// Sets the current graph color.
	case INSTR_GRAPH_SET_COLOR:
		color = *(Color*)instruction->param1;
		printf("[ARGUS]: info: setting current graph color to (%f,%f,%f).\n", color.r, color.g, color.b);
		argus_graph_set_color(color);
		break;

	// Sets the current graph background color.
	case INSTR_GRAPH_SET_BACKGROUND_COLOR:
		color = *(Color*)instruction->param1;
		printf("[ARGUS]: info: setting current graph background color to (%f,%f,%f).\n", color.r, color.g, color.b);
		argus_graph_set_background_color(color);
		break;

	// Sets the current graph text color.
	case INSTR_GRAPH_SET_TEXT_COLOR:
		color = *(Color*)instruction->param1;
		printf("[ARGUS]: info: setting current graph text color to (%f,%f,%f).\n", color.r, color.g, color.b);
		argus_graph_set_text_color(color);
		break;

	// Sets the current graph title color.
	case INSTR_GRAPH_SET_TITLE_COLOR:
		color = *(Color*)instruction->param1;
		printf("[ARGUS]: info: setting current graph title color to (%f,%f,%f).\n", color.r, color.g, color.b);
		argus_graph_set_title_color(color);
		break;

	// Sets the current graph title.
	case INSTR_GRAPH_SET_TITLE:
		printf("[ARGUS]: info: setting current graph title to \"%s\".\n", (char*)instruction->param1);
		argus_graph_set_title(instruction->param1);
		break;

	// Sets the current graph x-axis title.
	case INSTR_GRAPH_SET_X_TITLE:
		printf("[ARGUS]: info: setting current graph x-axis title to \"%s\".\n", (char*)instruction->param1);
		argus_graph_set_x_title(instruction->param1);
		break;
	
	// Sets the current graph y-axis title.
	case INSTR_GRAPH_SET_Y_TITLE:
		printf("[ARGUS]: info: setting current graph y-axis title to \"%s\".\n", (char*)instruction->param1);
		argus_graph_set_y_title(instruction->param1);
		break;

	// This instruction do nothing.
	case INSTR_NONE:
	}

	// Frees the instruction parameters.
	free(instruction->param1);
	free(instruction->param2);
	instruction->param1 = NULL;
	instruction->param2 = NULL;
	return true;
}



// The main function. This clears the console and start reading the commands.
int main() {
    char *input = NULL;
	bool run = true;

	// Initialize the lib and clears the console.
	argus_init();
	if (!argus_is_init()) return -1;
	system("clear");
	printf("[ARGUS] Welcome !\n");

	// The main loop. Reads the commands until a quit instruction is found.
	using_history();
	while (run) {

		// Reads the command.
		input = readline("> ");
		if (!input) continue;
		add_history(input);

		// Parses and runs the instruction. 
		Instruction instruction = parse_line(input);
		run = execute_instruction(&instruction);
		if (!argus_is_init()) return -1;
		free(input);
	}

	// Quits the lib.
	argus_quit();
	return 0;
}
