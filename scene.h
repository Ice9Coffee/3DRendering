#pragma once

#include "primitive.h"
#include "light.h"
#include "camera.h"
#include <vector>

class Scene {
public:
    std::vector<Primitive*> prim;
    std::vector<Light*> light;
    Camera camera;

    bool Hit(const Ray& ray, HitInfo* out = nullptr);
    bool Hit(const Ray& ray, double & t, HitInfo* out = nullptr);
};