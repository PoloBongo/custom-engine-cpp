#include "Systems/point_light_system.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>
#include <vulkan/vulkan.hpp>

// std
#include <array>
#include <cassert>
#include <map>
#include <stdexcept>

#include "Modules/TimeModule.h"

namespace lve
{
	PointLightSystem::PointLightSystem(LveDevice&              _device, vk::RenderPass _renderPass,
	                                   vk::DescriptorSetLayout _globalSetLayout) : lveDevice(_device)
	{
		CreatePipelineLayout(_globalSetLayout);
		CreatePipeline(_renderPass);
	}

	PointLightSystem::~PointLightSystem() { lveDevice.Device().destroyPipelineLayout(pipelineLayout, nullptr); }

	void PointLightSystem::CreatePipelineLayout(vk::DescriptorSetLayout _globalSetLayout)
	{
		vk::PushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
		pushConstantRange.offset     = 0;
		pushConstantRange.size       = sizeof(PointLightPushConstants);

		std::vector<vk::DescriptorSetLayout> descriptorSetLayouts{_globalSetLayout};

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType                  = vk::StructureType::ePipelineLayoutCreateInfo;
		pipelineLayoutInfo.setLayoutCount         = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts            = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges    = &pushConstantRange;
		if (lveDevice.Device().createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout) !=
		    vk::Result::eSuccess)
			throw std::runtime_error("failed to create pipeline layout!");
	}

	void PointLightSystem::CreatePipeline(vk::RenderPass _renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		LvePipeline::DefaultPipelineConfigInfo(pipelineConfig);
		LvePipeline::EnableAlphaBlending(pipelineConfig);
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.renderPass     = _renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline                   = std::make_unique<LvePipeline>(
			lveDevice,
			"Shaders/point_light.vert.spv",
			"Shaders/point_light.frag.spv",
			pipelineConfig);
	}

	void PointLightSystem::Update(LveGameObject::Map& _gameObjects, GlobalUbo& _ubo)
	{
		auto rotateLight = rotate(glm::mat4(1.f), 0.5f * TimeModule::GetDeltaTime(), {0.f, -1.f, 0.f});
		int  lightIndex  = 0;
		for (auto& kv : _gameObjects)
		{
			auto& obj = kv.second;
			if (obj.pointLight == nullptr) continue;

			assert(lightIndex < MAX_LIGHTS && "Point lights exceed maximum specified");

			// update light position
			obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.f));

			// copy light to ubo
			_ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.f);
			_ubo.pointLights[lightIndex].color    = glm::vec4(obj.color, obj.pointLight->lightIntensity);

			lightIndex += 1;
		}
		_ubo.numLights = lightIndex;
	}

	void PointLightSystem::Render(FrameInfo& _frameInfo)
	{
		// Tri des lumières
		std::map<float, LveGameObject::id_t> sorted;
		for (auto& kv : _frameInfo.gameObjects)
		{
			auto& obj = kv.second;
			if (obj.pointLight == nullptr) continue;

			// Calcul de la distance 
			auto  offset            = _frameInfo.camera.GetPosition() - obj.transform.translation;
			float distanceSquared   = dot(offset, offset);
			sorted[distanceSquared] = obj.GetId();
		}
		lvePipeline->Bind(_frameInfo.commandBuffer);

		// Liaison de l'ensemble de descripteurs global
		_frameInfo.commandBuffer.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			pipelineLayout,
			0,
			_frameInfo.globalDescriptorSet,
			nullptr);

		// Itération à travers les lumières triées
		for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			// Utilisation de l'ID de l'objet pour trouver l'objet lumière
			auto& obj = _frameInfo.gameObjects.at(it->second);

			PointLightPushConstants push{};
			push.position = glm::vec4(obj.transform.translation, 1.f);
			push.color    = glm::vec4(obj.color, obj.pointLight->lightIntensity);
			push.radius   = obj.transform.scale.x;

			// Mise à jour des push constants
			_frameInfo.commandBuffer.pushConstants<PointLightPushConstants>(
				pipelineLayout,
				vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
				0,
				push);

			// Dessin d'une primitive pour représenter la lumière
			_frameInfo.commandBuffer.draw(6, 1, 0, 0);
		}
	}
} // namespace lve
