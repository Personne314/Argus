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
bool segments_intersect(Point p1, Point q1, Point p2, Point q2) {

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
bool intersect_rectangle(Point A, Point B) {
	return segments_intersect(A, B, (Point){0,0}, (Point){0,1}) ||
		   segments_intersect(A, B, (Point){0,1}, (Point){1,1}) ||
		   segments_intersect(A, B, (Point){1,1}, (Point){1,0}) ||
		   segments_intersect(A, B, (Point){1,0}, (Point){0,0});
}

/// @brief Calculates the intersection point of two lines defined by two points.
/// @param p1 First line first point.
/// @param q1 First line second point.
/// @param p2 Second line first point.
/// @param q2 Second line second point.
/// @param res A pointer to a point where to store the result.
/// @note Assumes that the two lines are not collinear.
void find_intersection(Point p1, Point q1, Point p2, Point q2, Point* res) {
    
	// Calculates the first line parameters.
    double a1 = q1.y - p1.y;
    double b1 = p1.x - q1.x;
    double c1 = a1 * p1.x + b1 * p1.y;

	// Calculates the second line parameters.
    double a2 = q2.y - p2.y;
    double b2 = p2.x - q2.x;
    double c2 = a2 * p2.x + b2 * p2.y;

    // Calculates the intersection point.
    const double det = a1 * b2 - a2 * b1;
    res->x = (b2 * c1 - b1 * c2) / det;
    res->y = (a1 * c2 - a2 * c1) / det;
}

/// @brief Calculates the intersection point of the ray [A,B) and the rectangle defined by [0,0], [1,1].
/// @param A First point of the ray.
/// @param B Second point of the ray.
/// @param res A pointer to a point where to store the result.
void move_in_rectangle(Point A, Point B, Point *res) {
	if (B.x-1 > 0) find_intersection(A, B, (Point){1,1}, (Point){1,0}, &B);
	if (B.y-1 > 0) find_intersection(A, B, (Point){0,1}, (Point){1,1}, &B);
	if (B.x < 0)   find_intersection(A, B, (Point){0,0}, (Point){0,1}, &B);
	if (B.y < 0)   find_intersection(A, B, (Point){1,0}, (Point){0,0}, &B);
	res->x = B.x;
	res->y = B.y;
}
