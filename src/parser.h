#pragma once

#include <stddef.h>



/// @enum InstructionType
/// @brief This enum lists all possible basic instructions.
typedef enum {
	INSTR_NONE,			///< Do nothing.
	INSTR_SHOW,			///< Show the defined graphs.
	INSTR_SET_TITLE,	///< Sets the window title.
	INSTR_SET_SIZE,		///< Sets the window size.
	INSTR_SCREENSHOT_SET_PATH,	///< Sets the screenshot save folder path.
	INSTR_SCREENSHOT_SET_SIZE,	///< Sets the screenshot size.
	INSTR_GRID_SET_SIZE,		///< Sets the graph grid size.
	INSTR_SET_CURRENT_GRAPH,	///< Sets the current graph.
	INSTR_QUIT		///< Quit the application. 
} InstructionType;


/// @struct Instruction
/// @brief Tnis struct represents an instruction and its parameters.
typedef struct {
	InstructionType type;	///< The type of the instruction.
	void *param1;	///< The first parameter of the instruction.
	void *param2;	///< The second parameter of the instruction.
} Instruction;



// This parse a line into an instruction.
Instruction parse_line(const char *line);
