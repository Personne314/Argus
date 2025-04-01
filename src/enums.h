#pragma once



/// @enum AxisAdaptMode
/// @brief Used to define how to update the axis limits.
typedef enum {
	ADAPTMODE_NONE,
	ADAPTMODE_AUTO_EXTEND,
	ADAPTMODE_AUTO_FIT,
	ADAPTMODE_SLIDING_WINDOW
} AxisAdaptMode;
