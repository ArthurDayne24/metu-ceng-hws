#ifndef HW2_TYPES_H
#define HW2_TYPES_H

#include <cstring>

typedef struct {
    double x, y, z;
    int colorId;
} Vec3;

typedef struct {
    double tx, ty, tz;
} Translation;

typedef struct {
    double angle, ux, uy, uz;
} Rotation;

typedef struct {
    double sx, sy, sz;
} Scaling;

typedef struct {
    int cameraId;
    Vec3 pos;
    Vec3 gaze;
    Vec3 v;
    Vec3 u;
    Vec3 w;
    double l, r, b, t;
    double n;
    double f;
    int sizeX;
    int sizeY;
    char outputFileName[80];
} Camera;

typedef struct {
    double r, g, b;
} Color;

typedef struct {
    int vertexIds[3];
} Triangle;

typedef struct {
    int modelId;
    int type;
    int numberOfTransformations;
    int transformationIDs[1000];
    char transformationTypes[1000];
    int numberOfTriangles;
    Triangle triangles[25000];
} Model;

typedef class Matrix_4_4 {
private:
    double data[4][4];
public:
    double & getset(size_t ix, size_t iy) {
        return data[ix][iy];
    }
    // XXX One of them should be called just after constructor and ONLY
    // XXX one of them should be used for a Matrix_4_4
    void makeIdentity();
    void makeTranslation(const Translation & t);
    void makeScale(const Scaling & s);
    void makeRotationArbitrary(const Rotation & r);
    void makeFrom3Vec3(const Vec3 & u, const Vec3 & v, const Vec3 & w);
    // for rotation, helpers, not for explicit use
    void makeM(const Rotation & r);
    void makeRotationAroundX(const Rotation & r);
    Matrix_4_4 multiplyBy(const Matrix_4_4 & rhs) const;

    Matrix_4_4 getTranspose() const;
} Matrix_4_4;

#endif //HW2_TYPES_H