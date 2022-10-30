#pragma once
#ifndef PARTICLE_FORCE_POOL_H
#define PARTICLE_FORCE_POOL_H

#include "particle.hpp"
#include "particlefgen.hpp"

#include <vector>

struct ObjData;

class particleForcePool {
	private:
		struct particleForcePooler {
			particle *_particle;
			particleForceGen *_fg;
		};
		
		std::vector<particleForcePooler> pool;
	public:
		void add(particle* _particle, particleForceGen *_fg);
		void remove(particle* _particle, particleForceGen *_fg);
		void clear(particle* _particle, particleForceGen *_fg);
		void update(float dT);
		void draw(ObjData obj);
		int getSize();
		void updateForces(float dT);
};

#endif
