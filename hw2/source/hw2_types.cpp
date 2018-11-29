#include <cmath>
#include "hw2_types.h"
#include "hw2_math_ops.h"

void Matrix_4_4::makeIdentity() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == j) {
                data[i][j] = 1.0;
            } else {
                data[i][j] = 0.0;
            }
        }
    }
}

Matrix_4_4 Matrix_4_4::multiplyBy(const Matrix_4_4 & rhs) const {
    double total;

    Matrix_4_4 result;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            total = 0;
            for (int k = 0; k < 4; k++)
                total += data[i][k] * rhs.data[k][j];
            result.data[i][j] = total;
        }
    }

    return result;
}

// Make a translation matrix
void Matrix_4_4::makeTranslation(const Translation & t) {
    makeIdentity();

    data[0][3] = t.tx;
    data[1][3] = t.ty;
    data[2][3] = t.tz;
}

// Make a scale matrix
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

    data[0][0] = data[3][3] = 1;

    data[1][1] = r.uz / d;
    data[1][2] = -r.uy / d;

    data[2][1] = r.uy / d;
    data[2][2] = r.uz / d;
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

