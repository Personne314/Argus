#pragma once



/// @enum AxisAdaptMode
/// @brief Used to define how to update the axis limits.
typedef enum {
	ADAPTMODE_NONE,				///< Axis limits won't be changed.
	ADAPTMODE_AUTO_EXTEND,		///< Axis limits will be extended as needed.
	ADAPTMODE_AUTO_FIT,			///< Axis limits will be extended or reduced as needed.
	ADAPTMODE_SLIDING_WINDOW	///< Axis limits will be updated to follow the last point.
} AxisAdaptMode;


/// @enum DrawMode
/// @brief Used to define how to draw a Curve data.
typedef enum {
	DRAW_CURVE,		///< Draws a line made of all the points in data.
	DRAW_SCATTER	///< Draws each point in data separately.
} DrawMode;
