#pragma once



/// @struct Rect
/// @brief Struct used to represent a rectangle aligned with the axis.
typedef struct {
	float x;	///< x coordinate.
	float y;	///< y coordinate.
	float w;	///< Width of the rectangle.
	float h;	///< Height of the rectangle.
} Rect;

// Default rectangle value.
#define RECT_INIT (Rect){0.0f,0.0f,0.0f,0.0f}



/// @struct Color
/// @brief Struct used to manipulate an rgb formated color. 
typedef struct {
	float r;	///< Red component.
	float g;	///< Green component.
	float b;	///< Blue component.
} Color;

// Set of default colors.
#define COLOR_WHITE		(Color){1.0f, 1.0f, 1.0f}
#define COLOR_BLACK		(Color){0.0f, 0.0f, 0.0f}
#define COLOR_RED		(Color){1.0f, 0.0f, 0.0f}
#define COLOR_GREEN		(Color){0.0f, 1.0f, 0.0f}
#define COLOR_BLUE		(Color){0.0f, 0.0f, 1.0f}
#define COLOR_YELLOW	(Color){1.0f, 1.0f, 0.0f}
#define COLOR_MAGENTA	(Color){1.0f, 0.0f, 1.0f}
#define COLOR_CYAN		(Color){0.0f, 1.0f, 1.0f}
#define COLOR_ORANGE	(Color){1.0f, 0.5f, 0.0f}
#define COLOR_PURPLE	(Color){0.5f, 0.0f, 0.5f}
#define COLOR_PINK		(Color){1.0f, 0.75f, 0.8f}
#define COLOR_BROWN		(Color){0.6f, 0.4f, 0.2f}
#define COLOR_LIME		(Color){0.75f, 1.0f, 0.0f}
#define COLOR_TEAL		(Color){0.0f, 0.5f, 0.5f}
#define COLOR_NAVY		(Color){0.0f, 0.0f, 0.5f}
#define COLOR_MAROON	(Color){0.5f, 0.0f, 0.0f}
#define COLOR_OLIVE		(Color){0.5f, 0.5f, 0.0f}
#define COLOR_SILVER	(Color){0.75f, 0.75f, 0.75f}
#define COLOR_GOLD		(Color){1.0f, 0.84f, 0.0f}
#define COLOR_INDIGO	(Color){0.29f, 0.0f, 0.51f}
#define COLOR_TURQUOISE	(Color){0.25f, 0.88f, 0.82f}
#define COLOR_VIOLET	(Color){0.93f, 0.51f, 0.93f}
#define COLOR_CORAL		(Color){1.0f, 0.5f, 0.31f}
#define COLOR_GRAY1		(Color){0.1f, 0.1f, 0.1f}
#define COLOR_GRAY2		(Color){0.2f, 0.2f, 0.2f}
#define COLOR_GRAY3		(Color){0.3f, 0.3f, 0.3f}
#define COLOR_GRAY4		(Color){0.4f, 0.4f, 0.4f}
#define COLOR_GRAY5		(Color){0.5f, 0.5f, 0.5f}
#define COLOR_GRAY6		(Color){0.6f, 0.6f, 0.6f}
#define COLOR_GRAY7		(Color){0.7f, 0.7f, 0.7f}
#define COLOR_GRAY8		(Color){0.8f, 0.8f, 0.8f}
#define COLOR_GRAY9		(Color){0.9f, 0.9f, 0.9f}
