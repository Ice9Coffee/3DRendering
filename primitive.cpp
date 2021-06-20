#include "primitive.h"
#include <cmath>

Primitive::Primitive(Material* mat):
    material(mat)
{}

bool Primitive::Hit(const Ray& ray, HitInfo* out) {
    double unused;
    return Hit(ray, unused, out);
}

Plane::Plane(Vector3 O, Vector3 u, Vector3 v, Material* mat) : Primitive(mat) {
    _u = u, _v = v, _O = O;
    _N = cross(_u, _v).unify();
    _N.w = _u.w = _v.w = 0.0;
    M = Transform(_u, _v, _N, _O);
}

bool Plane::Hit(const Ray& ray, double& t, HitInfo* out) {
    double c = -dot(ray.d, _N);
    t = -dot(_O - ray.e, _N) / c;

    // too inclined or opposite direction
    if(fabs(c) < EPS || t < EPS) return false;

    if(out) {
        out->hit = true;
        out->P = ray.e + t * ray.d;
        out->P_uvw = M.i() * out->P;
        out->tex_u = 0.5 - out->P_uvw.y;
        out->tex_u -= floor(out->tex_u);
        out->tex_v = out->P_uvw.x - 0.5;
        out->tex_v -= floor(out->tex_v);
        out->N = _N;
        out->V = ray.d;
        out->front = (c > -EPS);
        out->t = t;
        out->obj = this;
    }
    return true;
}

Sphere::Sphere(Vector3 O, double r, double theta, double phi, Material* mat) : Primitive(mat) {
    M = Transform::Rotate(0.0, 1.0, 0.0, theta) *
        Transform::Rotate(0.0, 0.0, 1.0, phi) *
        Transform::Scale(r);
    M.translate_(O);
    //M.print();
}

bool Sphere::Hit(const Ray& ray, double& t, HitInfo* out) {
    
    Vector3 e_ = M.i() * ray.e;
    Vector3 d_ = M.i() * ray.d;
    
    double dot_ed = dot(e_, d_), d_l2 = d_.length2();
    double discrim = dot_ed * dot_ed - d_l2 * (e_.length2() - 1);
    if(discrim < EPS) return false;  // not hit

    discrim = sqrt(discrim);
    double t1 = (-dot_ed - discrim) / d_l2,
           t2 = (-dot_ed + discrim) / d_l2;
    bool front = true;
    if(t2 < EPS) return false;       // opposite direction
    if(t1 > EPS) {
        t = t1;
        front = true;
    }
    else {
        t = t2;
        front = false;
    }
    if(out) {
        Vector3 P_ = e_ + t * d_;
        Vector3 N_ = Vector3(P_, 0.0);
        out->hit = true;
        out->P = ray.e + t * ray.d;
        out->P_uvw = P_;
        out->N = M.i().T() * N_;
        out->V = ray.d;
        out->front = front;
        out->t = t;
        out->obj = this;
                
        out->tex_u = acos(P_.y) / PI;
        double theta = acos(P_.z / sqrt(P_.z * P_.z + P_.x * P_.x)) / PI / 2.0;
        if(P_.x > 0.0) theta = 1.0 - theta;
        out->tex_v = theta;
    }

    //printf("Hit!\n");
    return true;
}

void Bezier_rotated::setTransform(Transform transform) {
    M = transform;
}

void Bezier_rotated::addPoint(double r, double z) {
    ++n; P_r.push_back(r); P_z.push_back(z);

    if(n == 0) max_r = fabs(r), max_z = min_z = z;
    else {
        max_r = fmax(max_r, fabs(r));
        max_z = fmax(max_z, z);
        min_z = fmin(min_z, z);
    }
}

Vector3 Bezier_rotated::S(double t, double theta) {
    double r = 0, z = 0;
    for(int i = 0; i <= n; ++i)
        r += P_r[i] * Bernstein(i, n, t),
        z += P_z[i] * Bernstein(i, n, t);

    return Vector3(r * cos(theta), r * sin(theta), z, 1);
}

