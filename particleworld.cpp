#include "particleworld.hpp"

#include "particlecontact.hpp"

void particleWorld::startFrame() {
	for(long unsigned int i = 0; i < particlePool.size(); i++) {
		particlePool[i]->clearForceAccum();
	}
}

void particleWorld::generateContacts() {
	unsigned limit = contactPool.size();

	for(long unsigned int i = 0; i < contactGenPool.size(); i++) {
		contactGenPool[i]->addContact(contactPool[i], limit);
		limit -= 1;

		if(limit <=0) {
			break;
		}
	}
}

void particleWorld::update(float dT) {
	for(long unsigned int i = 0; i < particlePool.size(); i++) {
		particlePool[i]->update(dT);
	}
}

void particleWorld::runPhysics(float dT) {
	forcePool.updateForces(dT);

	this->update(dT);
	
	/*
	generateContacts();

	if(contactPool.size()) {
		resolver.setIterations(contactPool.size() * 2);
		resolver.resolveContacts(contactPool, contactPool.size(), dT);
	}
	*/
}

void particleWorld::draw(float dT) {
	for(long unsigned int i = 0; i < particlePool.size(); i++) {
		particlePool[i]->draw();
	}
}

particleForcePool& particleWorld::getForcePool(){
	return forcePool;
}

std::vector<std::shared_ptr<particle>>& particleWorld::getParticlePool(){
	return particlePool;
}

std::vector<std::shared_ptr<particleContactGen>>& particleWorld::getContactGenPool(){
	return contactGenPool;
}
