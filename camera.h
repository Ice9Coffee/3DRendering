#pragma once

#include "vector3.h"
#include "transform.h"

class Camera {
public:
    Transform M;

   	int max_photons;
	int emit_photons;
	int sample_photons;
	double sample_dist;

    Camera();
    Camera(Vector3 Pos, Vector3 Up, Vector3 LookAt);
    void set(const Transform& Matr);
    void set(Vector3 Pos, Vector3 Up, Vector3 LookAt);
    Ray ray(Vector3 P);
};
