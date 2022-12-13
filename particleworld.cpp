#include "particleworld.hpp"

#include "particlecontact.hpp"
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <algorithm>

void particleWorld::startFrame() {
	for(long unsigned int i = 0; i < particlePool.size(); i++) {
		particlePool[i]->clearForceAccum();
	}
}

void particleWorld::addContact(std::shared_ptr<particle> a, std::shared_ptr<particle> b, float restitution, glm::vec3 normal) {
	
	std::shared_ptr<particleContact> contact(new particleContact());
	//std::cout << "[DEBUG] - NameAadd: " << a << std::endl;
	//std::cout << "[DEBUG] - NameBadd: " << b << std::endl;
	contact->_particle[0] = a;
	contact->_particle[1] = b;
	contact->restitution = restitution;
	contact->contactNormal = normal;
	//contact->penetration = a->radius - normal;

	contactPool.push_back(contact);
	

}

void particleWorld::generateContacts() {
	contactPool.clear();
	getOverlaps();
	//std::cout << "[DEBUG] - ContactPoolCountBefore: " << contactPool.size() << std::endl;

	for(long unsigned int i = 0; i < contactGenPool.size(); i++) {
		//std::cout << "[DEBUG] - ContactGenName: " << contactGenPool[i] << std::endl;
		//std::cout << "[DEBUG] - ContactPoolCountAfter: " << contactPool.size() << std::endl;
		std::shared_ptr<particleContact> newContact = contactGenPool[i]->addContact();

		if(newContact != nullptr) {
			contactPool.push_back(newContact);
		}
	}
}

void particleWorld::getOverlaps() {
	for (long unsigned int i = 0; i < particlePool.size(); i++) {
		for (long unsigned int p = i + 1; p < particlePool.size(); p++) {
			generateParticleContacts(particlePool[i], particlePool[p]);
		}
	}
}

void particleWorld::generateParticleContacts(std::shared_ptr<particle> a, std::shared_ptr<particle> b) {
	float mag = glm::length2(a->getPosition() - b->getPosition());
	float rad = (a->radius + b->radius) * (a->radius + b->radius);

	if (mag <= rad) {
		//float r = rad - mag;
		//float depth = sqrt(r);

		float restitution = a->restitution;
		if (b->restitution < restitution) {
			restitution = b->restitution;
		}
		//std::cout << "[DEBUG] - NameApreadd: " << a << std::endl;
		//std::cout << "[DEBUG] - NameBpreadd: " << b << std::endl;
		addContact(a,b, restitution,glm::normalize(a->getPosition() - b->getPosition()));
	}
}

void particleWorld::update(float dT) {
	this->checkLife();

	for(long unsigned int i = 0; i < particlePool.size(); i++) {
		particlePool[i]->update(dT);
		//std::cout << "[DEBUG] - ParticlePosition: " << glm::to_string(particlePool[i]->getPosition()) << std::endl; 
		//std::cout << "[DEBUG] - ParticleVelocity: " << glm::to_string(particlePool[i]->getVelocity()) << std::endl;
	}
}

void particleWorld::runPhysics(float dT) {
	forcePool.updateForces(dT);

	this->update(dT);
	
	generateContacts();

	if(contactPool.size() > 0) {
		resolver.setIterations(contactPool.size() * 2);
		resolver.resolveContacts(contactPool, dT);
	}
	
}

void particleWorld::draw(float dT) {
	for(long unsigned int i = 0; i < particlePool.size(); i++) {
		particlePool[i]->draw();
	}
}

void particleWorld::checkLife() {
	for(long unsigned int i = 0; i < particlePool.size(); i++) {
		if(particlePool[i]->isDestroyed() == true){
			particlePool.erase(particlePool.begin()+i);
		}
	}

	/* Alternative way of erasing, doesn't work because requires another function it seems.
	particlePool.erase(
		std::remove_if(particlePool.begin(), particlePool.end(), isDestroyed),
		particlePool.end()
	);
	*/
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