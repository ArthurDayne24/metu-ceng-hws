#ifndef HW2_TYPES_H
#define HW2_TYPES_H

#include <cstring>

class Vec4;

class Vec3 {
public:
    Vec3(int x=0, int y=0, int z=0, int colorId=0);
    // ignore last dimension
    explicit Vec3(const Vec4 & rhs);
    double x, y, z;
    int colorId;
};

class Vec4 {
public:
    // pad 1 to last dimention
    explicit Vec4(const Vec3 & rhs);
    Vec4(int x=0, int y=0, int z=0, int w=0, int colorId=0);
    void make_homogenous();
    double x, y, z, w;
    int colorId;
};

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
    void bringtToOrigin();
} Camera;

typedef struct {
    double r, g, b;
} Color;

typedef struct {
    int vertexIds[3];
    bool backface_passed = false;
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

typedef struct Matrix_4_4 {
    double data[4][4];

    // XXX One of them should be called just after constructor and ONLY
    // XXX one of them should be used for a Matrix_4_4
    void makeIdentity();
    void makeTranslation(const Translation & t);
    void makeScale(const Scaling & s);
    void makeRotationArbitrary(const Rotation & r);
    void makeFrom3Vec3(const Vec3 & u, const Vec3 & v, const Vec3 & w);

    Matrix_4_4 multiplyBy(const Matrix_4_4 & rhs) const;
    Vec4 multiplyBy(const Vec4 & rhs) const;
    Matrix_4_4 getTranspose() const;

    void makeZeros();

private:
    // for rotation, helpers, not for explicit use
    void makeM(const Rotation & r);
    void makeRotationAroundX(const Rotation & r);

} Matrix_4_4;

#endif //HW2_TYPES_H
