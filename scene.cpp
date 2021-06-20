#include "scene.h"

bool Scene::Hit(const Ray& ray, HitInfo* out) {
    double unused;
    return Hit(ray, unused, out);
}

bool Scene::Hit(const Ray& ray, double & t, HitInfo* out) {
    
    t = MAX_RAY_LENGTH;
    double dist;
    int prim_id = -1;
    HitInfo *cur_info = nullptr;
    if(out) cur_info = new HitInfo;

    for(int i = 0; i < prim.size(); ++i) {
        if(prim[i]->Hit(ray, dist, cur_info) && dist < t) {
            t = dist;
            if(out) *out = *cur_info;
            prim_id = i;
        }
    }

    delete cur_info;
    cur_info = nullptr;

    return (prim_id != -1);
}
