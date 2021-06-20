#pragma once

#include "scene.h"
#include "photonmap.h"

extern const int MAX_PHOTONS;
extern const int MAX_PHOTONTRACING_DEP;

class Photontracer {
	Scene* scene;
	Photonmap* photonmap;

	void PhotonTracing( Photon , int dep );
	bool PhotonDiffusion( HitInfo* , Photon , int dep , double* prob );
	bool PhotonReflection( HitInfo* , Photon , int dep , double* prob );
	bool PhotonRefraction( HitInfo* , Photon , int dep , double* prob );

public:
	void SetScene( Scene* input ) { scene = input; }
	Photonmap* GetPhotonmap() { return photonmap; }
	void Run();
};
