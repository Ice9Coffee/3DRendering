#include "material.h"
#include <cmath>


Material* Material::DEFAULT = new Material(
    Color::white(), 0.6, 0.2, 10.0, 0.0,
    Color::black(), 0.0, 1.0);

Material::Material(Color _color, double diffusion, double spection, double spection_pow, double reflection,
    Color absor_color, double refraction, double refraction_rate) 
    : color(_color), absor(absor_color)
{
    diff = diffusion;
    spec = spection;
    spec_pow = spection_pow;
    rfle = reflection;
    rfra = refraction;
    n_rfra = refraction_rate;
    tex = nullptr;
}

Color Material::BRDF(Vector3 L, Vector3 N, Vector3 V, Color lightClr, double u, double v) {

    if(dot(L, N) * dot(V, N) < 0) return Color::black();

    Color tex_cl = color;
    if(tex && 0 <= u && u <= 1 && 0 <= v && v <= 1)
        tex_cl = tex_cl * tex->at(u, v);

    L = L.unify(), N = N.unify(), V = V.unify();
    return diff * fabs(dot(L, N)) * tex_cl + spec * pow(-dot(V, L.reflect(N)), spec_pow) * lightClr;
}
