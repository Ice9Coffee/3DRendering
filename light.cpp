#include "light.h"
#include <cmath>

Color Light::CaQIrd(Vector3 S, HitInfo* info, Scene* scene) {
    Ray test_ray = Ray::FromTo(info->P, S);
    double test_dis;
    double PS_length2 = (S - info->P).length2();
    double PS_length = sqrt(PS_length2);

    if(scene->Hit(test_ray, test_dis) == false || PS_length <= test_dis) {
        return lumin * lightcolor * info->obj->material->BRDF(-test_ray.d, info->N, info->V, lightcolor, info->tex_u, info->tex_v) / (PS_length2 + 1.0);
    }
    else return Color::black();
}


Color PointLight::GetIrd(HitInfo* info, Scene* scene, unsigned quality) {
    Color result = Color::black();
    quality <<= 4;
    for(int i = quality; i > 0; --i) {
        Vector3 S = O + Vector3::rand_unity() * jiggle;
        result = raw_sum(result, CaQIrd(S, info, scene));
    }
    result = result / (quality);
    return result;
}

Photon PointLight::EmitPhoton() {
	Photon pht;
	pht.bright = lightcolor / lightcolor.brightness();
	pht.pos = O;
	pht.dir = Vector3::rand_unity();
	return pht;
}


Color AreaLight::GetIrd(HitInfo * info, Scene * scene, unsigned quality) {
    // Only light y<0
    //if((M.i() * info->P).y > EPS) return Color::black();

    int qualt_x_aera = quality * area;
    if(qualt_x_aera < 1) qualt_x_aera = 1;
    Color result = Color::black();

    for(int i = -2; i < 2; ++i) for(int j = -2; j < 2; ++j)
        for(int k = qualt_x_aera; k > 0; --k) {
            Vector3 S((rand(i, i + 1)) / 2, (rand(j, j + 1)) / 2, 0.0);
            result = raw_sum(result, CaQIrd(M * S, info, scene));
        }
    result = result / (qualt_x_aera * 16);

    return result;
}

Photon AreaLight::EmitPhoton() {
	Photon pht;
	pht.bright = lightcolor / lightcolor.brightness();
	pht.pos = M * Vector3( rand(-1 , 1), 0.0, rand(-1, 1));
	pht.dir = Vector3::rand_unity();
	return pht;
}
