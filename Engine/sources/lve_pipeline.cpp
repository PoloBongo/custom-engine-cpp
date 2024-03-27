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
        vkDestroyShaderModule(lveDevice.device(), vertShaderModule, nullptr);
        vkDestroyShaderModule(lveDevice.device(), fragShaderModule, nullptr);
        vkDestroyPipeline(lveDevice.device(), graphicsPipeline, nullptr);
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
            _configInfo.pipelineLayout != VK_NULL_HANDLE &&
            "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
        assert(
            _configInfo.renderPass != VK_NULL_HANDLE &&
            "Cannot create graphics pipeline: no renderPass provided in configInfo");

        auto vertCode = ReadFile(_vertFilepath);
        auto fragCode = ReadFile(_fragFilepath);

        CreateShaderModule(vertCode, &vertShaderModule);
        CreateShaderModule(fragCode, &fragShaderModule);


        /***********************/
        /**** SHADER STAGES ****/
        /***********************/

        VkPipelineShaderStageCreateInfo shaderStages[2];
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vertShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[0].flags = 0;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;
        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = fragShaderModule;
        shaderStages[1].pName = "main";
        shaderStages[1].flags = 0;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;

        auto& bindingDescriptions = _configInfo.bindingDescriptions;
        auto& attributeDescriptions = _configInfo.attributeDescriptions;
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &_configInfo.inputAssemblyInfo;
        pipelineInfo.pViewportState = &_configInfo.viewportInfo;
        pipelineInfo.pRasterizationState = &_configInfo.rasterizationInfo;
        pipelineInfo.pMultisampleState = &_configInfo.multisampleInfo;
        pipelineInfo.pColorBlendState = &_configInfo.colorBlendInfo;
        pipelineInfo.pDepthStencilState = &_configInfo.depthStencilInfo;
        pipelineInfo.pDynamicState = &_configInfo.dynamicStateInfo;

        pipelineInfo.layout = _configInfo.pipelineLayout;
        pipelineInfo.renderPass = _configInfo.renderPass;
        pipelineInfo.subpass = _configInfo.subpass;

        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(
            lveDevice.device(),
            VK_NULL_HANDLE,
            1,
            &pipelineInfo,
            nullptr,
            &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline");
        }
    }

    void LvePipeline::CreateShaderModule(const std::vector<char>& _code, VkShaderModule* _shaderModule) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = _code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(_code.data());

        if (vkCreateShaderModule(lveDevice.device(), &createInfo, nullptr , _shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module");
        }
    }

    void LvePipeline::Bind(VkCommandBuffer _commandBuffer) {
        vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    }

    void LvePipeline::DefaultPipelineConfigInfo(PipelineConfigInfo& _configInfo) {
        
        // Configure le type d'informations d'assemblage pour le pipeline
        _configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

        // Spécifie la topologie des primitives, qui peut être :
        // - VK_PRIMITIVE_TOPOLOGY_POINT_LIST : liste de points
        // - VK_PRIMITIVE_TOPOLOGY_LINE_LIST : liste de lignes
        // - VK_PRIMITIVE_TOPOLOGY_LINE_STRIP : bande de lignes
        // - VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : liste de triangles
        // - VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : bande de triangles
        _configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;


        // Active ou désactive la possibilité de redémarrer les primitives, 
        // ce qui signifie que si cela est activé, lorsqu'un certain index 
        // est rencontré dans le flux d'indices, une nouvelle primitive est commencée
        _configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;


        /******************/
        /**** VIEWPORT ****/
        /******************/

        // Configuration de l'état des viewports pour le pipeline
        _configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        
        // Nombre de viewports à utiliser. Un viewport définit la région de l'image à dessiner
        _configInfo.viewportInfo.viewportCount = 1;
        
        // Pointeur vers un tableau de viewports, nullptr signifie que la valeur par défaut est utilisée
        _configInfo.viewportInfo.pViewports = nullptr;
        
        // Nombre de ciseaux de découpe à utiliser. Un ciseau de découpe définit la zone de l'image sur laquelle le dessin est limité
        _configInfo.viewportInfo.scissorCount = 1;
        
        // Pointeur vers un tableau de ciseaux de découpe, nullptr signifie que la valeur par défaut est utilisée
        _configInfo.viewportInfo.pScissors = nullptr;


        /***********************/
        /**** RASTERIZATION ****/
        /***********************/

        // Configuration de l'état de la rasterisation pour le pipeline
        _configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

        // Activation ou désactivation de la clamping de profondeur. Si activé, les fragments en dehors de la plage de profondeur sont clippés plutôt que d'être rejetés
        _configInfo.rasterizationInfo.depthClampEnable = VK_FALSE; // ou VK_FALSE

        // Activation ou désactivation du rejet de rasterization. Si activé, la géométrie de la primitive n'est pas envoyée au pipeline de rasterization.
        _configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE; // ou VK_FALSE

        // Mode de remplissage des polygones. Les options incluent VK_POLYGON_MODE_FILL, VK_POLYGON_MODE_LINE et VK_POLYGON_MODE_POINT
        _configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL; // ou VK_POLYGON_MODE_LINE ou VK_POLYGON_MODE_POINT

        // Épaisseur de ligne lors du rendu en mode ligne (si utilisé)
        _configInfo.rasterizationInfo.lineWidth = 1.0f; // tout float
        
        // Mode de culling des faces. Les options incluent VK_CULL_MODE_NONE, VK_CULL_MODE_FRONT_BIT et VK_CULL_MODE_BACK_BIT
        _configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE; // ou VK_CULL_MODE_FRONT_BIT ou VK_CULL_MODE_BACK_BIT ou VK_CULL_MODE_FRONT_AND_BACK
        
        // Orientation des faces à culler. VK_FRONT_FACE_CLOCKWISE ou VK_FRONT_FACE_COUNTER_CLOCKWISE
        _configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE; // ou VK_FRONT_FACE_COUNTER_CLOCKWISE
        
        // Activation ou désactivation du décalage de profondeur. Si activé, il ajuste la profondeur des fragments pour éviter le z-fighting.
        _configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE; // ou VK_FALSE
        
        // Facteur de décalage constant pour chaque fragment
        _configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f; // tout float
        
        // Valeur maximale (ou minimale) du décalage de profondeur
        _configInfo.rasterizationInfo.depthBiasClamp = 0.0f; // tout float

        _configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;


        /*********************/
        /**** MULTISAMPLE ****/
        /*********************/

        _configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        _configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;

        // Le nombre d'échantillons de rasterization utilisés. Les options incluent :
        // - VK_SAMPLE_COUNT_1_BIT : Aucun échantillonnage. Un seul échantillon est utilisé.
        // - VK_SAMPLE_COUNT_2_BIT : 2 échantillons par pixel.
        // - VK_SAMPLE_COUNT_4_BIT : 4 échantillons par pixel.
        // - VK_SAMPLE_COUNT_8_BIT : 8 échantillons par pixel.
        // - VK_SAMPLE_COUNT_16_BIT : 16 échantillons par pixel.
        // - VK_SAMPLE_COUNT_32_BIT : 32 échantillons par pixel.
        // - VK_SAMPLE_COUNT_64_BIT : 64 échantillons par pixel.
        _configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // Choisissez la valeur appropriée en fonction des besoins de qualité d'image et de performances.
        _configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
        _configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
        _configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
        _configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional


        /********************************/
        /**** COLORBLEND ATTACHEMENT ****/
        /********************************/

        _configInfo.colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        _configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
        _configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        _configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        _configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
        _configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        _configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        _configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional


        /********************/
        /**** COLORBLEND ****/
        /********************/

        _configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        _configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;

        // L'opération logique à appliquer. Les options incluent :
        // - VK_LOGIC_OP_CLEAR : Résultat de 0.
        // - VK_LOGIC_OP_AND : Résultat de A & B.
        // - VK_LOGIC_OP_AND_REVERSE : Résultat de A & ~B.
        // - VK_LOGIC_OP_COPY : Résultat de A.
        // - VK_LOGIC_OP_SET : Résultat de 1.
        // - VK_LOGIC_OP_COPY_INVERTED : Résultat de ~A.
        // - VK_LOGIC_OP_NO_OP : Ne fait rien.
        // - VK_LOGIC_OP_INVERT : Résultat de ~A & B.
        // - VK_LOGIC_OP_OR : Résultat de A | B.
        // - VK_LOGIC_OP_NOR : Résultat de ~(A | B).
        // - VK_LOGIC_OP_XOR : Résultat de A ^ B.
        // - VK_LOGIC_OP_EQUIVALENT : Résultat de ~(A ^ B).
        // - VK_LOGIC_OP_AND_INVERTED : Résultat de ~A & B.
        // - VK_LOGIC_OP_OR_REVERSE : Résultat de A | ~B.
        // - VK_LOGIC_OP_OR_INVERTED : Résultat de ~A | B.
        // - VK_LOGIC_OP_NAND : Résultat de ~(A & B).
        _configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // ou une autre opération logique selon les besoins. // Optional
        _configInfo.colorBlendInfo.attachmentCount = 1;
        _configInfo.colorBlendInfo.pAttachments = &_configInfo.colorBlendAttachment;
        _configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
        _configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
        _configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
        _configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional


        /***********************/
        /**** DEPTH STENCIL ****/
        /***********************/

        _configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        _configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        _configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
        _configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        _configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        _configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
        _configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
        _configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
        _configInfo.depthStencilInfo.front = {};  // Optional
        _configInfo.depthStencilInfo.back = {};   // Optional


        /***********************/
        /**** DYNAMIC STATE ****/
        /***********************/

        _configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        _configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        _configInfo.dynamicStateInfo.pDynamicStates = _configInfo.dynamicStateEnables.data();
        _configInfo.dynamicStateInfo.dynamicStateCount =
            static_cast<uint32_t>(_configInfo.dynamicStateEnables.size());
        _configInfo.dynamicStateInfo.flags = 0;

        _configInfo.bindingDescriptions = LveModel::Vertex::GetBindingDescriptions();
        _configInfo.attributeDescriptions = LveModel::Vertex::GetAttributeDescriptions();
    }

}  // namespace lve