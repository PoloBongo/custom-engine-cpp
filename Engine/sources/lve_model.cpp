#include "lve_model.h"

#include "lve_utils.h"

//lives
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>

//std
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <unordered_map>

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif


namespace std {
	template<>
	struct hash<lve::LveModel::Vertex> {
		size_t operator()(lve::LveModel::Vertex const& _vertex) const {
			size_t seed = 0;
			lve::HashCombine(seed, _vertex.position, _vertex.color, _vertex.normal, _vertex.uv);
			return seed;
		}
	};
}

namespace lve {
	/*struct Vertex {
		glm::vec2 position;

		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescription();
	};*/

	LveModel::LveModel(LveDevice& _lveDevice, const LveModel::Builder& _builder) : lveDevice(_lveDevice){
		CreateVertexBuffer(_builder.vertices);
		CreateIndexBuffer(_builder.indices);
	}

	LveModel::~LveModel() {}


	std::unique_ptr<LveModel> LveModel::CreateModelFromFile(LveDevice& _device, const std::string& _filepath){
		Builder builder{};
		builder.LoadModel(ENGINE_DIR + _filepath);
		std::cout << "Vertex Count" << builder.vertices.size() << std::endl;

		return std::make_unique<LveModel>(_device, builder);
	}


	void LveModel::Bind(vk::CommandBuffer _commandBuffer) {
		vk::Buffer buffers[] = { vertexBuffer->getBuffer() };
		vk::DeviceSize offsets[] = { 0 };
		_commandBuffer.bindVertexBuffers(0, 1, buffers, offsets);

		if (hasIndexBuffer) {
			_commandBuffer.bindIndexBuffer(indexBuffer->getBuffer(), 0, vk::IndexType::eUint32);
		}
	}


	void LveModel::Draw(vk::CommandBuffer _commandBuffer) {
		if (hasIndexBuffer) {
			_commandBuffer.drawIndexed(indexCount, 1, 0, 0, 0);
		}
		else {
			_commandBuffer.draw(vertexCount, 1, 0, 0);
		}
	}


	void LveModel::CreateVertexBuffer(const std::vector<Vertex>& _vertices) {
		vertexCount = static_cast<uint32_t>(_vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		vk::DeviceSize bufferSize = sizeof(_vertices[0]) * vertexCount;
		uint32_t vertexSize = sizeof(_vertices[0]);

		LveBuffer stagingBuffer{
			lveDevice,
			vertexSize,
			vertexCount,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)(_vertices.data()));

		vertexBuffer = std::make_unique<LveBuffer>(
			lveDevice,
			vertexSize,
			vertexCount,
			vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);

		lveDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
	}


	void LveModel::CreateIndexBuffer(const std::vector<uint32_t>& _indices) {
		// Détermine le nombre d'indices et vérifie s'il y en a au moins un
		indexCount = static_cast<uint32_t>(_indices.size());
		hasIndexBuffer = indexCount > 0;

		// Si aucun indice n'est présent, la fonction se termine
		if (!hasIndexBuffer) {
			return;
		}

		// Calcul de la taille du tampon en bytes
		vk::DeviceSize bufferSize = sizeof(_indices[0]) * indexCount;
		uint32_t indexSize = sizeof(_indices[0]);

		// Création du tampon de transfert pour les données d'indices
		LveBuffer stagingBuffer{
			lveDevice,
			indexSize,
			indexCount,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		};

		// Mappage du tampon de transfert et écriture des données
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)(_indices.data()));

		// Création du tampon d'indices sur le GPU
		indexBuffer = std::make_unique<LveBuffer>(
			lveDevice,
			indexSize,
			indexCount,
			vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);

		// Copie des données de l'indice du tampon de transfert vers le tampon d'indices du GPU
		lveDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
	}


	std::vector<vk::VertexInputBindingDescription> LveModel::Vertex::GetBindingDescriptions() {
		std::vector<vk::VertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].setBinding(0);
		bindingDescriptions[0].setStride(sizeof(Vertex));
		bindingDescriptions[0].setInputRate(vk::VertexInputRate::eVertex);
		return bindingDescriptions;
	}

	std::vector<vk::VertexInputAttributeDescription> LveModel::Vertex::GetAttributeDescriptions() {
		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

		attributeDescriptions.push_back({ 0, 0, vk::Format::eR32G32B32Sfloat, static_cast<uint32_t>(offsetof(Vertex, position)) });
		attributeDescriptions.push_back({ 1, 0, vk::Format::eR32G32B32Sfloat, static_cast<uint32_t>(offsetof(Vertex, color)) });
		attributeDescriptions.push_back({ 2, 0, vk::Format::eR32G32B32Sfloat, static_cast<uint32_t>(offsetof(Vertex, normal)) });
		attributeDescriptions.push_back({ 3, 0, vk::Format::eR32G32Sfloat, static_cast<uint32_t>(offsetof(Vertex, uv)) });

		return attributeDescriptions;
	}

	void LveModel::Builder::LoadModel(const std::string& _filepath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, _filepath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};
				}

				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}


} //namespace lve