#pragma once

#include "vector3.h"
#include "transform.h"
#include "material.h"
#include "ray.h"
#include <vector>

class Primitive;

class HitInfo {
public:
    HitInfo() { hit = false, front = true, t = 0, obj = nullptr; }
    bool hit;       // if hit
    Vector3 P;		// the hit point
    Vector3 P_uvw;  // the hit point in space(u,v,w)
    Vector3 N;		// the normal vector
    Vector3 V;      // the view direction
    bool front;		// if hit from the front
    double t;       // t of Ray(t) = e + t*d
    double tex_u, tex_v;
    Primitive *obj; // the hit object
};

class Primitive {
protected:
    Transform M;
public:
    Material *material;

    Primitive(Material* mat);

    bool Hit(const Ray& ray, HitInfo* info = nullptr);
    virtual bool Hit(const Ray& ray, double& t, HitInfo* out = nullptr) = 0;
};

class Plane : public Primitive {
    Vector3 _O, _N, _u, _v;
public:
    Plane(Vector3 O, Vector3 u, Vector3 v, Material* mat);
    bool Hit(const Ray& ray, double& t, HitInfo* out = nullptr) override;
};

class Sphere : public Primitive {
public:
    Sphere(Vector3 O, double r, double theta, double phi, Material* mat);
    bool Hit(const Ray& ray, double& t, HitInfo* out = nullptr) override;
};

class Bezier_rotated : public Primitive {
    std::vector<double> P_r; //控制点
    std::vector<double> P_z; //控制点
    int n;
    double max_r, max_z, min_z;

public:
    Bezier_rotated(Material* mat) : Primitive(mat) { n = -1; }
    
    void setTransform(Transform transform);

    void addPoint(double r, double z);

    Vector3 S(double t, double theta);
    Vector3 dS_dt(double t, double theta);
    Vector3 dS_dtheta(double t, double theta);

    bool Hit(const Ray& ray, double& t, HitInfo* out = nullptr) override;
};