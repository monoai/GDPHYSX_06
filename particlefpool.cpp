#include "particlefpool.hpp"

void particleForcePool::add(particle* particle, particleForceGen *fg) {
	particleForcePooler newPool;
	newPool.particle = particle;
	newPool.fg = fg;
	
	this->pool.push_back(newPool);
}

void particleForcePool::updateForces(float dT) {
	for(auto& i : pool) {
		i->fg->updateForce(i->particle, dT);
	}
}
