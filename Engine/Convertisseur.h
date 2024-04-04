#pragma once
#include <glm.hpp>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
class Convertisseur
{
public:

	static btVector3 glmVec3ToBtVector3(const glm::vec3& glmVec) 
    {
        return btVector3(glmVec.x, glmVec.y, glmVec.z);
    }
};

