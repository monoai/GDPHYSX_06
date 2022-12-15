#include "rigidbodyworld.hpp"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <algorithm>

void rigidWorld::startFrame() {
	for(long unsigned int i = 0; i < rbodyPool.size(); i++) {
		rbodyPool[i]->clearAccums();
		//rbodyPool[i]->calculateDerivedData();
	}
}

void rigidWorld::runPhysics(float dT) {
	for(long unsigned int i = 0; i < rbodyPool.size(); i++) {
		rbodyPool[i]->update(dT);
		//rbodyPool[i]->calculateDerivedData();
	}

	/*
	generateContacts();

	if(contactPool.size() > 0) {
		resolver.setIterations(contactPool.size() * 2);
		resolver.resolveContacts(contactPool, dT);
	}
	*/
}

void rigidWorld::draw(float dT) {
	for(long unsigned int i = 0; i < rbodyPool.size(); i++) {
		rbodyPool[i]->draw();
	}
}

std::vector<std::shared_ptr<rigidBody>>& rigidWorld::getRBodyPool(){
	return rbodyPool;
}

/*
std::vector<std::shared_ptr<rigidContactGen>>& rigidWorld::getContactGenPool(){
	return contactGenPool;
}
*/
