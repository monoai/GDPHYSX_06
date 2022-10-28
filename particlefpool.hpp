#pragma once
#ifndef PARTICLE_FORCE_POOL_H
#define PARTICLE_FORCE_POOL_H

#include "particle.hpp"
#include "particlefgen.hpp"

#include <vector>

class particleForcePool {
	private:
		struct particleForcePooler {
			particle *particle;
			particleForceGen *fg;
		};
		
		std::vector<particleForcePooler> pool;
	public:
		void add(particle* particle, particleForceGen *fg);
		void remove(particle* particle, particleForceGen *fg);
		void clear(particle* particle, particleForceGen *fg);
		void updateForces(float dT);
};

#endif
