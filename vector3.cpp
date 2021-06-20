#include "vector3.h"
#include <cmath>
#include <cstdio>

unsigned Combine(int n, int i) {
    static std::vector<unsigned*> c;

    if(n < 0) return Combine(i - n - 1, i);     //负二项式
    if(i < 0 || i > n) return 0;                //越界
    if(n < c.size()) return c[n][i];            //已计算

    if(c.size() == 0) c.push_back(new unsigned(1)); //递归基

    for(int k = c.size(); k <= n; ++k) {        //拓展
        c.push_back(new unsigned[k + 1]);
        for(int j = 0; j <= k; ++j) c[k][j] = Combine(k - 1, j - 1) + Combine(k - 1, j);
    }

    return c[n][i];
}

Vector3 operator+(const Vector3& A, const Vector3& B){
    return Vector3(A.x + B.x, A.y + B.y, A.z + B.z);
}
Vector3 operator-(const Vector3& A, const Vector3& B){
    return Vector3(A.x - B.x, A.y - B.y, A.z - B.z);
}
Vector3 operator*(const Vector3& A, double k) {
    return Vector3(A.x * k, A.y * k, A.z * k);
}
Vector3 operator/(const Vector3& A, double k) {
    return Vector3(A.x / k, A.y / k, A.z / k);
}
Vector3 operator*(double k, const Vector3& A) {
    return A * k;
}
Vector3 operator-(const Vector3& A) {
    return Vector3(-A.x, -A.y, -A.z);
}
double dot(const Vector3& A, const Vector3& B) {
    return A.x * B.x + A.y * B.y + A.z * B.z;
}
Vector3 cross(const Vector3& A, const Vector3& B) {
    return Vector3(
        A.y*B.z - A.z*B.y, 
        A.z*B.x - A.x*B.z, 
        A.x*B.y - A.y*B.x);
}
double mix(const Vector3& A, const Vector3& B, const Vector3& C) {
    return dot(A, cross(B, C));
}
double distance2(const Vector3& A, const Vector3& B){
    return (A - B).length2();
}
double distance(const Vector3& A, const Vector3& B){
    return (A - B).length();
}

Vector3& Vector3::operator+=(const Vector3& B) {
    x += B.x, y += B.y, z += B.z;
    return *this;
}
Vector3& Vector3::operator-=(const Vector3& B) {
    x -= B.x, y -= B.y, z -= B.z;
    return *this;
}
Vector3& Vector3::operator*=(double k) {
    x *= k, y *= k, z *= k;
    return *this;
}
Vector3& Vector3::operator/=(double k) {
    x /= k, y /= k, z /= k;
    return *this;
}
double Vector3::dot(const Vector3& B) const {
    return ::dot(*this, B);
}
Vector3 Vector3::cross(const Vector3& B) const {
    return ::cross(*this, B);
}
double Vector3::length2() const {
    return x * x + y * y + z * z;
}
double Vector3::length() const {
    return sqrt(x * x + y * y + z * z);
}
double Vector3::distance2(const Vector3& B) const {
    return ::distance2(*this, B);
}
double Vector3::distance(const Vector3& B) const {
    return ::distance(*this, B);
}
Vector3 Vector3::unify() const {
    return *this / length();
}
bool Vector3::isZero() const {
    return fabs(x) < EPS && fabs(y) < EPS && fabs(z) < EPS;
}
Vector3 Vector3::reflect(Vector3 N) const {
    N = N.unify();
    return *this - 2 * dot(N) * N;
}
Vector3 Vector3::refract(Vector3 N, double n) const {
    Vector3 V = this->unify();
    N = N.unify();
    n = 1.0 / n;    //方便计算
    double cosI = ::dot(V, N);
    if(cosI > 0.0) N = -N;
    else cosI = -cosI;
    double cosT2 = 1 - (1 - cosI * cosI) * (n * n);
    if(cosT2 < EPS) return V.reflect(N); // full reflection
    else return ( V*n + N*(cosI * n - sqrt(cosT2)) ) * length();
}
Vector3 Vector3::rotate(Vector3 axis, double theta) const {
    Vector3 R = Vector3::zero();
    axis = axis.unify();
    double c = cos(theta), s = sin(theta);
    R.x += x * (axis.x * axis.x * (1-c) + c);
    R.x += x * (axis.x * axis.y * (1-c) - s * axis.z);
    R.x += x * (axis.x * axis.z * (1-c) + s * axis.y);
    R.y += y * (axis.y * axis.x * (1-c) + s * axis.z);
    R.y += y * (axis.y * axis.y * (1-c) + c);
    R.y += y * (axis.y * axis.z * (1-c) - s * axis.x);
    R.z += z * (axis.z * axis.x * (1-c) - s * axis.y);
    R.z += z * (axis.z * axis.y * (1-c) + s * axis.x);
    R.z += z * (axis.z * axis.z * (1-c) + c);
	return R;
}
Vector3 Vector3::diffuse() const {
    Vector3 d(0.0, 1.0, 0.0);
    Vector3 axis = this->cross(d);
    double theta = acos(this->unify().dot(d));

    d = d.rotate(Vector3(1.0, 0.0, 0.0), acos(sqrt(rand(0, 1))))
        .rotate(Vector3(0.0, 1.0, 0.0), rand(0, 2 * PI));

    if(axis.length2() > EPS) {
        d = d.rotate(axis, -theta);
    }
    
    return d;
}


void Vector3::print() const {
    printf("Vector3 0x%x: [%f, %f, %f, %f]", this, x, y, z, w);
}
