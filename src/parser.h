#pragma once

#include <stddef.h>



/// @enum InstructionType
/// @brief This enum lists all possible basic instructions.
typedef enum {
	INSTR_NONE,			///< Do nothing.
	INSTR_QUIT,			///< Quit the application. 
	INSTR_SHOW,			///< Show the defined graphs.
	INSTR_SET_TITLE,	///< Sets the window title.
	INSTR_SET_SIZE,		///< Sets the window size.
	INSTR_SCREENSHOT_SET_PATH,	///< Sets the screenshot save folder path.
	INSTR_SCREENSHOT_SET_SIZE,	///< Sets the screenshot size.
	INSTR_SET_GRID_SIZE,		///< Sets the graph grid size.
	INSTR_SET_CURRENT_GRAPH,	///< Sets the current graph.
	INSTR_SET_BACKGROUND_COLOR, ///< Sets the window background color.
	INSTR_GRAPH_SET_COLOR,		///< Sets the graph color.
	INSTR_GRAPH_SET_BACKGROUND_COLOR,	///< Sets the graph background color.
	INSTR_GRAPH_SET_TITLE_COLOR,	///< Sets the graph title color.
	INSTR_GRAPH_SET_TEXT_COLOR,		///< Sets the graph text color.
	INSTR_GRAPH_SET_TITLE,		///< Sets the current graph title.
	INSTR_GRAPH_SET_X_TITLE,	///< Sets the current graph x-axis label. 
	INSTR_GRAPH_SET_Y_TITLE,	///< Sets the current graph y-axis label.
	INSTR_GRAPH_ADAPT,		///< Sets the adapt mode for both axis.
	INSTR_GRAPH_X_ADAPT,	///< Sets the adapt mode for the x-axis.
	INSTR_GRAPH_Y_ADAPT,	///< Sets the adapt mode for the y-axis.
	INSTR_CURVE_ADD,		///< Adds a new curve to the current graph.
	INSTR_CURVE_REMOVE,		///< Removes the current curve.
	INSTR_SET_CURVE			///< Sets the current curve.
} InstructionType;


/// @struct Instruction
/// @brief Tnis struct represents an instruction and its parameters.
typedef struct {
	InstructionType type;	///< The type of the instruction.
	void *param1;	///< The first parameter of the instruction.
	void *param2;	///< The second parameter of the instruction.
} Instruction;



// This parse a string into a float.
double parse_double(const char *line, size_t *offset);

// This parse a line into an instruction.
Instruction parse_line(const char *line);
