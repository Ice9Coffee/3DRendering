#include "camera.h"

Camera::Camera() :
    M(Transform::Scale(1.0))
{
    max_photons = 5000000;
    emit_photons = 2000000;
    sample_photons = 50;
    sample_dist = 1;
}

Camera::Camera(Vector3 Pos, Vector3 Up, Vector3 LookAt) {
    Vector3 w(LookAt.unify(), 0.0);
    Vector3 u(cross(Up, w).unify(), 0.0);
    Vector3 v(cross(w, u), 0.0);
    M = Transform(u, v, w, Pos);
}

void Camera::set(const Transform & Matr) {
    M = Matr;
}

void Camera::set(Vector3 Pos, Vector3 Up, Vector3 LookAt) {
    Vector3 w(LookAt.unify(), 0.0);
    Vector3 u(cross(Up, w).unify(), 0.0);
    Vector3 v(cross(w, u), 0.0);
    M = Transform(u, v, w, Pos);
}

Ray Camera::ray(Vector3 P) {
    return Ray::FromTo(M * Vector3::zero(), M * P);
}