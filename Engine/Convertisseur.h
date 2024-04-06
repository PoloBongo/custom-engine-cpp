#pragma once
#include <glm.hpp>
#include <BulletCollision/CollisionShapes/btBoxShape.h>

class Convertisseur
{
	public:
		static btVector3 GlmVec3ToBtVector3(const glm::vec3& _glmVec)
		{
			return btVector3(_glmVec.x, _glmVec.y, _glmVec.z);
		}
};
