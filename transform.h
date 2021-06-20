#pragma once

#include "vector3.h"
#include "ray.h"

class Transform {
    double *m, *mi;
    unsigned *cnt;      //最低位用来表示mi是否已计算
    //Transform *Ti;

    // helper function
    //void init();
    void remove();
    double& operator()(int i, int j) { return m[4 * i + j]; }
    double det_company(int i, int j) const;

public:

    static unsigned inverse_calcued;

    Transform();
    ~Transform();
    Transform(double* _data);
    Transform(Vector3 u, Vector3 v, Vector3 w, Vector3 t);
    Transform(const Transform&);
    
    Transform& operator=(const Transform&);
    Transform copy() const;
    static Transform Scale(double k);
    static Transform Rotate(Vector3 axis, double theta);
    static Transform Rotate(double x, double y, double z, double theta);
    const double& operator()(int i, int j) const { return m[4*i+j]; }

    Transform  operator*(const Transform&) const;
    Vector3    operator*(const Vector3&)   const;
    Ray        operator*(const Ray&)       const;
    Transform  translate(const Vector3)    const;
    Transform& translate_(const Vector3);
    Transform  translate(double x, double y, double z) const;
    Transform& translate_(double x, double y, double z);
    Transform  T() const;
    Transform& T_();
    Transform  i();

    double det() const;

    void print() const;
};