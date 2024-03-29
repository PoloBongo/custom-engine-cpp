#include "lve_pipeline.h"

#include "lve_model.h"

// std
#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif

namespace lve {

    LvePipeline::LvePipeline(
        LveDevice& device,
        const std::string& vertFilepath,
        const std::string& fragFilepath,
        const PipelineConfigInfo& configInfo)
        : lveDevice{ device } {
        CreateGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
    }

    LvePipeline::~LvePipeline() {
        lveDevice.device().destroyShaderModule(vertShaderModule);
        lveDevice.device().destroyShaderModule(fragShaderModule);
        lveDevice.device().destroyPipeline(graphicsPipeline);
    }

    std::vector<char> LvePipeline::ReadFile(const std::string& _filepath) {
        std::string enginePath = ENGINE_DIR + _filepath;
        std::ifstream file{ enginePath, std::ios::ate | std::ios::binary };

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + _filepath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

    void LvePipeline::CreateGraphicsPipeline(
        const std::string& _vertFilepath,
        const std::string& _fragFilepath,
        const PipelineConfigInfo& _configInfo) {
        assert(
            _configInfo.pipelineLayout != nullptr &&
            "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
        assert(
            _configInfo.renderPass != nullptr &&
            "Cannot create graphics pipeline: no renderPass provided in configInfo");

        auto vertCode = ReadFile(_vertFilepath);
        auto fragCode = ReadFile(_fragFilepath);

        CreateShaderModule(vertCode, &vertShaderModule);
        CreateShaderModule(fragCode, &fragShaderModule);

        vk::PipelineShaderStageCreateInfo shaderStages[2];
        shaderStages[0].sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        shaderStages[0].setStage(vk::ShaderStageFlagBits::eVertex);
        shaderStages[0].setModule(vertShaderModule);
        shaderStages[0].setPName("main");
        shaderStages[0].flags = vk::PipelineShaderStageCreateFlags(0);
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;
        shaderStages[1].sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        shaderStages[1].setStage(vk::ShaderStageFlagBits::eFragment);
        shaderStages[1].setModule(fragShaderModule);
        shaderStages[1].setPName("main");
        shaderStages[1].flags = vk::PipelineShaderStageCreateFlags(0);
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;

        auto& bindingDescriptions = _configInfo.bindingDescriptions;
        auto& attributeDescriptions = _configInfo.attributeDescriptions;
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
        vertexInputInfo.setVertexAttributeDescriptions(attributeDescriptions);
        vertexInputInfo.setVertexBindingDescriptions(bindingDescriptions);

        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
        pipelineInfo.setStageCount(2);
        pipelineInfo.setPStages(shaderStages);
        pipelineInfo.setPVertexInputState(&vertexInputInfo);
        pipelineInfo.setPInputAssemblyState(&_configInfo.inputAssemblyInfo);
        pipelineInfo.setPViewportState(&_configInfo.viewportInfo);
        pipelineInfo.setPRasterizationState(&_configInfo.rasterizationInfo);
        pipelineInfo.setPMultisampleState(&_configInfo.multisampleInfo);
        pipelineInfo.setPColorBlendState(&_configInfo.colorBlendInfo);
        pipelineInfo.setPDepthStencilState(&_configInfo.depthStencilInfo);
        pipelineInfo.setPDynamicState(&_configInfo.dynamicStateInfo);

        pipelineInfo.setLayout(_configInfo.pipelineLayout);
        pipelineInfo.setRenderPass(_configInfo.renderPass);
        pipelineInfo.setSubpass(_configInfo.subpass);

        pipelineInfo.setBasePipelineIndex(-1);
        pipelineInfo.setBasePipelineHandle(nullptr);

        try {
            auto result = lveDevice.device().createGraphicsPipeline(nullptr, pipelineInfo,nullptr);
            if (result.result != vk::Result::eSuccess) {
                throw std::runtime_error("failed to create graphics pipeline!");
            }
            graphicsPipeline = result.value;
        }
        catch (const vk::SystemError& e) {
            throw std::runtime_error("failed to create graphics pipeline: " + std::string(e.what()));
        }
    }

    void LvePipeline::CreateShaderModule(const std::vector<char>& _code, vk::ShaderModule* _shaderModule) {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
        createInfo.setCodeSize(_code.size());
        createInfo.setPCode(reinterpret_cast<const uint32_t*>(_code.data()));

        try {
            *_shaderModule = lveDevice.device().createShaderModule(createInfo);
        }
        catch (const vk::SystemError& e) {
            throw std::runtime_error("failed to create shader module: " + std::string(e.what()));
        }
    }


    void LvePipeline::Bind(vk::CommandBuffer _commandBuffer) {
        _commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
    }

    void LvePipeline::DefaultPipelineConfigInfo(PipelineConfigInfo& _configInfo) {
        _configInfo.inputAssemblyInfo.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
        _configInfo.inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
        _configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        _configInfo.viewportInfo.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
        _configInfo.viewportInfo.viewportCount = 1;
        _configInfo.viewportInfo.pViewports = nullptr;
        _configInfo.viewportInfo.scissorCount = 1;
        _configInfo.viewportInfo.pScissors = nullptr;

        _configInfo.rasterizationInfo.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
        _configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
        _configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        _configInfo.rasterizationInfo.polygonMode = vk::PolygonMode::eFill;
        _configInfo.rasterizationInfo.lineWidth = 1.0f;
        _configInfo.rasterizationInfo.cullMode = vk::CullModeFlagBits::eNone;
        _configInfo.rasterizationInfo.frontFace = vk::FrontFace::eClockwise;
        _configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
        _configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
        _configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
        _configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

        _configInfo.multisampleInfo.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
        _configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        _configInfo.multisampleInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
        _configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
        _configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
        _configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
        _configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

        _configInfo.colorBlendAttachment.colorWriteMask =
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        _configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
        _configInfo.colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;   // Optional
        _configInfo.colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;  // Optional
        _configInfo.colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;              // Optional
        _configInfo.colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;   // Optional
        _configInfo.colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;  // Optional
        _configInfo.colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;              // Optional

        _configInfo.colorBlendInfo.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
        _configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
        _configInfo.colorBlendInfo.logicOp = vk::LogicOp::eCopy;  // Optional
        _configInfo.colorBlendInfo.attachmentCount = 1;
        _configInfo.colorBlendInfo.pAttachments = &_configInfo.colorBlendAttachment;
        _configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
        _configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
        _configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
        _configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

        _configInfo.depthStencilInfo.sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo;
        _configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        _configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
        _configInfo.depthStencilInfo.depthCompareOp = vk::CompareOp::eLess;
        _configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        _configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
        _configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
        _configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
        _configInfo.depthStencilInfo.front = vk::StencilOpState{};  // Optional
        _configInfo.depthStencilInfo.back = vk::StencilOpState{};   // Optional

        _configInfo.dynamicStateEnables = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        _configInfo.dynamicStateInfo.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
        _configInfo.dynamicStateInfo.pDynamicStates = _configInfo.dynamicStateEnables.data();
        _configInfo.dynamicStateInfo.dynamicStateCount =
            static_cast<uint32_t>(_configInfo.dynamicStateEnables.size());
        _configInfo.dynamicStateInfo.flags = vk::PipelineDynamicStateCreateFlags(0);

        _configInfo.bindingDescriptions = LveModel::Vertex::GetBindingDescriptions();
        _configInfo.attributeDescriptions = LveModel::Vertex::GetAttributeDescriptions();
    }



    void LvePipeline::EnableAlphaBlending(PipelineConfigInfo& _configInfo) {
        _configInfo.colorBlendAttachment.blendEnable = VK_TRUE;

        _configInfo.colorBlendAttachment.colorWriteMask =
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

        _configInfo.colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        _configInfo.colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        _configInfo.colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
        _configInfo.colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
        _configInfo.colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
        _configInfo.colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;
    }

}  // namespace lve