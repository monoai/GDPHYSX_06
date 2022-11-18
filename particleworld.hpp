#pragma once
#ifndef PARTICLE_WORLD_H
#define PARTICLE_WORLD_H

#include "particle.hpp"
#include "particlefpool.hpp"
#include "particlecontact.hpp"

#include <vector>
#include <memory>

class particleWorld {
	struct particlePooler {
		std::shared_ptr<particle> _particle;
		std::shared_ptr<particlePooler> next;
	};

	std::shared_ptr<particlePooler> firstParticle;

	struct contactGenPooler {
		std::shared_ptr<particleContactGen> gen;
		std::shared_ptr<contactGenPooler> next;
	};

	std::shared_ptr<contactGenPooler> firstContactGen;

	

	private:
		bool calculateIterations;	
		particleForcePool forcePool;
		particleContactResolver resolver;
		std::vector<particleContactGen> contactGen;
		particleContact* contacts;
		unsigned maxContacts;
	public:
		particleWorld(unsigned maxContacts, unsigned iterations=0);
		void startFrame();
		unsigned generateContacts();
		void update(float dT);
		void runPhysics(float dT);
		particleForcePool& getForcePool();
};

#endif
