#include "lve_descriptors.h"
// std
#include <cassert>
#include <stdexcept>

namespace lve {

    // *************** Descriptor Set Layout Builder *********************

    LveDescriptorSetLayout::Builder& LveDescriptorSetLayout::Builder::AddBinding(
        uint32_t _binding,
        VkDescriptorType _descriptorType,
        VkShaderStageFlags _stageFlags,
        uint32_t _count) {
        assert(bindings.count(_binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = _binding;
        layoutBinding.descriptorType = _descriptorType;
        layoutBinding.descriptorCount = _count;
        layoutBinding.stageFlags = _stageFlags;
        bindings[_binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<LveDescriptorSetLayout> LveDescriptorSetLayout::Builder::Build() const {
        return std::make_unique<LveDescriptorSetLayout>(lveDevice, bindings);
    }

    // *************** Descriptor Set Layout *********************

    LveDescriptorSetLayout::LveDescriptorSetLayout(
        LveDevice& _lveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> _bindings)
        : lveDevice{ _lveDevice }, bindings{ _bindings } {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : _bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
            lveDevice.device(),
            &descriptorSetLayoutInfo,
            nullptr,
            &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    LveDescriptorSetLayout::~LveDescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(lveDevice.device(), descriptorSetLayout, nullptr);
    }

    // *************** Descriptor Pool Builder *********************

    LveDescriptorPool::Builder& LveDescriptorPool::Builder::AddPoolSize(
        VkDescriptorType _descriptorType, uint32_t _count) {
        poolSizes.push_back({ _descriptorType, _count });
        return *this;
    }

    LveDescriptorPool::Builder& LveDescriptorPool::Builder::SetPoolFlags(
        VkDescriptorPoolCreateFlags _flags) {
        poolFlags = _flags;
        return *this;
    }
    LveDescriptorPool::Builder& LveDescriptorPool::Builder::SetMaxSets(uint32_t _count) {
        maxSets = _count;
        return *this;
    }

    std::unique_ptr<LveDescriptorPool> LveDescriptorPool::Builder::Build() const {
        return std::make_unique<LveDescriptorPool>(lveDevice, maxSets, poolFlags, poolSizes);
    }

    // *************** Descriptor Pool *********************

    LveDescriptorPool::LveDescriptorPool(
        LveDevice& lveDevice,
        uint32_t _maxSets,
        VkDescriptorPoolCreateFlags _poolFlags,
        const std::vector<VkDescriptorPoolSize>& _poolSizes)
        : lveDevice{ lveDevice } {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(_poolSizes.size());
        descriptorPoolInfo.pPoolSizes = _poolSizes.data();
        descriptorPoolInfo.maxSets = _maxSets;
        descriptorPoolInfo.flags = _poolFlags;

        if (vkCreateDescriptorPool(lveDevice.device(), &descriptorPoolInfo, nullptr, &descriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    LveDescriptorPool::~LveDescriptorPool() {
        vkDestroyDescriptorPool(lveDevice.device(), descriptorPool, nullptr);
    }

    bool LveDescriptorPool::AllocateDescriptor(
        const VkDescriptorSetLayout _descriptorSetLayout, VkDescriptorSet& _descriptor) const {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.pSetLayouts = &_descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(lveDevice.device(), &allocInfo, &_descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    void LveDescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet>& _descriptors) const {
        vkFreeDescriptorSets(
            lveDevice.device(),
            descriptorPool,
            static_cast<uint32_t>(_descriptors.size()),
            _descriptors.data());
    }

    void LveDescriptorPool::ResetPool() {
        vkResetDescriptorPool(lveDevice.device(), descriptorPool, 0);
    }

    // *************** Descriptor Writer *********************

    LveDescriptorWriter::LveDescriptorWriter(LveDescriptorSetLayout& _setLayout, LveDescriptorPool& _pool)
        : setLayout{ _setLayout }, pool{ _pool } {}

    LveDescriptorWriter& LveDescriptorWriter::WriteBuffer(
        uint32_t _binding, VkDescriptorBufferInfo* _bufferInfo) {
        assert(setLayout.bindings.count(_binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[_binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = _binding;
        write.pBufferInfo = _bufferInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    LveDescriptorWriter& LveDescriptorWriter::WriteImage(
        uint32_t _binding, VkDescriptorImageInfo* _imageInfo) {
        assert(setLayout.bindings.count(_binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[_binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = _binding;
        write.pImageInfo = _imageInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    bool LveDescriptorWriter::Build(VkDescriptorSet& _set) {
        bool success = pool.AllocateDescriptor(setLayout.GetDescriptorSetLayout(), _set);
        if (!success) {
            return false;
        }
        Overwrite(_set);
        return true;
    }

    void LveDescriptorWriter::Overwrite(VkDescriptorSet& _set) {
        for (auto& write : writes) {
            write.dstSet = _set;
        }
        vkUpdateDescriptorSets(pool.lveDevice.device(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    }

}  // namespace lve