Vector3 Bezier_rotated::dS_dt(double t, double theta) {
    double dr = 0, dz = 0;
    for(int i = 0; i <= n; ++i)
        dr += P_r[i] * d_Bernstein(i, n, t),
        dz += P_z[i] * d_Bernstein(i, n, t);
    
    return Vector3(dr * cos(theta), dr * sin(theta), dz, 0);
}

Vector3 Bezier_rotated::dS_dtheta(double t, double theta) {
    double r = 0;
    for(int i = 0; i <= n; ++i) r = r + P_r[i] * Bernstein(i, n, t);
    
    return Vector3(-r * sin(theta), r * cos(theta), 0, 0);
}

bool Bezier_rotated::Hit(const Ray& ray, double& t, HitInfo* out) {

    Vector3 e_ = M.i() * ray.e;
    Vector3 d_ = M.i() * ray.d;
    double t0, dt, u, v;
    double du, dv;
    Vector3 dC_dt, dS_du, dS_dv, f;

    //圆柱包围盒
    double a = d_.x * d_.x + d_.y * d_.y,
        b = e_.x * d_.x + e_.y * d_.y,
        c = e_.x * e_.x + e_.y * e_.y - max_r * max_r;
    double discrim = b * b - a * c;
    if(discrim < EPS) return false;  // not hit

    discrim = sqrt(discrim);
    double t1 = (-b - discrim) / a,
        t2 = (-b + discrim) / a;
    if(t2 < EPS) return false;
    
    Vector3 P1 = e_ + t1 * d_,
        P2 = e_ + t2 * d_;
    if(P1.z < min_z && P2.z < min_z || P1.z > max_z && P2.z > max_z) return false;


    for(int k = 0; k < 2; ++k) {    //牛顿法迭代两点 《计算机图形学基础教程(第2版)》P107
        //选初值
        t0 = (k == 0) ? t1 : t2;

        Vector3 P0 = (k == 0) ? P1 : P2;
        u = (P0.z - max_z) / (min_z - max_z);
        v = acos(P0.unify().x);
        if(P0.y < 0) v = -v;
        if(u - 1.0 > EPS) u = 0.98;
        if(u < EPS) u = 0.02;
        
        //迭代
        int newton_cnt = 0;
        do {
            dC_dt = d_,
                dS_du = dS_dt(u, v),
                dS_dv = dS_dtheta(u, v);
            f = (e_ + t0 * d_ - S(u, v));
            double D = mix(dC_dt, dS_du, dS_dv);

            if(fabs(D) < EPS) {
                t0 += rand(-0.02, 0.02), u += rand(-0.02, 0.02), v += rand(-0.02, 0.02);
                --newton_cnt; dt = 1;
                continue;
            }

            dt = -mix(dS_du, dS_dv, f) / D;
            du = -mix(dC_dt, dS_dv, f) / D;
            dv = mix(dC_dt, dS_du, f) / D;
            t0 += dt * newton_leap, u += du * newton_leap, v += dv * newton_leap;

            if(u < EPS || u - 1.0 > -EPS) u = rand(0, 1), newton_cnt -= 1;

        } while(f.length() > 0.001 && ++newton_cnt < 10);

        if(f.length() < 0.002 && t0 >= EPS) 
            break;  //ok
    }  
    if(f.length() >= 0.002 || t0 < EPS) return false;

    t = t0;
    if(out) {
        out->hit = true;
        out->P = ray.e + t0 * ray.d;
        out->P_uvw = e_ + t0 * ray.d;
        out->N = M.i().T() * cross(dS_dt(u, v), dS_dtheta(u, v));
        out->V = ray.d;
        out->front = (dot(out->N, out->V) < EPS);
        out->t = t0;
        out->obj = this;

        out->tex_u = u;
        double round = v / (2 * PI);
        out->tex_v = round - floor(round);
    }

    return true;
}
