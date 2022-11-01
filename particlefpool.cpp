#include "particlefpool.hpp"

#include "obj_mesh.h"

void particleForcePool::add(std::shared_ptr<particle>_particle, std::shared_ptr<particleForceGen>_fg) {
	particleForcePooler newPool;
	newPool._particle = _particle;
	newPool._fg = _fg;
	
	this->pool.push_back(newPool);
	//std::cout << "pool pushed" << std::endl;
}

void particleForcePool::remove(std::shared_ptr<particle>_particle, std::shared_ptr<particleForceGen>_fg) {
	for(int i = 0; i < this->getSize(); i++) {
		if(this->pool[i]._particle == _particle) {
			this->pool.erase(pool.begin()+i);
			return;
		}
	}
}

void particleForcePool::updateForces(float dT) {
	for(int i = 0; i < this->getSize(); i++) {
		//std::cout << "curr iteration updF: " << i << std::endl;
		this->pool[i]._fg->updateForce(this->pool[i]._particle, dT);
		this->pool[i]._particle->update(dT);
	}
}

void particleForcePool::update(float dT) {
	for(int i = 0; i < this->getSize(); i++) {
		//std::cout << "curr iteration upd: " << i << std::endl;
		this->pool[i]._particle->update(dT);
	}
}

void particleForcePool::draw() {
	for(int i = 0; i < this->getSize(); i++) {
		//std::cout << "curr iteration draw: " << i << std::endl;
		this->pool[i]._particle->draw();
	}
}

int particleForcePool::getSize() {
	return pool.size();
}

void particleForcePool::checkLife() {
	for(int i = 0; i < this->getSize(); i++) {
		if(this->pool[i]._particle->inUse == false) {
			this->remove(this->pool[i]._particle, this->pool[i]._fg);
		}
	}
}

void particleForcePool::getContents() {
	for(int i = 0; i < this->getSize(); i++) {
		std::cout << "Pool [" << i << "] Particle - " << this->pool[i]._particle << std::endl;
		std::cout << "Pool [" << i << "] ForceGen - " << this->pool[i]._fg << std::endl;
	}
}
