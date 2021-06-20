#include "transform.h"
#include <cmath>
#include <cstdio>

#define me (*this)

unsigned Transform::inverse_calcued = 0;

double Transform::det_company(int i, int j) const {
    register int 
        a = 0 + (i==0)*4 + (j==0),
        b = 1 + (i==0)*4 + (j!=2), 
        c = 4 + (i!=2)*4 + (j==0), 
        d = 5 + (i!=2)*4 + (j!=2);
    return m[a]*m[d] - m[b]*m[c];
}

void Transform::remove() {
    if(m || mi || cnt) {
        (*cnt) -= 2;
        if((*cnt) >> 1 == 0) {
            delete[] m;
            delete[] mi;
            delete cnt;
        }
        m = nullptr, mi = nullptr, cnt = nullptr;
    }
}

Transform::Transform() {
    m = nullptr, mi = nullptr, cnt = nullptr;
}

Transform::~Transform() {
    remove();
}

Transform::Transform(double* _data) {
    cnt = new unsigned(2);
    mi = new double[16];
    m = _data;
}

Transform::Transform(Vector3 u, Vector3 v, Vector3 w, Vector3 t) {
    cnt = new unsigned(2);
    mi = new double[16];
    m = new double[16] {
        u.x, v.x, w.x, t.x,
        u.y, v.y, w.y, t.y,
        u.z, v.z, w.z, t.z,
        0.0, 0.0, 0.0, 1.0
    };
}

Transform::Transform(const Transform& T) {
    m = T.m, mi = T.mi, cnt = T.cnt;
    (*cnt) += 2;
}

Transform& Transform::operator=(const Transform& T) {
    if(m == T.m) return *this;
    remove();
    m = T.m, mi = T.mi, cnt = T.cnt;
    (*cnt) += 2;
    return *this;
}

Transform Transform::copy() const {
    double *cp = new double[16];
    for(int i = 0; i < 16; ++i) cp[i] = m[i];
    return Transform(cp);
}

