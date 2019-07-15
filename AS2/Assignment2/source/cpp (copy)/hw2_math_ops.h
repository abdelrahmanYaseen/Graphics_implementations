#include "hw2_types.h"

#ifndef HW2_MATH_OPS_H
#define HW2_MATH_OPS_H

/*
 * Calculate cross product of vec3 a, vec3 b and return resulting vec3.
 */
Vec3 crossProductVec3(Vec3 a, Vec3 b);

/*
 * Calculate dot product of vec3 a, vec3 b and return resulting value.
 */
double dotProductVec3(Vec3 a, Vec3 b);

/*
 * Find length (|v|) of vec3 v.
 */
double lengthOfVec3(Vec3 v);

/*
 * Normalize the vec3 to make it unit vec3.
 */
Vec3 normalizeVec3(Vec3 v);

/*
 * Add vec3 a to vec3 b and return resulting vec3 (a+b).
 */
Vec3 addVec3(Vec3 a, Vec3 b);

/*
 * Subtract vec3 b from vec3 a and return resulting vec3 (a-b).
 */
Vec3 subtractVec3(Vec3 a, Vec3 b);

/*
 * Multiply each element of vec3 with scalar.
 */
Vec3 multiplyVec3WithScalar(Vec3 v, double c);

/*
 * Prints elements in a vec3. Can be used for debugging purposes.
 */
void printVec3(Vec3 v);

/*
 * Check whether vec3 a and vec3 b are equal.
 * In case of equality, returns 1.
 * Otherwise, returns 0.
 */
int areEqualVec3(Vec3 a, Vec3 b);

/*
 * Makes matrix m (double[4][4]) an identity matrix (values on the diagonal are 1, others are 0).
*/
void makeIdentityMatrix(double m[4][4]);

/*
 * Multiply matrices m1 (double[4][4]) and m2 (double[4][4]) and store the result in result matrix r (double[4][4]).
 */
void multiplyMatrixWithMatrix(double r[4][4], double m1[4][4], double m2[4][4]);

/*
 * Multiply matrix m (double[4][4]) with vector v (double[4]) and store the result in vector r (double[4]).
 */
void multiplyMatrixWithVec4d(double r[4], double m[4][4], double v[4]);

/*
 * Multiply matrix m (double[3][3]) with vector v (double[3]) and store the result in vector r (double[3]).
 */
void multiplyMatrixWithVec3d(double r[3], double m[3][3], double v[3]);

/*
* Get Slope of line between two points: a, b . Ignores the Z coordinate
*/
double get2DSlope(Vec3 a, Vec3 b);

/*
* swaps vectors, becomes handy when dealing with line drawing
*/
void swap(Vec3 a, Vec3 b);

/*
* subtracts colors, becomes handy when dealing with line  coloring
*/
Color subtractColors(Color a, Color b);

/*
* adds colors, becomes handy when dealing with line  coloring
*/
Color addColors(Color a, Color b);


/*
* divides a color by number, becomes handy when dealing with line  coloring
*/
Color divideColorByVal(Color a , double val);

/*
* computes and alpha,beta,gamma in the results matrix
*/
void getBarycentric(double x, double y,Vec3 a, Vec3 b, Vec3 c , double results[3]);

/*
* returns a copy of a vector
*/
Vec3 copyOfVec3(Vec3 v);


/*
* prints a 4x4 matrix
*/
void printM4(double m[4][4]);

/*
* copies the content of the source to the distination
*/
void copyM4toM4(double dist[4][4], double source[4][4]);

/*
* Performs Translation t on vector v
*/
void translate(Vec3* v, Translation t);

void translate_MATRIX(Translation t, double M[4][4]);

/*
* Performs Scaling s on vector v
*/
void scale(Vec3* v, Scaling s);
void scale_MATRIX(Scaling t, double M[4][4]);
/*
* Performs Rotation r on vector v
*/
void rotate(Vec3* v, Rotation r);

void rotate_MATRIX(Rotation t, double M[4][4]);

/*
* Performs Rotation r on vector v
*/
void rotate2(Vec3 *v, Rotation r);
#endif //HW2_MATH_OPS_H
