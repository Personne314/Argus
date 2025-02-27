#pragma once



/// @struct Rect
/// @brief Struct used to represent a rectangle aligned with the axis.
struct Rect {
	float x;	///< x coordinate.
	float y;	///< y coordinate.
	float w;	///< Width of the rectangle.
	float h;	///< Height of the rectangle.
};
typedef struct Rect Rect;

// Default rectangle value.
#define RECT_INIT (Rect){0.0f,0.0f,0.0f,0.0f}


/// @struct Color
/// @brief Struct used to manipulate an rgb formated color. 
struct Color {
	float r;	///< Red component.
	float g;	///< Green component.
	float b;	///< Blue component.
};
typedef struct Color Color;

// Set of default colors.
#define COLOR_WHITE (Color){1.0f, 1.0f, 1.0f}
#define COLOR_BLACK (Color){0.0f, 0.0f, 0.0f}
#define COLOR_GRAY1 (Color){0.1f, 0.1f, 0.1f}
#define COLOR_GRAY2 (Color){0.2f, 0.2f, 0.2f}
#define COLOR_GRAY3 (Color){0.3f, 0.3f, 0.3f}
#define COLOR_GRAY4 (Color){0.4f, 0.4f, 0.4f}
#define COLOR_GRAY5 (Color){0.5f, 0.5f, 0.5f}
#define COLOR_GRAY6 (Color){0.6f, 0.6f, 0.6f}
#define COLOR_GRAY7 (Color){0.7f, 0.7f, 0.7f}
#define COLOR_GRAY8 (Color){0.8f, 0.8f, 0.8f}
#define COLOR_GRAY9 (Color){0.9f, 0.9f, 0.9f}
