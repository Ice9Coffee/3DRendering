#pragma once

#include "scene.h"
#include "sampler.h"
#include "photontracer.h"
#include <cmath>

const unsigned COLOR_BIT = 8;
const double DECAY_LIMIT = 0.01 / (1 << COLOR_BIT);

class RayTracer {
    unsigned max_dep;           //最大追踪深度
    unsigned light_quality;     //软阴影质量：点光源采样次数/面光源单位面积采样次数
    double ambient;             //环境光系数
    Photonmap *photonmap;       //光子图（由PhotonTracer得到）

    Color trcDiff(HitInfo* info, Scene* scene, double ambient);         //追踪漫反射光
    Color trcRfle(HitInfo* info, Scene* scene, int dep, double decay);  //追踪反射光
    Color trcRfra(HitInfo* info, Scene* scene, int dep, double decay);  //追踪折射光
    Color Trace(const Ray& ray, Scene* scene, int dep, double decay);   //追踪光线ray

public:
    RayTracer();
    ~RayTracer();

    void setMaxDepth(unsigned dep) { max_dep = dep; }
    void setLightQuality(unsigned quality) { light_quality = quality; }
    void setAmbient(double _ambient) { ambient = _ambient; }

    Film* Run(Scene *scene, Sampler *sampler);      //主循环
};
