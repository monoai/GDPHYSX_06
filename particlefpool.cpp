#include "particlefpool.hpp"

#include "obj_mesh.h"

void particleForcePool::add(particle *_particle, particleForceGen *_fg) {
	particleForcePooler newPool;
	newPool._particle = _particle;
	newPool._fg = _fg;
	
	this->pool.push_back(newPool);
	std::cout << "pool pushed" << std::endl;
}

void particleForcePool::updateForces(float dT) {
	//std::vector<particleForcePooler>::iterator i = pool.begin();
	for(int i = 0; i < this->getSize(); i++) {
		std::cout << "curr iteration updF: " << i << std::endl;
		this->pool[i]._fg->updateForce(this->pool[i]._particle, dT);
	}
}

int particleForcePool::getSize() {
	return pool.size();
}

void particleForcePool::update(float dT) {
	//std::vector<particleForcePooler>::iterator i = pool.begin();
	for(int i = 0; i < this->getSize(); i++) {
		std::cout << "curr iteration upd: " << i << std::endl;
		//std::cout << "dT upd: " << dT << std::endl;
		this->pool[i]._particle->update(dT);
	}
}

void particleForcePool::draw(ObjData obj) {
	//std::vector<particleForcePooler>::iterator i = pool.begin();
	for(int i = 0; i < this->getSize(); i++) {
		std::cout << "curr iteration draw: " << i << std::endl;
		this->pool[i]._particle->draw(obj);
	}
}
