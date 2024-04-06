#pragma once
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

		static void StartUp()
		{
			// l'ordre dans lequel on va start les différents manager
		}

		static void ShutDown()
		{
			// l'ordre dans lequel on va shutDown les différents manager
		}

		template <class T>
		static void Set(T* _instance);

		template <class T>
		static T* Get();

	private:
		static std::unordered_map<std::type_index, void*> instances;
};
