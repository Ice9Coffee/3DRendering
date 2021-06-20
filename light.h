#pragma once

#include "color.h"
#include "vector3.h"
#include "primitive.h"
#include "photonmap.h"

class Scene;

class Light {
public:
    double lumin;
    Color lightcolor;

    Light(double _lumin, Color color) :
        lumin(_lumin), lightcolor(color)
    {}

    Color CaQIrd(Vector3 S, HitInfo* info, Scene* scene);
    virtual Color GetIrd(HitInfo* info, Scene* scene, unsigned quality) = 0;
    virtual Photon EmitPhoton() = 0;
};

class PointLight :public Light {
    Vector3 O;
    double jiggle;

public:
    PointLight(double _lumin, Color color, Vector3 pos, double _jiggle = 0.05) :
        Light(_lumin, color)
    {
        O = pos;
        jiggle = _jiggle;
    }

    Color GetIrd(HitInfo* info, Scene* scene, unsigned quality) override;
    Photon EmitPhoton() override;
};

class AreaLight : public Light {
    Transform M;
    double area;

public:
    AreaLight(double _lumin, Color color, Transform transform) :
        Light(_lumin, color)
    {
        M = transform;
        area = cross(M*Vector3(1,0,0), M*Vector3(0,1,0)).length();
    }

    Color GetIrd(HitInfo* info, Scene* scene, unsigned quality) override;
    Photon EmitPhoton() override;    
};



#include "scene.h"