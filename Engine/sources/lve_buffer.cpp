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

namespace lve
{
	/**
	 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
	 *
	 * @param instanceSize The size of an instance
	 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
	 * minUniformBufferOffsetAlignment)
	 *
	 * @return vk::Result of the buffer mapping call
	 */
	vk::DeviceSize LveBuffer::GetAlignment(vk::DeviceSize _instanceSize, vk::DeviceSize _minOffsetAlignment)
	{
		if (_minOffsetAlignment > 0) return (_instanceSize + _minOffsetAlignment - 1) & ~(_minOffsetAlignment - 1);
		return _instanceSize;
	}

	LveBuffer::LveBuffer(
		LveDevice&              _device,
		vk::DeviceSize          _instanceSize,
		uint32_t                _instanceCount,
		vk::BufferUsageFlags    _usageFlags,
		vk::MemoryPropertyFlags _memoryPropertyFlags,
		vk::DeviceSize          _minOffsetAlignment) : lveDevice{_device},
		                                      instanceCount{_instanceCount},
		                                      instanceSize{_instanceSize},
		                                      usageFlags{_usageFlags},
		                                      memoryPropertyFlags{_memoryPropertyFlags}
	{
		alignmentSize = GetAlignment(_instanceSize, _minOffsetAlignment);
		bufferSize    = alignmentSize * instanceCount;
		_device.CreateBuffer(bufferSize, usageFlags, _memoryPropertyFlags, buffer, memory);
	}

	LveBuffer::~LveBuffer()
	{
		Unmap();
		lveDevice.Device().destroyBuffer(buffer, nullptr);
		lveDevice.Device().freeMemory(memory, nullptr);
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
	vk::Result LveBuffer::Map(vk::DeviceSize size, vk::DeviceSize offset)
	{
		assert(buffer && memory && "Called map on buffer before create");
		return lveDevice.Device().mapMemory(memory, offset, size, vk::MemoryMapFlags(), &mapped);
	}

	/**
	 * Unmap a mapped memory range
	 *
	 * @note Does not return a result as vkUnmapMemory can't fail
	 */
	void LveBuffer::Unmap()
	{
		if (mapped)
		{
			lveDevice.Device().unmapMemory(memory);
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
	void LveBuffer::WriteToBuffer(void* data, vk::DeviceSize size, vk::DeviceSize offset)
	{
		assert(mapped && "Cannot copy to unmapped buffer");

		if (size == VK_WHOLE_SIZE)
		{
			memcpy(mapped, data, bufferSize);
		}
		else
		{
			auto memOffset = static_cast<char*>(mapped);
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
	vk::Result LveBuffer::Flush(vk::DeviceSize size, vk::DeviceSize offset)
	{
		vk::MappedMemoryRange mappedRange{};
		mappedRange.memory = memory;
		mappedRange.offset = offset;
		mappedRange.size   = size;
		return lveDevice.Device().flushMappedMemoryRanges(1, &mappedRange);
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
	vk::Result LveBuffer::Invalidate(vk::DeviceSize size, vk::DeviceSize offset)
	{
		vk::MappedMemoryRange mappedRange{};
		mappedRange.memory = memory;
		mappedRange.offset = offset;
		mappedRange.size   = size;
		return lveDevice.Device().invalidateMappedMemoryRanges(1, &mappedRange);
	}

	/**
	 * Create a buffer info descriptor
	 *
	 * @param size (Optional) Size of the memory range of the descriptor
	 * @param offset (Optional) Byte offset from beginning
	 *
	 * @return vk::DescriptorBufferInfo of specified offset and range
	 */
	vk::DescriptorBufferInfo LveBuffer::DescriptorInfo(vk::DeviceSize size, vk::DeviceSize offset)
	{
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
	void LveBuffer::WriteToIndex(void* data, int index)
	{
		WriteToBuffer(data, instanceSize, index * alignmentSize);
	}

	/**
	 *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
	 *
	 * @param index Used in offset calculation
	 *
	 */
	vk::Result LveBuffer::FlushIndex(int index) { return Flush(alignmentSize, index * alignmentSize); }

	/**
	 * Create a buffer info descriptor
	 *
	 * @param index Specifies the region given by index * alignmentSize
	 *
	 * @return vk::DescriptorBufferInfo for instance at index
	 */
	vk::DescriptorBufferInfo LveBuffer::DescriptorInfoForIndex(int index)
	{
		return DescriptorInfo(alignmentSize, index * alignmentSize);
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
	vk::Result LveBuffer::InvalidateIndex(int index)
	{
		return Invalidate(alignmentSize, index * alignmentSize);
	}
} // namespace lve
