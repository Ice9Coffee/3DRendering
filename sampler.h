#pragma once

#include "vector3.h"
#include "color.h"
#include <cmath>

class Film {
    int w_pix, h_pix;
    Color *data;
public:
    Film(int W, int H) {
        w_pix = W, h_pix = H;
        data = new Color[W * H];
    }
    ~Film() { delete[] data; data = nullptr; }

    int W() const { return w_pix; }
    int H() const { return h_pix; }
    Color& at(int k) { return data[k]; }
    Color& at(int i, int j) { return data[w_pix * i + j]; }
};

class Sampler {
protected:
    int w_pix, h_pix, n_pix;
    double w, h;
    int w_SS, h_SS, n_SS;
    Sampler() {}
public:
    Sampler(int W, int H, double fov = 1.0, unsigned SSAA_W = 1, unsigned SSAA_H = 1) {
        w_pix = W, h_pix = H, n_pix = W * H;
        h = atan(fov), w = h * (double)W / (double)H;
        w_SS = SSAA_W, h_SS = SSAA_H, n_SS = SSAA_W * SSAA_H;
    }

    int SSAA() const { return n_SS; }
    bool legal(int k) const { return (0 <= k && k < n_pix * n_SS); }
    Vector3 point(int k) const {
        int ss = k % n_SS;
        int ss_i = ss / w_SS,
            ss_j = ss % w_SS;
        k = k / n_SS;
        return point(k / w_pix, k % w_pix, ss_i, ss_j);
    }
    virtual Vector3 point(int i, int j, int ss_i, int ss_j) const {
        if(n_SS > 1) {
            return Vector3(
                w * (2 * j + ((double)(2 * ss_j + 1) / w_SS)) / w_pix - w,
                -h * (2 * i + ((double)(2 * ss_i + 1) / h_SS)) / h_pix + h,
                1.0);
        }
        else {
            return Vector3(
                ((double)(2 * j + 1) / w_pix - 1.0) * w,
                (1.0 - (double)(2 * i - 1) / h_pix) * h,
                1.0);
        }
    }
    Film* genFilm() const { return new Film(w_pix, h_pix); }
};

class WideAngleSampler : public Sampler {
public:
    WideAngleSampler(int W, int H, double fov = 1.0, unsigned SSAA_W = 1, unsigned SSAA_H = 1) {
        w_pix = W, h_pix = H, n_pix = W * H;
        w = fov, h = w * (double)H / (double)W;
        w_SS = SSAA_W, h_SS = SSAA_H, n_SS = SSAA_W * SSAA_H;
    }

    Vector3 point(int i, int j, int ss_i, int ss_j) const override {
        double u, v;
        if(n_SS > 1) {
            u = w * (2 * j + ((double)(2 * ss_j + 1) / w_SS)) / w_pix - w;
            v = -h * (2 * i + ((double)(2 * ss_i + 1) / h_SS)) / h_pix + h;
        }
        else {
            u = ((double)(2 * j + 1) / w_pix - 1.0) * w;
            v = (1.0 - (double)(2 * i - 1) / h_pix) * h;
        }

        double phi = sqrt(u * u + v * v);

        return Vector3(
            cos(v) * sin(u),
            sin(v),
            cos(v) * cos(u)
        );
       
        //return Vector3(
        //    sin(phi) * u / phi,
        //    sin(phi) * v / phi,
        //    cos(phi)
        //);
    }
};

class FishEyeSampler : public Sampler {
public:
    FishEyeSampler(int W, int H, double fov = 1.0, unsigned SSAA_W = 1, unsigned SSAA_H = 1) {
        w_pix = W, h_pix = H, n_pix = W * H;
        double unit = sin(fov) / sqrt(W * W + H * H);
        h = unit * H;
        w = unit * W;
        w_SS = SSAA_W, h_SS = SSAA_H, n_SS = SSAA_W * SSAA_H;
    }

    Vector3 point(int i, int j, int ss_i, int ss_j) const override {
        double u, v;
        if(n_SS > 1) {
            u = w * (2 * j + ((double)(2 * ss_j + 1) / w_SS)) / w_pix - w;
            v = -h * (2 * i + ((double)(2 * ss_i + 1) / h_SS)) / h_pix + h;
        }
        else {
            u = ((double)(2 * j + 1) / w_pix - 1.0) * w;
            v = (1.0 - (double)(2 * i - 1) / h_pix) * h;
        }

        return Vector3(
            u,
            v,
            sqrt(1 - u * u - v * v)
        );

        //return Vector3(
        //    sin(phi) * u / phi,
        //    sin(phi) * v / phi,
        //    cos(phi)
        //);
    }
};