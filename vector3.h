#pragma once

#include <cstdlib>
#include <ctime>
#include <vector>

const double EPS = 1e-6;
const double PI = 3.1415926535897932384626;
const double MAX_RAY_LENGTH = 1e6;
const double newton_leap = 1;

inline double rand(double a, double b) {
    return (b - a) * rand() / RAND_MAX + a;
}

class Vector3{
public:
    double x, y, z;
    double w;
    
    Vector3(double X = 0.0, double Y = 0.0, double Z = 0.0, double W = 1.0)
        : x(X), y(Y), z(Z), w(W) {}
    Vector3(const Vector3& V, double W) : Vector3(V.x, V.y, V.z, W) {}
    static Vector3 zero() { return Vector3(0.0, 0.0, 0.0); }
    static Vector3 rand_unity() {
        return Vector3( rand(-1, 1), rand(-1, 1), rand(-1, 1) ).unify();
    }

    double& GetCoord(int axis) {
        switch(axis) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: return w;
        }
    }

    // operations
    friend Vector3 operator+(const Vector3&, const Vector3&);
    friend Vector3 operator-(const Vector3&, const Vector3&);
    friend Vector3 operator*(const Vector3&, double);
    friend Vector3 operator*(double, const Vector3&);
    friend Vector3 operator/(const Vector3&, double);
    friend Vector3 operator-(const Vector3&);
    Vector3& operator+= (const Vector3&);
    Vector3& operator-= (const Vector3&);
    Vector3& operator*= (double);
    Vector3& operator/= (double);

    friend double dot(const Vector3&, const Vector3&);
    friend Vector3 cross(const Vector3&, const Vector3&);
    friend double mix(const Vector3&, const Vector3&, const Vector3&);
    friend double distance2(const Vector3&, const Vector3&);
    friend double distance(const Vector3&, const Vector3&);
    double dot(const Vector3&) const;
    Vector3 cross(const Vector3&) const;
    double length2() const;
    double length() const;
    double distance2(const Vector3&) const;
    double distance(const Vector3&) const;
    
    Vector3 unify() const;
    bool isZero() const;
    
    Vector3 reflect(Vector3 N) const;
    Vector3 refract(Vector3 N, double n) const;
    Vector3 rotate(Vector3 axis, double theta) const;
    Vector3 diffuse() const;

    void print() const;
};

unsigned Combine(int n, int i);

inline double Bernstein(int i, int n, double t) {
    return Combine(n, i) * pow(t, i) * pow(1 - t, n - i);
}
inline double d_Bernstein(int i, int n, double t) {
    return n * (Bernstein(i - 1, n - 1, t) - Bernstein(i, n - 1, t));
}