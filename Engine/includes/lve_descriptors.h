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
                vk::DescriptorType _descriptorType,
                vk::ShaderStageFlags _stageFlags,
                uint32_t _count = 1);
            std::unique_ptr<LveDescriptorSetLayout> Build() const;

        private:
            LveDevice& lveDevice;
            std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> bindings{};
        };

        LveDescriptorSetLayout(
            LveDevice& _lveDevice, std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> _bindings);
        ~LveDescriptorSetLayout();
        LveDescriptorSetLayout(const LveDescriptorSetLayout&) = delete;
        LveDescriptorSetLayout& operator=(const LveDescriptorSetLayout&) = delete;

        vk::DescriptorSetLayout GetDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        LveDevice& lveDevice;
        vk::DescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> bindings;

        friend class LveDescriptorWriter;
    };

    class LveDescriptorPool {
    public:
        class Builder {
        public:
            Builder(LveDevice& _lveDevice) : lveDevice{ _lveDevice } {}

            Builder& AddPoolSize(vk::DescriptorType _descriptorType, uint32_t _count);
            Builder& SetPoolFlags(vk::DescriptorPoolCreateFlags _flags);
            Builder& SetMaxSets(uint32_t _count);
            std::unique_ptr<LveDescriptorPool> Build() const;

        private:
            LveDevice& lveDevice;
            std::vector<vk::DescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            vk::DescriptorPoolCreateFlags poolFlags = vk::DescriptorPoolCreateFlags{};
        };

        LveDescriptorPool(
            LveDevice& _lveDevice,
            uint32_t _maxSets,
            vk::DescriptorPoolCreateFlags _poolFlags,
            const std::vector<vk::DescriptorPoolSize>& _poolSizes);
        ~LveDescriptorPool();
        LveDescriptorPool(const LveDescriptorPool&) = delete;
        LveDescriptorPool& operator=(const LveDescriptorPool&) = delete;

        bool AllocateDescriptor(
            const vk::DescriptorSetLayout _descriptorSetLayout, vk::DescriptorSet& _descriptor) const;

        void FreeDescriptors(std::vector<vk::DescriptorSet>& _descriptors) const;

        void ResetPool();

    private:
        LveDevice& lveDevice;
        vk::DescriptorPool descriptorPool;

        friend class LveDescriptorWriter;
    };

    class LveDescriptorWriter {
    public:
        LveDescriptorWriter(LveDescriptorSetLayout& _setLayout, LveDescriptorPool& _pool);

        LveDescriptorWriter& WriteBuffer(uint32_t _binding, vk::DescriptorBufferInfo* _bufferInfo);
        LveDescriptorWriter& WriteImage(uint32_t _binding, vk::DescriptorImageInfo* _imageInfo);

        bool Build(vk::DescriptorSet& _set);
        void Overwrite(vk::DescriptorSet& _set);

    private:
        LveDescriptorSetLayout& setLayout;
        LveDescriptorPool& pool;
        std::vector<vk::WriteDescriptorSet> writes;
    };

}  // namespace lve