Transform Transform::Scale(double k) {
    double *data = new double[16]{
          k, 0.0, 0.0, 0.0,
        0.0,   k, 0.0, 0.0,
        0.0, 0.0,   k, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    return Transform(data);
}
Transform Transform::Rotate(Vector3 axis, double theta) {
    axis = axis.unify();
    double c = cos(theta), s = sin(theta);
    double *data = new double[16]{
        axis.x * axis.x * (1 - c) + c,
        axis.x * axis.y * (1 - c) - s * axis.z,
        axis.x * axis.z * (1 - c) + s * axis.y,
        0.0,
        axis.y * axis.x * (1 - c) + s * axis.z,
        axis.y * axis.y * (1 - c) + c,
        axis.y * axis.z * (1 - c) - s * axis.x,
        0.0,
        axis.z * axis.x * (1 - c) - s * axis.y,
        axis.z * axis.y * (1 - c) + s * axis.x,
        axis.z * axis.z * (1 - c) + c,
        0.0,
        0.0, 0.0, 0.0, 1.0
    };
    return Transform(data);
}

Transform Transform::Rotate(double x, double y, double z, double theta) {
    Vector3 axis(x, y, z);
    return Rotate(axis, theta);
}

Transform Transform::operator*(const Transform& T) const {
    double *data = new double[16]{
        me(0,0) * T(0,0) + me(0,1) * T(1,0) + me(0,2) * T(2,0) + me(0,3) * T(3,0),
        me(0,0) * T(0,1) + me(0,1) * T(1,1) + me(0,2) * T(2,1) + me(0,3) * T(3,1),
        me(0,0) * T(0,2) + me(0,1) * T(1,2) + me(0,2) * T(2,2) + me(0,3) * T(3,2),
        me(0,0) * T(0,3) + me(0,1) * T(1,3) + me(0,2) * T(2,3) + me(0,3) * T(3,3),
        me(1,0) * T(0,0) + me(1,1) * T(1,0) + me(1,2) * T(2,0) + me(1,3) * T(3,0),
        me(1,0) * T(0,1) + me(1,1) * T(1,1) + me(1,2) * T(2,1) + me(1,3) * T(3,1),
        me(1,0) * T(0,2) + me(1,1) * T(1,2) + me(1,2) * T(2,2) + me(1,3) * T(3,2),
        me(1,0) * T(0,3) + me(1,1) * T(1,3) + me(1,2) * T(2,3) + me(1,3) * T(3,3),
        me(2,0) * T(0,0) + me(2,1) * T(1,0) + me(2,2) * T(2,0) + me(2,3) * T(3,0),
        me(2,0) * T(0,1) + me(2,1) * T(1,1) + me(2,2) * T(2,1) + me(2,3) * T(3,1),
        me(2,0) * T(0,2) + me(2,1) * T(1,2) + me(2,2) * T(2,2) + me(2,3) * T(3,2),
        me(2,0) * T(0,3) + me(2,1) * T(1,3) + me(2,2) * T(2,3) + me(2,3) * T(3,3),
        me(3,0) * T(0,0) + me(3,1) * T(1,0) + me(3,2) * T(2,0) + me(3,3) * T(3,0),
        me(3,0) * T(0,1) + me(3,1) * T(1,1) + me(3,2) * T(2,1) + me(3,3) * T(3,1),
        me(3,0) * T(0,2) + me(3,1) * T(1,2) + me(3,2) * T(2,2) + me(3,3) * T(3,2),
        me(3,0) * T(0,3) + me(3,1) * T(1,3) + me(3,2) * T(2,3) + me(3,3) * T(3,3)
    };
    return Transform(data);
}

Vector3 Transform::operator*(const Vector3& a) const {
    Vector3 b = Vector3(
        me(0, 0) * a.x + me(0, 1) * a.y + me(0, 2) * a.z + me(0, 3) * a.w,
        me(1, 0) * a.x + me(1, 1) * a.y + me(1, 2) * a.z + me(1, 3) * a.w,
        me(2, 0) * a.x + me(2, 1) * a.y + me(2, 2) * a.z + me(2, 3) * a.w,
        me(3, 0) * a.x + me(3, 1) * a.y + me(3, 2) * a.z + me(3, 3) * a.w
    );
    if(a.w == 0.0) b.w = 0.0;
    else b /= b.w, b.w = 1.0;
    return b;
}

Ray Transform::operator*(const Ray& ray) const {
    return Ray(operator*(ray.e), operator*(ray.d));
}

Transform Transform::translate(const Vector3 t) const {
    return translate(t.x, t.y, t.z);
}

Transform& Transform::translate_(const Vector3 t) {
    return translate_(t.x, t.y, t.z);
}

Transform Transform::translate(double x, double y, double z) const {
    Transform cp = copy();
    cp(0, 3) += x;
    cp(1, 3) += y;
    cp(2, 3) += z;
    return cp;
}

Transform& Transform::translate_(double x, double y, double z) {
    m[3] += x;
    m[7] += y;
    m[11] += z;
    if(*cnt & 1) {
        mi[3] -= x;
        mi[7] -= y;
        mi[11] -= z;
    }
    return *this;
}

Transform Transform::T() const {
    double *r = new double[16]{
        m[ 0], m[ 4], m[ 8], m[12],
        m[ 1], m[ 5], m[ 9], m[13],
        m[ 2], m[ 6], m[10], m[14],
        m[ 3], m[ 7], m[11], m[15]
    };
    return Transform(r);
}

Transform& Transform::T_() {
    double *tmp = new double[16] {
        m[ 0], m[ 4], m[ 8], m[12],
        m[ 1], m[ 5], m[ 9], m[13],
        m[ 2], m[ 6], m[10], m[14],
        m[ 3], m[ 7], m[11], m[15]
    };
    for(int i = 0; i < 16; ++i) m[i] = tmp[i];
    delete[] tmp, tmp = nullptr;

    if((*cnt) & 1) {
        tmp = new double[16]{
            mi[0], mi[4], mi[ 8], mi[12],
            mi[1], mi[5], mi[ 9], mi[13],
            mi[2], mi[6], mi[10], mi[14],
            mi[3], mi[7], mi[11], mi[15]
        };
        for(int i = 0; i < 16; ++i) mi[i] = tmp[i];
        delete[] tmp, tmp = nullptr;
    }
    
    return *this;
}

Transform Transform::i() {
    Transform Ti;
    Ti.m = mi, Ti.mi = m, Ti.cnt = cnt;
    (*cnt) += 2;
    if((*cnt) & 1) return Ti;
    
    (*cnt) |= 1;
    ++inverse_calcued;

    double _det = det();
    if(fabs(_det) < EPS) return Transform(); // no inversion
    
    mi[ 0] =  det_company(0, 0) / _det,
    mi[ 1] = -det_company(1, 0) / _det,
    mi[ 2] =  det_company(2, 0) / _det,
    mi[ 3] = -(mi[0] * m[3] + mi[1] * m[7] + mi[2] * m[11]);
    mi[ 4] = -det_company(0, 1) / _det,
    mi[ 5] =  det_company(1, 1) / _det,
    mi[ 6] = -det_company(2, 1) / _det,
    mi[ 7] = -(mi[4] * m[3] + mi[5] * m[7] + mi[6] * m[11]);
    mi[ 8] =  det_company(0, 2) / _det,
    mi[ 9] = -det_company(1, 2) / _det,
    mi[10] =  det_company(2, 2) / _det,
    mi[11] = -(mi[8] * m[3] + mi[9] * m[7] + mi[10] * m[11]);
    mi[12] = 0.0, mi[13] = 0.0, mi[14] = 0.0, mi[15] = 1.0;

    return Ti;
}

double Transform::det() const {
    return 
         m[0]*m[5]*m[10] + m[1]*m[6]*m[8] + m[2]*m[4]*m[9]
       - m[2]*m[5]*m[8]  - m[0]*m[6]*m[9] - m[1]*m[4]*m[10];
}

void Transform::print() const {
    printf("Transform 0x%x:\n", this);
    for(int i=0;i<4;++i){
        putchar('\t');
        for(int j=0; j<4; ++j) printf("%f ", m[4*i+j]);
        putchar('\n');
    }
}
