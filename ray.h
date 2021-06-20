#pragma once

#include "vector3.h"

class Ray {
public:
    Vector3 e;	// emit point
    Vector3 d;	// direction

    Ray(Vector3 E, Vector3 D) : e(E), d(D) { e.w = 1.0, d.w = 0.0; }
    static Ray Emit(Vector3 emit, Vector3 direction) {
        return Ray(emit, direction.unify());
    }
    static Ray FromTo(Vector3 from, Vector3 to) {
        return Ray(from, (to - from).unify());
    }

    Vector3 unit_d() { return d.unify(); }
};