#pragma once

#include "lve_device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace lve {

    class LveDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(LveDevice& _lveDevice) : lveDevice{ _lveDevice } {}

            Builder& AddBinding(
                uint32_t _binding,
                VkDescriptorType _descriptorType,
                VkShaderStageFlags _stageFlags,
                uint32_t _count = 1);
            std::unique_ptr<LveDescriptorSetLayout> Build() const;

        private:
            LveDevice& lveDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        LveDescriptorSetLayout(
            LveDevice& _lveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> _bindings);
        ~LveDescriptorSetLayout();
        LveDescriptorSetLayout(const LveDescriptorSetLayout&) = delete;
        LveDescriptorSetLayout& operator=(const LveDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout GetDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        LveDevice& lveDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class LveDescriptorWriter;
    };

    class LveDescriptorPool {
    public:
        class Builder {
        public:
            Builder(LveDevice& _lveDevice) : lveDevice{ _lveDevice } {}

            Builder& AddPoolSize(VkDescriptorType _descriptorType, uint32_t _count);
            Builder& SetPoolFlags(VkDescriptorPoolCreateFlags _flags);
            Builder& SetMaxSets(uint32_t _count);
            std::unique_ptr<LveDescriptorPool> Build() const;

        private:
            LveDevice& lveDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        LveDescriptorPool(
            LveDevice& _lveDevice,
            uint32_t _maxSets,
            VkDescriptorPoolCreateFlags _poolFlags,
            const std::vector<VkDescriptorPoolSize>& _poolSizes);
        ~LveDescriptorPool();
        LveDescriptorPool(const LveDescriptorPool&) = delete;
        LveDescriptorPool& operator=(const LveDescriptorPool&) = delete;

        bool AllocateDescriptor(
            const VkDescriptorSetLayout _descriptorSetLayout, VkDescriptorSet& _descriptor) const;

        void FreeDescriptors(std::vector<VkDescriptorSet>& _descriptors) const;

        void ResetPool();

    private:
        LveDevice& lveDevice;
        VkDescriptorPool descriptorPool;

        friend class LveDescriptorWriter;
    };

    class LveDescriptorWriter {
    public:
        LveDescriptorWriter(LveDescriptorSetLayout& _setLayout, LveDescriptorPool& _pool);

        LveDescriptorWriter& WriteBuffer(uint32_t _binding, VkDescriptorBufferInfo* _bufferInfo);
        LveDescriptorWriter& WriteImage(uint32_t _binding, VkDescriptorImageInfo* _imageInfo);

        bool Build(VkDescriptorSet& _set);
        void Overwrite(VkDescriptorSet& _set);

    private:
        LveDescriptorSetLayout& setLayout;
        LveDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace lve