#pragma once

#include <stdbool.h>


/// @struct Point
/// @brief Represent a point in a plane.
typedef struct {
    float x;
    float y;
} Point;


// Checks if the segments [p1,q1] and [p2,q2] intersect.
bool segmentsIntersect(Point p1, Point q1, Point p2, Point q2);

// Checks if the segment [A,B] intersects the rectangle defined by [0,0], [1,1].
bool intersectRectangle(Point A, Point B);
