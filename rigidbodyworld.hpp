#pragma once
#ifndef RIGID_WORLD_H
#define RIGID_WORLD_H

#include "rigidbody.hpp"
#include "rigidbodycontact.hpp"

#include <vector>
#include <memory>

class rigidBodyWorld {
	private:
		std::vector<std::shared_ptr<rigidBody>> rbodyPool;
		//particleForcePool forcePool;
		std::vector<std::shared_ptr<rigidBodyContactGen>> contactGenPool;
		rigidBodyContactResolver resolver = rigidBodyContactResolver(20);
		std::vector<std::shared_ptr<rigidBodyContact>> contactPool;
	public:
		void startFrame();
		void generateContacts();
		void getOverlaps();
		void generateRigidContactsCircle(std::shared_ptr<rigidBody> a, std::shared_ptr<rigidBody> b);
		void generateRigidContactsMixed(std::shared_ptr<rigidBody> a, std::shared_ptr<rigidBody> b);

		void update(float dT);
		void runPhysics(float dT);
		void draw(float dT);
		void checkLife();
		void addContact(std::shared_ptr<rigidBody> a, std::shared_ptr<rigidBody> b, float restitution, glm::vec3 vector);
		//particleForcePool& getForcePool();
		std::vector<std::shared_ptr<rigidBody>>& getRBodyPool();
		std::vector<std::shared_ptr<rigidBodyContactGen>>& getContactGenPool();


};

#endif
