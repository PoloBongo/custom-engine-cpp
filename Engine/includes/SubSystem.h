#pragma once
#include "Modules/CollisionModule.h"
#include <typeindex>
#include <unordered_map>

class SubSystem
{
	public:
		SubSystem()
		{
			// do nothing
		}

		~SubSystem()
		{
			// do nothing
		}

		void startUp()
		{
			// l'ordre dans lequel on va start les différents manager
		}

		void shutDown()
		{
			// l'ordre dans lequel on va shutDown les différents manager
		}

		template <class T>
		static void Set(T* instance);

		template <class T>
		static T* Get();

	private:
		static std::unordered_map<std::type_index, void*> instances;
};
