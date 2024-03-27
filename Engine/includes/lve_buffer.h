#pragma once

#include "lve_device.h"

namespace lve {

    class LveBuffer {
    public:
        LveBuffer(
            LveDevice& _device,
            VkDeviceSize _instanceSize,
            uint32_t _instanceCount,
            VkBufferUsageFlags _usageFlags,
            VkMemoryPropertyFlags _memoryPropertyFlags,
            VkDeviceSize _minOffsetAlignment = 1);
        ~LveBuffer();

        LveBuffer(const LveBuffer&) = delete;
        LveBuffer& operator=(const LveBuffer&) = delete;


        VkResult map(VkDeviceSize _size = VK_WHOLE_SIZE, VkDeviceSize _offset = 0);


        void unmap();


        void writeToBuffer(void* _data, VkDeviceSize _size = VK_WHOLE_SIZE, VkDeviceSize _offset = 0);


        VkResult flush(VkDeviceSize _size = VK_WHOLE_SIZE, VkDeviceSize _offset = 0);


        VkDescriptorBufferInfo descriptorInfo(VkDeviceSize _size = VK_WHOLE_SIZE, VkDeviceSize _offset = 0);


        VkResult invalidate(VkDeviceSize _size = VK_WHOLE_SIZE, VkDeviceSize _offset = 0);


        void writeToIndex(void* _data, int _index);


        VkResult flushIndex(int _index);


        VkDescriptorBufferInfo descriptorInfoForIndex(int _index);


        VkResult invalidateIndex(int _index);

        VkBuffer getBuffer() const { return buffer; }
        void* getMappedMemory() const { return mapped; }
        uint32_t getInstanceCount() const { return instanceCount; }
        VkDeviceSize getInstanceSize() const { return instanceSize; }
        VkDeviceSize getAlignmentSize() const { return instanceSize; }
        VkBufferUsageFlags getUsageFlags() const { return usageFlags; }
        VkMemoryPropertyFlags getMemoryPropertyFlags() const { return memoryPropertyFlags; }
        VkDeviceSize getBufferSize() const { return bufferSize; }

    private:


        static VkDeviceSize getAlignment(VkDeviceSize _instanceSize, VkDeviceSize _minOffsetAlignment);

        LveDevice& lveDevice;
        void* mapped = nullptr;
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;

        VkDeviceSize bufferSize;
        uint32_t instanceCount;
        VkDeviceSize instanceSize;
        VkDeviceSize alignmentSize;
        VkBufferUsageFlags usageFlags;
        VkMemoryPropertyFlags memoryPropertyFlags;
    };

}  // namespace lve
