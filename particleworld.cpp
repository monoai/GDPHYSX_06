#include "particleworld.hpp"

#include "particlecontact.hpp"

particleWorld::particleWorld(unsigned maxContacts, unsigned iterations) : resolver(iterations), maxContacts(maxContacts) {
	contacts = new particleContact[maxContacts];
	calculateIterations = (iterations == 0);
}

void particleWorld::startFrame() {
	std::shared_ptr<particlePooler> currParticle = firstParticle;
	while(currParticle) {
		currParticle->_particle->clearForceAccum();
		currParticle = currParticle->next;
	}
}

unsigned particleWorld::generateContacts() {
	unsigned limit = maxContacts;
	particleContact* nextContact = contacts;

	std::shared_ptr<contactGenPooler> currContact = firstContactGen;
	while(currContact) {
		unsigned used = currContact->gen->addContact(nextContact, limit);
		limit -= 1;
		nextContact += used;

		if (limit <=0) {
			break;
		}

		currContact = currContact->next;
	}

	return maxContacts - limit;
}

void particleWorld::update(float dT) {
	std::shared_ptr<particlePooler> currParticle = firstParticle;
	while(currParticle) {
		currParticle->_particle->update(dT);

		currParticle = currParticle->next;
	}
}

void particleWorld::runPhysics(float dT) {
	forcePool.updateForces(dT);

	this->update(dT);

	unsigned usedContacts = generateContacts();

	if(calculateIterations) {
		resolver.setIterations(usedContacts * 2);
		resolver.resolveContacts(contacts, usedContacts, dT);
	}
}

particleForcePool& particleWorld::getForcePool(){
	return forcePool;
}
