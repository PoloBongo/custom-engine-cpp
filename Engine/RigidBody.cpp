#include "RigidBody.h"


RigidBody::RigidBody(btDiscreteDynamicsWorld* _dynamicsWorld, btCollisionShape* _shape, float _mass, glm::vec3& _position)
{
    btVector3  localInertia(0, 0, 0);
    _shape->calculateLocalInertia(_mass, localInertia);

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(Convertisseur::glmVec3ToBtVector3(_position));

    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(_mass, motionState, _shape, localInertia);
    rigidBody = new btRigidBody(rbInfo);

    dynamicsWorld->addRigidBody(rigidBody);
}

void RigidBody::Update(const float& _delta_time) {
    dynamicsWorld->stepSimulation(1 / 60.f);
}

void RigidBody::AddForce(glm::vec3& _force) {
    rigidBody->applyCentralForce(Convertisseur::glmVec3ToBtVector3(_force));
}

void RigidBody::SetVelocity(glm::vec3& _velocity) {
    rigidBody->setLinearVelocity(Convertisseur::glmVec3ToBtVector3(_velocity));
}

btVector3 RigidBody::GetVelocity() {
    return rigidBody->getLinearVelocity();
}

void RigidBody::SetGravity(glm::vec3& _gravity) {
    rigidBody->setGravity(Convertisseur::glmVec3ToBtVector3(_gravity));
}

void RigidBody::SetKinematic(bool _isKinematic) {
    if (_isKinematic) {
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        rigidBody->setActivationState(DISABLE_DEACTIVATION);
    }
    else {
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
    }
}

void RigidBody::SetRotationLocked(bool _lockRotationX, bool _lockRotationY, bool _lockRotationZ) {
    rigidBody->setAngularFactor(btVector3(_lockRotationX ? 0 : 1, _lockRotationY ? 0 : 1, _lockRotationZ ? 0 : 1));
}

void RigidBody::SetPosition(glm::vec3& _position, bool lockX, bool lockY, bool lockZ) {
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(Convertisseur::glmVec3ToBtVector3(_position));

    if (lockX) {
        transform.getBasis().setValue
            (0, 0, 0,
            0, 1, 0,
            0, 0, 1);
    }
    if (lockY) {
        transform.getBasis().setValue
            (1, 0, 0,
            0, 0, 0,  
            0, 0, 1); 
    }
    if (lockZ) {
        transform.getBasis().setValue
            (1, 0, 0,
            0, 1, 0,  
            0, 0, 0);
    }

    rigidBody->setWorldTransform(transform);
}



RigidBody::~RigidBody() {
    dynamicsWorld->removeRigidBody(rigidBody);
    delete rigidBody->getMotionState();
    delete rigidBody;
}
