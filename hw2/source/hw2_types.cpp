
#include <cmath>
#include "hw2_types.h"
#include "hw2_math_ops.h"

void Matrix_4_4::makeIdentity() {
    makeZeros();

    data[0][0] = 1;
    data[1][1] = 1;
    data[2][2] = 1;
    data[3][3] = 1;
}

Matrix_4_4 Matrix_4_4::multiplyBy(const Matrix_4_4 & rhs) const {
    double total;

    Matrix_4_4 result;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            total = 0;
            for (int k = 0; k < 4; k++) {
                total += data[i][k] * rhs.data[k][j];
            }
            result.data[i][j] = total;
        }
    }

    return result;
}

void Matrix_4_4::makeTranslation(const Translation & t) {
    makeIdentity();

    data[0][3] = t.tx;
    data[1][3] = t.ty;
    data[2][3] = t.tz;
}

void Matrix_4_4::makeScale(const Scaling & s) {
    makeIdentity();

    data[0][0] = s.sx;
    data[1][1] = s.sy;
    data[2][2] = s.sz;
}

void Matrix_4_4::makeM(const Rotation & r) {
    data[0][0] = r.ux;
    data[0][1] = r.uy;
    data[0][2] = r.uz;
    data[0][3] = 0;

    data[1][0] = -r.uy;
    data[1][1] = r.ux;
    data[1][2] = 0;
    data[1][3] = r.uz;

    Vec3 u, v, w;

    u.x = data[0][0];
    u.y = data[0][1];
    u.z = data[0][2];

    v.x = data[1][0];
    v.y = data[1][1];
    v.z = data[1][2];

    w = crossProductVec3(u, v);

    data[2][0] = w.x;
    data[2][1] = w.y;
    data[2][2] = w.z;
    data[2][3] = 0;

    data[3][0] = 0;
    data[3][1] = 0;
    data[3][2] = 0;
    data[3][3] = 1;
}

Matrix_4_4 Matrix_4_4::getTranspose() const {
    Matrix_4_4 result;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.data[i][j] = data[j][i];
        }
    }

    return result;
}

void Matrix_4_4::makeRotationAroundX(const Rotation & r) {
    makeIdentity();

    double d = sqrt(r.uy*r.uy + r.uz*r.uz);

    data[0][0] = 1;

    data[1][1] = r.uz / d;
    data[1][2] = -r.uy / d;

    data[2][1] = r.uy / d;
    data[2][2] = r.uz / d;

    data[3][3] = 1;
}

// rotate "angle" degrees around arbitrary axis
void Matrix_4_4::makeRotationArbitrary(const Rotation & r) {
    Matrix_4_4 M;
    M.makeM(r);

    Matrix_4_4 M_inv = M.getTranspose();

    Matrix_4_4 rotX;
    rotX.makeRotationAroundX(r);

    Matrix_4_4 result = M_inv.multiplyBy(rotX.multiplyBy(M));

    memcpy(data, result.data, 16 * sizeof(double));
}

void Matrix_4_4::makeFrom3Vec3(const Vec3 & u, const Vec3 & v, const Vec3 & w) {
    data[0][0] = u.x;
    data[0][1] = u.y;
    data[0][2] = u.z;
    data[0][3] = 0;

    data[1][0] = v.x;
    data[1][1] = v.y;
    data[1][2] = v.z;
    data[1][3] = 0;

    data[2][0] = w.x;
    data[2][1] = w.y;
    data[2][2] = w.z;
    data[2][3] = 0;

    data[3][0] = 0;
    data[3][1] = 0;
    data[3][2] = 0;
    data[3][3] = 0;
}

Vec4 Matrix_4_4::multiplyBy(const Vec4 & rhs) const {
    Vec4 result;

    result.x = data[0][0] * rhs.x + data[0][1] * rhs.y + data[0][2] * rhs.z + data[0][3] * rhs.w;
    result.y = data[1][0] * rhs.x + data[1][1] * rhs.y + data[1][2] * rhs.z + data[1][3] * rhs.w;
    result.z = data[2][0] * rhs.x + data[2][1] * rhs.y + data[2][2] * rhs.z + data[2][3] * rhs.w;
    result.w = data[3][0] * rhs.x + data[3][1] * rhs.y + data[3][2] * rhs.z + data[3][3] * rhs.w;

    result.colorId = rhs.colorId;

    return result;
}

void Matrix_4_4::makeZeros() {
    memset(data, 0, sizeof(double) * 16);
}

Vec4::Vec4(const Vec3 & rhs) {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    w = 1;

    colorId = rhs.colorId;
}

void Vec4::make_homogenous() {
    x /= w;
    y /= w;
    z /= w;
    w = 1;
}

Vec4::Vec4(int x, int y, int z, int w, int colorId) : x(x), y(y), z(z), w(w), colorId(colorId) {
}

void Camera::bringtToOrigin() {
    // TODO not sure xd

    pos = Vec3(0, 0, 0);

    gaze = Vec3(0, 0, -1);

    v = Vec3(0, 1, 0);
    u = Vec3(1, 0, 0);
    w = Vec3(0, 0, 1);

    l = -1;
    r = 1;
    b = -1;
    t = 1;

    n = -1;
    f = 1;
}

Vec3::Vec3(const Vec4 &rhs) {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;

    colorId = rhs.colorId;
}

Vec3::Vec3(int x, int y, int z, int colorId) : x(x), y(y), z(z), colorId(colorId) {
}
