#pragma once

#include "lve_device.h"
#include "lve_buffer.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

//std
#include <memory>
#include <vector>
#include <string>

namespace lve {
class LveModel
{

public:

	struct Vertex {
		glm::vec3 position{};
		glm::vec3 color{};
		glm::vec3 normal{};
		glm::vec2 uv{};

		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

		bool operator==(const Vertex& _other) const {
			return position == _other.position && color == _other.color && normal == _other.normal && uv == _other.uv;
		}
	};

	struct Builder {
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};

		void LoadModel(const std::string& _filepath);
	};

	LveModel(LveDevice& _lveDevice, const LveModel::Builder &_builder);
	~LveModel();

	LveModel(const LveModel&) = delete;
	LveModel operator=(const LveModel&) = delete;

	static std::unique_ptr<LveModel> CreateModelFromFile(LveDevice& _device, const std::string& _filepath);

	void Bind(VkCommandBuffer _commandBuffer);
	void Draw(VkCommandBuffer _commandBuffer);

private:
	void CreateVertexBuffer(const std::vector<Vertex>& _vertices);

	void CreateIndexBuffer(const std::vector<uint32_t>& _indices);

	LveDevice& lveDevice;

	std::unique_ptr<LveBuffer> vertexBuffer;
	uint32_t vertexCount;

	bool hasIndexBuffer = false;
	std::unique_ptr<LveBuffer> indexBuffer;
	uint32_t indexCount;

};


} //lve
