#pragma once

#include "color.h"
#include "vector3.h"
#include <vector>

class Texture {
public:
    int w_pix, h_pix;
    std::vector<Color> map;
    Color at(double u, double v) {
        u *= h_pix, v *= w_pix;
        int i = (int)u, j = (int)v;
        u -= i, v -= j;
        if(w_pix * i + j + w_pix + 1 < w_pix * h_pix)
            return 0.25 * 
                (map[w_pix * i + j] * (1-u) * (1-v) + 
                map[w_pix * i + j + 1] * (1-u) * v +
                map[w_pix * i + j + w_pix] * u * (1-v) +
                map[w_pix * i + j + w_pix + 1] * u * v);
        else return 0.25 * map[w_pix * i + j];
    }
};

class Material {
public:
	Color color, absor;
	double rfle, rfra, diff, spec, spec_pow;
	double n_rfra;
    Texture* tex;

    Material(Color color, double diffusion, double spection, double spection_pow, double reflection,
        Color absor_color, double refraction, double refraction_rate);
    Color BRDF(Vector3 L, Vector3 N, Vector3 V, Color lightClr, double u = -1.0, double v = -1.0);

    static Material* DEFAULT;
};
