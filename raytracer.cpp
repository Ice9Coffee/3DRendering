#include "raytracer.h"
#include <cmath>
#include <iostream>

Color RayTracer::trcDiff(HitInfo* info, Scene* scene, double ambient)
{
    Material *material = info->obj->material;
    Color c = ambient * material->color;

    c = c + material->color * 
        photonmap->GetIrradiance(info->P, info->N, scene->camera.sample_dist, scene->camera.sample_photons) * material->diff;

    for(int lt = 0; lt < scene->light.size(); ++lt) {
        c = c + scene->light[lt]->GetIrd(info, scene, light_quality);
    }
    return c;
}

Color RayTracer::trcRfle(HitInfo* info, Scene* scene, int dep, double decay) {
    return info->obj->material->rfle * Trace(
        Ray::Emit(info->P, info->V.reflect(info->N)),
        scene,
        dep - 1,
        decay * info->obj->material->rfle
    );
}

Color RayTracer::trcRfra(HitInfo* info, Scene * scene, int dep, double decay) {
    double n = info->obj->material->n_rfra;
    Color trans = Color::white();
    if(!info->front) {
        n = 1.0 / n;
        Color absor = info->obj->material->absor * info->t;
        trans = Color(exp(-absor.r), exp(-absor.g), exp(-absor.b));
    }
    
    return info->obj->material->rfra * trans * Trace(
        Ray::Emit(info->P, info->V.refract(info->N, n)),
        scene,
        dep - 1,
        decay * info->obj->material->rfra
    );
}

Color RayTracer::Trace(const Ray& ray, Scene* scene, int dep, double decay) {
    if(dep == 0 || decay < DECAY_LIMIT) return Color::black();

    Color c = Color::black();
    HitInfo *info = new HitInfo;

    if(scene->Hit(ray, info)) {

        Material* mat = info->obj->material;

        if(mat->diff > EPS || mat->spec > EPS)
            c = c + trcDiff(info, scene, ambient);
        if(mat->rfle > EPS)
            c = c + trcRfle(info, scene, dep, decay);
        if(mat->rfra > EPS)
            c = c + trcRfra(info, scene, dep, decay);
    }
    
    delete info;
    info = nullptr;

    return c;
}

RayTracer::RayTracer() {
    max_dep = 5;
    light_quality = 4;
    ambient = 0.1;
    photonmap = nullptr;
}


RayTracer::~RayTracer() {
    delete photonmap;
}

Film* RayTracer::Run(Scene* scene, Sampler* sampler) {

    Photontracer* photontracer = new Photontracer;
    photontracer->SetScene(scene);
    photontracer->Run();
    photonmap = photontracer->GetPhotonmap();
    delete photontracer;


    Film *film = sampler->genFilm();
    int ssaa = sampler->SSAA();
    Color c = Color::black();
    
    for(int k = 0; sampler->legal(k); ++k) {
        c = raw_sum(c, Trace(scene->camera.ray(sampler->point(k)), scene, max_dep, 1.0));
        if(k % ssaa == ssaa - 1) {
            film->at(k / ssaa) = c / ssaa;
            c = Color::black();
        }
        if(k % 10000 == 0)
            printf("Tracing pixel %d...\n", k);
    }
    return film;
}
