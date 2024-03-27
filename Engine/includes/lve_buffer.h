#pragma once

#include "lve_device.h"

namespace lve {

    class LveBuffer {
    public:
        LveBuffer(
            LveDevice& _device,
            vk::DeviceSize _instanceSize,
            uint32_t _instanceCount,
            vk::BufferUsageFlags _usageFlags,
            vk::MemoryPropertyFlags _memoryPropertyFlags,
            vk::DeviceSize _minOffsetAlignment = 1);
        ~LveBuffer();

        LveBuffer(const LveBuffer&) = delete;
        LveBuffer& operator=(const LveBuffer&) = delete;

        VkResult map(vk::DeviceSize _size = VK_WHOLE_SIZE, vk::DeviceSize _offset = 0);
        void unmap();

        void writeToBuffer(void* _data, vk::DeviceSize _size = VK_WHOLE_SIZE, vk::DeviceSize _offset = 0);
        VkResult flush(vk::DeviceSize _size = VK_WHOLE_SIZE, vk::DeviceSize _offset = 0);
        VkDescriptorBufferInfo descriptorInfo(vk::DeviceSize _size = VK_WHOLE_SIZE, vk::DeviceSize _offset = 0);
        VkResult invalidate(vk::DeviceSize _size = VK_WHOLE_SIZE, vk::DeviceSize _offset = 0);

        void writeToIndex(void* _data, int _index);
        VkResult flushIndex(int _index);
        VkDescriptorBufferInfo descriptorInfoForIndex(int _index);
        VkResult invalidateIndex(int _index);

        VkBuffer getBuffer() const { return buffer; }
        void* getMappedMemory() const { return mapped; }
        uint32_t getInstanceCount() const { return instanceCount; }
        vk::DeviceSize getInstanceSize() const { return instanceSize; }
        vk::DeviceSize getAlignmentSize() const { return instanceSize; }
        vk::BufferUsageFlags getUsageFlags() const { return usageFlags; }
        vk::MemoryPropertyFlags getMemoryPropertyFlags() const { return memoryPropertyFlags; }
        vk::DeviceSize getBufferSize() const { return bufferSize; }

    private:
        static vk::DeviceSize getAlignment(vk::DeviceSize _instanceSize, vk::DeviceSize _minOffsetAlignment);

        LveDevice& lveDevice;
        void* mapped = nullptr;
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;

        vk::DeviceSize bufferSize;
        uint32_t instanceCount;
        vk::DeviceSize instanceSize;
        vk::DeviceSize alignmentSize;
        vk::BufferUsageFlags usageFlags;
        vk::MemoryPropertyFlags memoryPropertyFlags;
    };

}  // namespace lve
