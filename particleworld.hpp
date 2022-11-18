#pragma once
#ifndef PARTICLE_WORLD_H
#define PARTICLE_WORLD_H

#include "particle.hpp"
#include "particlefpool.hpp"
#include "particlecontact.hpp"

#include <vector>
#include <memory>

class particleWorld {
	private:
		bool calculateIterations;
		std::vector<std::shared_ptr<particle>> particlePool;
		particleForcePool forcePool;
		std::vector<std::shared_ptr<particleContactGen>> contactGenPool;
		particleContactResolver resolver = particleContactResolver(0);
		std::vector<std::shared_ptr<particleContact>> contactPool;
	public:
		void startFrame();
		void generateContacts();
		void update(float dT);
		void runPhysics(float dT);
		void draw(float dT);
		particleForcePool& getForcePool();
		std::vector<std::shared_ptr<particle>>& getParticlePool();
		std::vector<std::shared_ptr<particleContactGen>>& getContactGenPool();

};

#endif
