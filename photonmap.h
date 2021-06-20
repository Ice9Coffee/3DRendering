#pragma once

#include "vector3.h"
#include "color.h"
#include <vector>

class Photon {
public:
	Vector3 pos , dir;
	Color bright;
	int plane;
};

class Nearestphotons {
public:
	Vector3 pos;
	int max_photons, found;

	bool got_heap;
	double* dist2;
	Photon** photons;

	Nearestphotons();
	~Nearestphotons();
};

class Photonmap {
	int max_photons , stored_photons , emit_photons;
	Photon* photons;
	Vector3 box_min , box_max;

	void BalanceSegment( Photon* porg , int index , int st , int en );
	void MedianSplit( Photon* porg , int st , int en , int med , int axis );
	void LocatePhotons( Nearestphotons* np , int index ); //called by index = 1

public:
	Photonmap( int size = 0 );
	~Photonmap();
	
	int GetStoredPhotons() { return stored_photons; }
	void SetEmitPhotons( int photons ) { emit_photons = photons; }

	void Store( Photon );
	void Balance();
	Color GetIrradiance( Vector3 O , Vector3 N , double max_dist , int n );
};
