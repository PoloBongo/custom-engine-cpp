#include "lve_model.h"

#include <cassert>
#include <cstring>

namespace lve {
	/*struct Vertex {
		glm::vec2 position;

		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescription();
	};*/

	LveModel::LveModel(LveDevice& _lveDevice, const std::vector<Vertex>& _vertices) : lveDevice(_lveDevice){
		CreateVertexBuffer(_vertices);
	}
	LveModel::~LveModel() {
		vkDestroyBuffer(lveDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(lveDevice.device(), vertexBufferMemory, nullptr);
	}


	void LveModel::Bind(VkCommandBuffer _commandBuffer){
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(_commandBuffer, 0, 1, buffers, offsets);
	}
	void LveModel::Draw(VkCommandBuffer _commandBuffer){
		vkCmdDraw(_commandBuffer, vertexCount, 1, 0, 0);
	}

	void LveModel::CreateVertexBuffer(const std::vector<Vertex>& _vertices) {
		vertexCount = static_cast<uint32_t>(_vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(_vertices[0]) * vertexCount;

		lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory
		);

		void* data;
		vkMapMemory(lveDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, _vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(lveDevice.device(), vertexBufferMemory);
	}

	std::vector<VkVertexInputBindingDescription> LveModel::Vertex::GetBindingDescriptions() {
		std::vector< VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride =sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	 }
	std::vector<VkVertexInputAttributeDescription> LveModel::Vertex::GetAttributeDescriptions() {
		std::vector< VkVertexInputAttributeDescription> attributeDescriptions(2);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex,color);
		return attributeDescriptions;
	 }
} //namespace lve