#include "Systems/simple_render_system.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace lve {

    SimpleRenderSystem::SimpleRenderSystem(LveDevice& _device, VkRenderPass _renderPass, VkDescriptorSetLayout _globalSetLayout) : lveDevice(_device) {
        CreatePipelineLayout(_globalSetLayout);
        CreatePipeline(_renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() { vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr); }

    void SimpleRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout _globalSetLayout) {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ _globalSetLayout };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }


    void SimpleRenderSystem::CreatePipeline(VkRenderPass _renderPass) {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::DefaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = _renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "Shaders/simple_shader.vert.spv",
            "Shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void SimpleRenderSystem::RenderGameObjects(FrameInfo& _frameInfo) {
        // Liaison du pipeline
        lvePipeline->Bind(_frameInfo.commandBuffer);

        // Liaison de l'ensemble de descripteurs global
        _frameInfo.commandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            pipelineLayout,
            0,
            _frameInfo.globalDescriptorSet,
            nullptr);

        for (auto& kv : _frameInfo.gameObjects) {
            auto& obj = kv.second;
            if (obj.model == nullptr) continue;
            SimplePushConstantData push{};
            push.modelMatrix = obj.transform.mat4();
            push.normalMatrix = obj.transform.normalMatrix();

            // Mise à jour des push constants
            _frameInfo.commandBuffer.pushConstants<SimplePushConstantData>(
                pipelineLayout,
                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                0,
                push);

            // Liaison du modèle et dessin
            obj.model->Bind(_frameInfo.commandBuffer);
            obj.model->Draw(_frameInfo.commandBuffer);
        }
    }


}  // namespace lve