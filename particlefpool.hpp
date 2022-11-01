#pragma once
#ifndef PARTICLE_FORCE_POOL_H
#define PARTICLE_FORCE_POOL_H

#include "particle.hpp"
#include "particlefgen.hpp"

#include <vector>
#include <memory>

struct ObjData;

class particleForcePool {
	private:
		struct particleForcePooler {
			std::shared_ptr<particle> _particle;
			std::shared_ptr<particleForceGen> _fg;
		};
		
		std::vector<particleForcePooler> pool;
	public:
		void add(std::shared_ptr<particle> _particle, std::shared_ptr<particleForceGen> _fg);
		void remove(std::shared_ptr<particle> _particle, std::shared_ptr<particleForceGen> _fg);
		void clear(particle* _particle, particleForceGen *_fg);
		void update(float dT);
		void updateForces(float dT);
		void draw(ObjData obj);
		void checkLife();
		int getSize();
		void getContents();
};

#endif
