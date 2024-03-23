#pragma once

#include "lve_device.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

//std
#include <vector>

namespace lve {
class LveModel
{

public:

	struct Vertex {
		glm::vec3 position;
		glm::vec3 color;

		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
	};

	LveModel(LveDevice& _lveDevice, const std::vector<Vertex>& _vertices);
	~LveModel();

	LveModel(const LveModel&) = delete;
	LveModel operator=(const LveModel&) = delete;

	void Bind(VkCommandBuffer _commandBuffer);
	void Draw(VkCommandBuffer _commandBuffer);

private:
	void CreateVertexBuffer(const std::vector<Vertex>& _vertices);

	LveDevice& lveDevice;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	uint32_t vertexCount;

};


} //lve
