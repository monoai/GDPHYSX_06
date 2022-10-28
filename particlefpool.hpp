#pragma once
#ifndef PARTICLE_FORCE_GEN_H
#define PARTICLE_FORCE_GEN_H

#include "particle.hpp"
#include "particlefgen.hpp"

#include <vector>

class particleForcePool {
	private:
		struct particleForcePooler {
			particle *particle;
			particleForcePool *fg;
		};
		
		std::vector<particleForcePool> pool;
	public:
		void add(particle* particle, particleForceGen *fg);
		void remove(particle* particle, particleForceGen *fg);
		void clear(particle* particle, particleForceGen *fg);
		void updateForces(float duration);
};

#endif
