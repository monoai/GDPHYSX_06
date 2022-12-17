#include "rigidbodyworld.hpp"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <algorithm>

void rigidBodyWorld::startFrame() {
	for(long unsigned int i = 0; i < rbodyPool.size(); i++) {
		rbodyPool[i]->clearAccums();
		//rbodyPool[i]->calculateDerivedData();
	}
}

void rigidBodyWorld::addContact(std::shared_ptr<rigidBody> a, std::shared_ptr<rigidBody> b, float restitution, glm::vec3 normal) {
	
	std::shared_ptr<rigidBodyContact> contact(new rigidBodyContact());
	//std::cout << "[DEBUG] - NameAadd: " << a << std::endl;
	//std::cout << "[DEBUG] - NameBadd: " << b << std::endl;
	contact->_rbody[0] = a;
	contact->_rbody[1] = b;
	contact->restitution = restitution;
	contact->contactNormal = normal;
	//contact->penetration = a->radius - normal;

	contactPool.push_back(contact);
	

}

void rigidBodyWorld::generateContacts() {
	contactPool.clear();
	getOverlaps();
	//std::cout << "[DEBUG] - ContactPoolCountBefore: " << contactPool.size() << std::endl;

	for(long unsigned int i = 0; i < contactGenPool.size(); i++) {
		//std::cout << "[DEBUG] - ContactGenName: " << contactGenPool[i] << std::endl;
		//std::cout << "[DEBUG] - ContactPoolCountAfter: " << contactPool.size() << std::endl;
		std::shared_ptr<rigidBodyContact> newContact = contactGenPool[i]->addContact();

		if(newContact != nullptr) {
			contactPool.push_back(newContact);
		}
	}
}

void rigidBodyWorld::getOverlaps() {
	for (long unsigned int i = 0; i < rbodyPool.size(); i++) {
		for (long unsigned int p = i + 1; p < rbodyPool.size(); p++) {
			if(rbodyPool[i]->getType() == 0 && rbodyPool[p]->getType() == 0) { // If both pool is circle
				generateRigidContactsCircle(rbodyPool[i], rbodyPool[p]);
			} else if(rbodyPool[i]->getType() == 0) { // If pool i is circle
				generateRigidContactsMixed(rbodyPool[i], rbodyPool[p]);
			} else { // Assume pool p is the circle
				generateRigidContactsMixed(rbodyPool[p], rbodyPool[i]);
			}
		}
	}
}

void rigidBodyWorld::generateRigidContactsCircle(std::shared_ptr<rigidBody> a, std::shared_ptr<rigidBody> b) {
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

void rigidBodyWorld::generateRigidContactsMixed(std::shared_ptr<rigidBody> a, std::shared_ptr<rigidBody> b) {
	glm::vec3 distance = b->getPosition() - a->getPosition();

	float mag = glm::length2(a->getPosition() - b->getPosition());
	float rad = (a->radius + b->radius) * (a->radius + b->radius);

	if (mag <= rad) {
		float restitution = a->restitution;
		if (b->restitution < restitution) {
			restitution = b->restitution;
		}
		//std::cout << "[DEBUG] - NameApreadd: " << a << std::endl;
		//std::cout << "[DEBUG] - NameBpreadd: " << b << std::endl;
		addContact(a,b, restitution,glm::normalize(a->getPosition() - b->getPosition()));
		a->addForceAtPoint(glm::vec3(400.0f, 0.0f, 0.0f), glm::vec3(5000.0f,250.0f,0.0f));
		b->addForceAtPoint(glm::vec3(400.0f, 0.0f, 0.0f), glm::vec3(5000.0f,250.0f,0.0f));
	}
}

void rigidBodyWorld::runPhysics(float dT) {
	for(long unsigned int i = 0; i < rbodyPool.size(); i++) {
		rbodyPool[i]->update(dT);
		//rbodyPool[i]->calculateDerivedData();
	}

	generateContacts();

	if(contactPool.size() > 0) {
		resolver.setIterations(contactPool.size() * 2);
		resolver.resolveContacts(contactPool, dT);
	}
}

void rigidBodyWorld::draw(float dT) {
	for(long unsigned int i = 0; i < rbodyPool.size(); i++) {
		rbodyPool[i]->draw();
	}
}

std::vector<std::shared_ptr<rigidBody>>& rigidBodyWorld::getRBodyPool(){
	return rbodyPool;
}

std::vector<std::shared_ptr<rigidBodyContactGen>>& rigidBodyWorld::getContactGenPool(){
	return contactGenPool;
}
