#include "BoxCollider.h"

BoxCollider::BoxCollider(btDiscreteDynamicsWorld* _dynamicsWorld,glm::vec3 _center, glm::vec3 _position, btCollisionWorld* _collisionWorld) {
    collisionShape = new btBoxShape(Convertisseur::glmVec3ToBtVector3(_position));

    ghostObject = new btGhostObject();
    ghostObject->setCollisionShape(collisionShape);
    ghostObject->setCollisionFlags(ghostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(Convertisseur::glmVec3ToBtVector3(_center));
    ghostObject->setWorldTransform(transform);

    _collisionWorld->addCollisionObject(ghostObject);
    _dynamicsWorld->addRigidBody(rigidBody);
}

void BoxCollider::SetSize(glm::vec3 _size) {
    delete collisionShape;
    collisionShape = new btBoxShape((Convertisseur::glmVec3ToBtVector3(_size)));
    ghostObject->setCollisionShape(collisionShape);
}

void BoxCollider::SetCenter(glm::vec3 _center) {
    btTransform transform = ghostObject->getWorldTransform();
    transform.setOrigin(Convertisseur::glmVec3ToBtVector3(_center));
    ghostObject->setWorldTransform(transform);
}

BoxCollider::~BoxCollider() {
    delete ghostObject;
    delete collisionShape;
}
