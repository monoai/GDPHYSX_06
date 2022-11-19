#include "particleworld.hpp"

#include "particlecontact.hpp"

void particleWorld::startFrame() {
	for(long unsigned int i = 0; i < particlePool.size(); i++) {
		particlePool[i]->clearForceAccum();
	}
}

void particleWorld::addContact(std::shared_ptr<particle> a, std::shared_ptr<particle> b, float restitution, glm::vec3 normal) {
	
	std::shared_ptr<particleContact> contact;

	contact->_particle[0] = a;
	contact->_particle[1] = b;
	contact->restitution = restitution;
	contact->contactNormal = normal;

	contactPool.push_back(contact);
	

}

void particleWorld::generateContacts() {
	unsigned limit = contactPool.size();
	contactPool.clear();
	getOverlaps();

	for(long unsigned int i = 0; i < contactGenPool.size(); i++) {
		contactGenPool[i]->addContact(contactPool[i], limit);
		limit -= 1;

		if(limit <=0) {
			break;
		}
	}
}

void particleWorld::getOverlaps() {
	for (int i = 0; i < particlePool.size(); i++) {
		for (int p = i +1 ; p < particlePool.size(); p++) {
			generateParticleContacts(particlePool[i], particlePool[p]);
		}
	}
}

void particleWorld::generateParticleContacts(std::shared_ptr<particle> a, std::shared_ptr<particle> b) {
	float mag = glm::length(a->getPosition() - b->getPosition());
	float rad = (a->radius + b->radius) * (a->radius + b->radius);

	if (mag <= rad) {
		float r = rad - mag;
		float depth = sqrt(r);

		float restitution = a->restitution;
		if (b->restitution < restitution) {
			restitution = b->restitution;
		}
		addContact(a,b, restitution,glm::normalize(a->getPosition() - b->getPosition()));
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
	
	
	generateContacts();

	if(contactPool.size()) {
		resolver.setIterations(contactPool.size() * 2);
		resolver.resolveContacts(contactPool, contactPool.size(), dT);
	}
	
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
