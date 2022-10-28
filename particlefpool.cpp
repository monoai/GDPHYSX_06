#include "particlefpool.hpp"

void particleForcePool::add(particle* _particle, particleForceGen *_fg) {
	particleForcePooler newPool;
	newPool._particle = _particle;
	newPool._fg = _fg;
	
	this->pool.push_back(newPool);
}

void particleForcePool::updateForces(float dT) {
	std::vector<particleForcePooler>::iterator i = pool.begin();
	for(; i != pool.end(); ++i) {
		i->_fg->updateForce(i->_particle, dT);
	}
}
