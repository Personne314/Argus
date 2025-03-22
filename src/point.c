#include "point.h"


// Calculates the normal between pq and pr.
#define NORMAL(p,q,r) ((q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y))

// True if q is in [p,r]
#define IN_SEGMENT(p, q, r) \
    (q.x <= (p.x > r.x ? p.x : r.x) && q.x >= (p.x < r.x ? p.x : r.x) && \
    q.y <= (p.y > r.y ? p.y : r.y) && q.y >= (p.y < r.y ? p.y : r.y))

// Epsilon for the float 0-comparison.
#define EPSILON 1e-9


/// @brief Checks if the segments [p1,q1] and [p2,q2] intersect.
/// @param p1 The beginning of the segment 1.
/// @param q1 The end of the segment 1.
/// @param p2 The beginning of the segment 2.
/// @param q2 The end of the segment 2.
/// @return true if the two segments intersect.
bool segmentsIntersect(Point p1, Point q1, Point p2, Point q2) {

	// Calculates the normals.
    double o1 = NORMAL(p1, q1, p2);
    double o2 = NORMAL(p1, q1, q2);
    double o3 = NORMAL(p2, q2, p1);
    double o4 = NORMAL(p2, q2, q1);

    // Check if there is an intersection.
    return (o1 * o2 < 0 && o3 * o4 < 0) ||
    	(o1 > -EPSILON && o1 < EPSILON && IN_SEGMENT(p1, p2, q1)) ||
    	(o2 > -EPSILON && o2 < EPSILON && IN_SEGMENT(p1, q2, q1)) ||
    	(o3 > -EPSILON && o3 < EPSILON && IN_SEGMENT(p2, p1, q2)) ||
    	(o4 > -EPSILON && o4 < EPSILON && IN_SEGMENT(p2, q1, q2));
}

/// @brief Checks if the segment [A,B] intersects the rectangle defined by [0,0], [1,1].
/// @param A The beginning of the segment.
/// @param B The end of the segment.
/// @return true if [A,B] intersects the rectangle defined by [0,0], [1,1].
bool intersectRectangle(Point A, Point B) {
	return segmentsIntersect(A, B, (Point){0,0}, (Point){0,1}) ||
		   segmentsIntersect(A, B, (Point){0,1}, (Point){1,1}) ||
		   segmentsIntersect(A, B, (Point){1,1}, (Point){1,0}) ||
		   segmentsIntersect(A, B, (Point){1,0}, (Point){0,0});
}
