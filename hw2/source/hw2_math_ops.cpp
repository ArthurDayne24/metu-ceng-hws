#include <cstdio>
#include <cmath>
#include "hw2_math_ops.h"
#define ABS(a) ((a)>0?(a):-1*(a))

/*
 * Calculate cross product of vec3 a, vec3 b and return resulting vec3.
 */
Vec3 crossProductVec3(const Vec3 & a, const Vec3 & b) {
    Vec3 result;

    result.x = a.y * b.z - b.y * a.z;
    result.y = b.x * a.z - a.x * b.z;
    result.z = a.x * b.y - b.x * a.y;

    // epsilon 10^-6, make values zero if smaller than that
    if (fabs(result.x) < 1E-6) result.x = 0;
    if (fabs(result.y) < 1E-6) result.y = 0;
    if (fabs(result.z) < 1E-6) result.z = 0;

    return result;
}

/*
 * Calculate dot product of vec3 a, vec3 b and return resulting value.
 */
double dotProductVec3(const Vec3 & a, const Vec3 & b) {
    return a.x * b.x + a.y * b.y + a.z * b.z < 1E-6 ? 0 : a.x * b.x + a.y * b.y + a.z * b.z;
}

/*
 * Find length (|v|) of vec3 v.
 */
double lengthOfVec3(const Vec3 & v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

/*
 * Normalize the vec3 to make it unit vec3.
 */
Vec3 normalizeVec3(const Vec3 & v) {
    Vec3 result;
    double d;

    d = lengthOfVec3(v);
    result.x = v.x / d;
    result.y = v.y / d;
    result.z = v.z / d;

    return result;
}

/*
 * Add vec3 a to vec3 b and return resulting vec3 (a+b).
 */
Vec3 addVec3(const Vec3 & a, const Vec3 & b) {
    Vec3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;

    return result;
}

/*
 * Subtract vec3 b from vec3 a and return resulting vec3 (a-b).
 */
Vec3 subtractVec3(const Vec3 & a, const Vec3 & b) {
    Vec3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;

    return result;
}

/*
 * Multiply each element of vec3 with scalar.
 */
Vec3 multiplyVec3WithScalar(const Vec3 & v, double c) {
    Vec3 result;
    result.x = v.x * c;
    result.y = v.y * c;
    result.z = v.z * c;

    return result;
}

/*
 * Prints elements in a vec3. Can be used for debugging purposes.
 */
void printVec3(const Vec3 & v) {
    printf("(%lf,%lf,%lf)\n", v.x, v.y, v.z);
}

/*
 * Check whether vec3 a and vec3 b are equal.
 * In case of equality, returns 1.
 * Otherwise, returns 0.
 */
int areEqualVec3(const Vec3 & a, const Vec3 & b) {
    double e = 0.000000001;

    /* if x difference, y difference and z difference is smaller than threshold, then they are equal */
    if ((ABS((a.x - b.x)) < e) && (ABS((a.y - b.y)) < e) && (ABS((a.z - b.z)) < e)) {
        return 1;
    } else {
        return 0;
    }
}
