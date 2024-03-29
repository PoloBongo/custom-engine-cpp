#include "lve_buffer.h"
/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */


 // std
#include <cassert>
#include <cstring>

namespace lve {

    /**
     * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
     *
     * @param instanceSize The size of an instance
     * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
     * minUniformBufferOffsetAlignment)
     *
     * @return vk::Result of the buffer mapping call
     */
    vk::DeviceSize LveBuffer::getAlignment(vk::DeviceSize _instanceSize, vk::DeviceSize _minOffsetAlignment) {
        if (_minOffsetAlignment > 0) {
            return (_instanceSize + _minOffsetAlignment - 1) & ~(_minOffsetAlignment - 1);
        }
        return _instanceSize;
    }

    LveBuffer::LveBuffer(
        LveDevice& _device,
        vk::DeviceSize _instanceSize,
        uint32_t _instanceCount,
        vk::BufferUsageFlags _usageFlags,
        vk::MemoryPropertyFlags _memoryPropertyFlags,
        vk::DeviceSize _minOffsetAlignment)
        : lveDevice{ _device },
        instanceSize{ _instanceSize },
        instanceCount{ _instanceCount },
        usageFlags{ _usageFlags },
        memoryPropertyFlags{ _memoryPropertyFlags } {
        alignmentSize = getAlignment(_instanceSize, _minOffsetAlignment);
        bufferSize = alignmentSize * instanceCount;
        _device.createBuffer(bufferSize, usageFlags, _memoryPropertyFlags, buffer, memory);
    }

    LveBuffer::~LveBuffer() {
        unmap();
        lveDevice.device().destroyBuffer( buffer, nullptr);
        lveDevice.device().freeMemory(memory, nullptr);
    }

    /**
     * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
     *
     * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
     * buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return vk::Result of the buffer mapping call
     */
    vk::Result LveBuffer::map(vk::DeviceSize size, vk::DeviceSize offset) {
        assert(buffer && memory && "Called map on buffer before create");
        return lveDevice.device().mapMemory(memory, offset, size, vk::MemoryMapFlags(), &mapped);
    }

    /**
     * Unmap a mapped memory range
     *
     * @note Does not return a result as vkUnmapMemory can't fail
     */
    void LveBuffer::unmap() {
        if (mapped) {
            lveDevice.device().unmapMemory(memory);
            mapped = nullptr;
        }
    }

    /**
     * Copies the specified data to the mapped buffer. Default value writes whole buffer range
     *
     * @param data Pointer to the data to copy
     * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
     * range.
     * @param offset (Optional) Byte offset from beginning of mapped region
     *
     */
    void LveBuffer::writeToBuffer(void* data, vk::DeviceSize size, vk::DeviceSize offset) {
        assert(mapped && "Cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE) {
            memcpy(mapped, data, bufferSize);
        }
        else {
            char* memOffset = static_cast<char*>(mapped);
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    /**
     * Flush a memory range of the buffer to make it visible to the device
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
     * complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return vk::Result of the flush call
     */
    vk::Result LveBuffer::flush(vk::DeviceSize size, vk::DeviceSize offset) {
        vk::MappedMemoryRange mappedRange{};
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return lveDevice.device().flushMappedMemoryRanges(1, &mappedRange);
    }

    /**
     * Invalidate a memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
     * the complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return vk::Result of the invalidate call
     */
    vk::Result LveBuffer::invalidate(vk::DeviceSize size, vk::DeviceSize offset) {
        vk::MappedMemoryRange mappedRange{};
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return lveDevice.device().invalidateMappedMemoryRanges(1, &mappedRange);
    }

    /**
     * Create a buffer info descriptor
     *
     * @param size (Optional) Size of the memory range of the descriptor
     * @param offset (Optional) Byte offset from beginning
     *
     * @return vk::DescriptorBufferInfo of specified offset and range
     */
    vk::DescriptorBufferInfo LveBuffer::descriptorInfo(vk::DeviceSize size, vk::DeviceSize offset) {
        return vk::DescriptorBufferInfo{
            buffer,
            offset,
            size,
        };
    }

    /**
     * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
     *
     * @param data Pointer to the data to copy
     * @param index Used in offset calculation
     *
     */
    void LveBuffer::writeToIndex(void* data, int index) {
        writeToBuffer(data, instanceSize, index * alignmentSize);
    }

    /**
     *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
     *
     * @param index Used in offset calculation
     *
     */
    vk::Result LveBuffer::flushIndex(int index) { return flush(alignmentSize, index * alignmentSize); }

    /**
     * Create a buffer info descriptor
     *
     * @param index Specifies the region given by index * alignmentSize
     *
     * @return vk::DescriptorBufferInfo for instance at index
     */
    vk::DescriptorBufferInfo LveBuffer::descriptorInfoForIndex(int index) {
        return descriptorInfo(alignmentSize, index * alignmentSize);
    }

    /**
     * Invalidate a memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param index Specifies the region to invalidate: index * alignmentSize
     *
     * @return vk::Result of the invalidate call
     */
    vk::Result LveBuffer::invalidateIndex(int index) {
        return invalidate(alignmentSize, index * alignmentSize);
    }

}  // namespace lve
