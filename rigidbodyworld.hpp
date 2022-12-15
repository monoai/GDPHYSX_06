#pragma once
#ifndef RIGID_WORLD_H
#define RIGID_WORLD_H

#include "rigidbody.hpp"
//#include "rigidbodycontact.hpp"

#include <vector>
#include <memory>

class rigidWorld {
	private:
		std::vector<std::shared_ptr<rigidBody>> rbodyPool;
		//particleForcePool forcePool;
		//std::vector<std::shared_ptr<rigidContactGen>> contactGenPool;
		//rigidContactResolver resolver = particleContactResolver(20);
		//std::vector<std::shared_ptr<rigidContact>> contactPool;
	public:
		void startFrame();
		void generateContacts();
		void getOverlaps();
		void generateRigidContacts(std::shared_ptr<rigidBody> a, std::shared_ptr<rigidBody> b);
		void update(float dT);
		void runPhysics(float dT);
		void draw(float dT);
		void checkLife();
		void addContact(std::shared_ptr<rigidBody> a, std::shared_ptr<rigidBody> b, float restitution, glm::vec3 vector);
		//particleForcePool& getForcePool();
		std::vector<std::shared_ptr<rigidBody>>& getRBodyPool();
		//std::vector<std::shared_ptr<rigidContactGen>>& getContactGenPool();


};

#endif
