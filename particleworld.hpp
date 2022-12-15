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
		std::vector<std::shared_ptr<particle>> particlePool;
		particleForcePool forcePool;
		std::vector<std::shared_ptr<particleContactGen>> contactGenPool;
		particleContactResolver resolver = particleContactResolver(20);
		std::vector<std::shared_ptr<particleContact>> contactPool;
	public:
		void startFrame();
		void generateContacts();
		void getOverlaps();
		void generateParticleContacts(std::shared_ptr<particle> a, std::shared_ptr<particle> b);
		void update(float dT);
		void runPhysics(float dT);
		void draw(float dT);
		void checkLife();
		void addContact(std::shared_ptr<particle> a, std::shared_ptr<particle> b, float restitution, glm::vec3 vector);
		particleForcePool& getForcePool();
		std::vector<std::shared_ptr<particle>>& getParticlePool();
		std::vector<std::shared_ptr<particleContactGen>>& getContactGenPool();

};

#endif
