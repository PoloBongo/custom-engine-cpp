#include "VulkanHooks.h"

namespace lve
{
	VulkanHooks* VulkanHooks::singletonInstance = nullptr;

VulkanHooks::VulkanHooks() {
	if (singletonInstance == nullptr) {
		singletonInstance = this;
	}
}

VulkanHooks::~VulkanHooks() {
	if (singletonInstance == this) {
		singletonInstance = nullptr;
	}
}
}
