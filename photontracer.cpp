#include "photontracer.h"
#include <cstdlib>
#include <iostream>

const int MAX_PHOTONS = 10000000;
const int MAX_PHOTONTRACING_DEP = 10;

bool Photontracer::PhotonDiffusion( HitInfo* info , Photon photon , int dep , double* prob ) {
	Material* material = info->obj->material;
	double eta = ( material->diff + material->spec ) * material->color.brightness();

	if ( eta <= rand(0, 1) * ( *prob ) ) {
		*prob -= eta;
		return false;
	}

	photon.dir = info->N.diffuse();
	photon.bright = photon.bright * material->color / material->color.brightness();
	PhotonTracing( photon , dep + 1 );
	return true;
}

bool Photontracer::PhotonReflection( HitInfo* info , Photon photon , int dep , double* prob ) {
	Material* material = info->obj->material;
	double eta = material->rfle * material->color.brightness();

	if ( eta <= rand(0, 1) * ( *prob ) ) {
		*prob -= eta;
		return false;
	}
	
	photon.dir = photon.dir.reflect( info->N );
	photon.bright = photon.bright * material->color / material->color.brightness();
	PhotonTracing( photon , dep + 1 );
	return true;
}

bool Photontracer::PhotonRefraction( HitInfo* info , Photon photon , int dep , double* prob ) {
	Material* material = info->obj->material;
    double n = material->n_rfra;
	double eta = material->rfra;
	if ( !info->front ) {
        n = 1 / n;
        Color absor = info->obj->material->absor * info->t;
        Color trans = Color(exp(-absor.r), exp(-absor.g), exp(-absor.b));
		eta *= trans.brightness();
		photon.bright = photon.bright * trans / trans.brightness();
	}

	if ( eta <= rand(0, 1) * ( *prob ) ) {
		*prob -= eta;
		return false;
	}
	
	photon.dir = photon.dir.refract( info->N , n );
	PhotonTracing( photon , dep + 1 );
	return true;
}

void Photontracer::PhotonTracing( Photon photon , int dep ) {
	if ( dep > MAX_PHOTONTRACING_DEP ) return;

    Ray test_ray = Ray::Emit(photon.pos, photon.dir);
    HitInfo* info = new HitInfo();

    if(scene->Hit(test_ray, info)) {

		photon.pos = info->P;
		if ( info->obj->material->diff > EPS && dep > 1 )
			photonmap->Store( photon );

		double prob = 1;
		if ( PhotonDiffusion( info, photon , dep , &prob ) == false )
		if ( PhotonReflection( info, photon , dep , &prob ) == false )
		if ( PhotonRefraction( info, photon , dep , &prob ) == false );
	}
    delete info;
}

void Photontracer::Run() {
	int n = scene->camera.emit_photons;
	photonmap = new Photonmap( scene->camera.max_photons );
	photonmap->SetEmitPhotons( n );

	double total_bright = 0;
    for(Light* light : scene->light)
        total_bright += light->lightcolor.brightness();
	double photon_bright = total_bright / n;
	
	int emited_photons = 0;
    for(Light* light : scene->light) {
        double light_bright = light->lightcolor.brightness();
        while(light_bright >= photon_bright) {
            if((++emited_photons & 0xfffff) == 0) printf("Emited photons: %d\n", emited_photons);
            Photon photon = light->EmitPhoton();
            photon.bright = photon.bright * total_bright;
            PhotonTracing(photon, 1);
            light_bright -= photon_bright;
        }
        printf("Stored photons: %d\n", photonmap->GetStoredPhotons());
    }
	

    printf("Tree balancing...\n");
	photonmap->Balance();
}
