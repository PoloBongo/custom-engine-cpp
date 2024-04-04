#pragma once

#include <vulkan/vulkan.hpp>

namespace lve
{
	class VulkanHooks {
	private:
		static VulkanHooks* singletonInstance;

		vk::Instance* instance;
		vk::PhysicalDevice* physicalDevice;
		vk::Device* device;
		vk::Queue* graphicsQueue;
		vk::CommandPool* commandPool;
		vk::RenderPass* renderPass;
		vk::PipelineLayout* pipelineLayout;
		vk::Pipeline* graphicsPipeline;
		vk::Buffer* vertexBuffer;
		vk::DeviceMemory* vertexBufferMemory;
		vk::Buffer* indexBuffer;
		vk::DeviceMemory* indexBufferMemory;
		vk::Buffer* uniformBuffer;
		vk::DeviceMemory* uniformBufferMemory;
		vk::DescriptorSetLayout* descriptorSetLayout;
		vk::DescriptorPool* descriptorPool;
		vk::DescriptorSet* descriptorSet;
		vk::CommandBuffer* commandBuffer;

	public:
		VulkanHooks();
		~VulkanHooks();

		// Getters
		vk::Instance* GetInstance() const { return instance; }
		vk::PhysicalDevice* GetPhysicalDevice() const { return physicalDevice; }
		vk::Device* GetDevice() const { return device; }
		vk::Queue* GetGraphicsQueue() const { return graphicsQueue; }
		vk::CommandPool* GetCommandPool() const { return commandPool; }
		vk::RenderPass* GetRenderPass() const { return renderPass; }
		vk::PipelineLayout* GetPipelineLayout() const { return pipelineLayout; }
		vk::Pipeline* GetGraphicsPipeline() const { return graphicsPipeline; }
		vk::Buffer* GetVertexBuffer() const { return vertexBuffer; }
		vk::DeviceMemory* GetVertexBufferMemory() const { return vertexBufferMemory; }
		vk::Buffer* GetIndexBuffer() const { return indexBuffer; }
		vk::DeviceMemory* GetIndexBufferMemory() const { return indexBufferMemory; }
		vk::Buffer* GetUniformBuffer() const { return uniformBuffer; }
		vk::DeviceMemory* GetUniformBufferMemory() const { return uniformBufferMemory; }
		vk::DescriptorSetLayout* GetDescriptorSetLayout() const { return descriptorSetLayout; }
		vk::DescriptorPool* GetDescriptorPool() const { return descriptorPool; }
		vk::DescriptorSet* GetDescriptorSet() const { return descriptorSet; }
		vk::CommandBuffer* GetCommandBuffer() const { return commandBuffer; }

		static VulkanHooks* GetSingletonInstance() { return singletonInstance; }

		// Setters
		void SetInstance(vk::Instance* _instance) { this->instance = _instance; }
		void SetPhysicalDevice(vk::PhysicalDevice* _physicalDevice) { this->physicalDevice = _physicalDevice; }
		void SetDevice(vk::Device* _device) { this->device = _device; }
		void SetGraphicsQueue(vk::Queue* _graphicsQueue) { this->graphicsQueue = _graphicsQueue; }
		void SetCommandPool(vk::CommandPool* _commandPool) { this->commandPool = _commandPool; }
		void SetRenderPass(vk::RenderPass* _renderPass) { this->renderPass = _renderPass; }
		void SetPipelineLayout(vk::PipelineLayout* _pipelineLayout) { this->pipelineLayout = _pipelineLayout; }
		void SetGraphicsPipeline(vk::Pipeline* _graphicsPipeline) { this->graphicsPipeline = _graphicsPipeline; }
		void SetVertexBuffer(vk::Buffer* _vertexBuffer) { this->vertexBuffer = _vertexBuffer; }
		void SetVertexBufferMemory(vk::DeviceMemory* _vertexBufferMemory) { this->vertexBufferMemory = _vertexBufferMemory; }
		void SetIndexBuffer(vk::Buffer* _indexBuffer) { this->indexBuffer = _indexBuffer; }
		void SetIndexBufferMemory(vk::DeviceMemory* _indexBufferMemory) { this->indexBufferMemory = _indexBufferMemory; }
		void SetUniformBuffer(vk::Buffer* _uniformBuffer) { this->uniformBuffer = _uniformBuffer; }
		void SetUniformBufferMemory(vk::DeviceMemory* _uniformBufferMemory) { this->uniformBufferMemory = _uniformBufferMemory; }
		void SetDescriptorSetLayout(vk::DescriptorSetLayout* _descriptorSetLayout) { this->descriptorSetLayout = _descriptorSetLayout; }
		void SetDescriptorPool(vk::DescriptorPool* _descriptorPool) { this->descriptorPool = _descriptorPool; }
		void SetDescriptorSet(vk::DescriptorSet* _descriptorSet) { this->descriptorSet = _descriptorSet; }
		void SetCommandBuffer(vk::CommandBuffer* _commandBuffer) { this->commandBuffer = _commandBuffer; }

		
		
	};
}
