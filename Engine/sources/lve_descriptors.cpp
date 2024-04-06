#include "lve_descriptors.h"
// std
#include <cassert>
#include <iostream>
#include <map>
#include <stdexcept>

namespace lve
{
	// *************** Descriptor Set Layout Builder *********************

	LveDescriptorSetLayout::Builder& LveDescriptorSetLayout::Builder::AddBinding(
		uint32_t             _binding,
		vk::DescriptorType   _descriptorType,
		vk::ShaderStageFlags _stageFlags,
		uint32_t             _count)
	{
		// Vérifie que le binding n'est pas déjà utilisé
		assert(bindings.count(_binding) == 0 && "Binding already in use");

		// Crée une nouvelle structure de layout de binding
		vk::DescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding         = _binding;        // Numéro du binding
		layoutBinding.descriptorType  = _descriptorType; // Type de descripteur
		layoutBinding.descriptorCount = _count;          // Nombre de descripteurs dans ce binding
		layoutBinding.stageFlags      = _stageFlags;     // Les stages de shader où ce binding est utilisé

		// Ajoute le binding à la map de bindings
		bindings[_binding] = layoutBinding;

		// Retourne une référence sur cet objet Builder pour permettre les appels en chaîne
		return *this;
	}


	std::unique_ptr<LveDescriptorSetLayout> LveDescriptorSetLayout::Builder::Build() const
	{
		return std::make_unique<LveDescriptorSetLayout>(lveDevice, bindings);
	}

	// *************** Descriptor Set Layout *********************

	LveDescriptorSetLayout::LveDescriptorSetLayout(
		LveDevice&                                                   _lveDevice,
		std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> _bindings) : lveDevice{_lveDevice},
		bindings{_bindings}
	{
		// Tri des bindings par indice de binding
		std::map<uint32_t, vk::DescriptorSetLayoutBinding> sortedBindings(
			_bindings.begin(), _bindings.end());

		// Création d'un vecteur contenant les bindings de l'ensemble de descripteurs
		std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings{};
		for (const auto& kv : sortedBindings)
		{
			setLayoutBindings.push_back(kv.second);
		}

		// Configuration des informations de création de l'ensemble de descripteurs
		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
		descriptorSetLayoutInfo.sType        = vk::StructureType::eDescriptorSetLayoutCreateInfo;
		descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
		descriptorSetLayoutInfo.pBindings    = setLayoutBindings.data();

		// Création de l'ensemble de descripteurs
		try
		{
			descriptorSetLayout = lveDevice.Device().createDescriptorSetLayout(descriptorSetLayoutInfo, nullptr);
		}
		catch (const vk::SystemError& e)
		{
			throw std::runtime_error("failed to create descriptor set layout: " + std::string(e.what()));
		}
	}


	LveDescriptorSetLayout::~LveDescriptorSetLayout()
	{
		lveDevice.Device().destroyDescriptorSetLayout(descriptorSetLayout, nullptr);
	}

	// *************** Descriptor Pool Builder *********************

	LveDescriptorPool::Builder& LveDescriptorPool::Builder::AddPoolSize(
		vk::DescriptorType _descriptorType, uint32_t _count)
	{
		poolSizes.push_back({_descriptorType, _count});
		return *this;
	}

	LveDescriptorPool::Builder& LveDescriptorPool::Builder::SetPoolFlags(
		vk::DescriptorPoolCreateFlags _flags)
	{
		poolFlags = _flags;
		return *this;
	}

	LveDescriptorPool::Builder& LveDescriptorPool::Builder::SetMaxSets(uint32_t _count)
	{
		maxSets = _count;
		return *this;
	}

	std::unique_ptr<LveDescriptorPool> LveDescriptorPool::Builder::Build() const
	{
		return std::make_unique<LveDescriptorPool>(lveDevice, maxSets, poolFlags, poolSizes);
	}

	// *************** Descriptor Pool *********************

	LveDescriptorPool::LveDescriptorPool(
		LveDevice&                                 lveDevice,
		uint32_t                                   maxSets,
		vk::DescriptorPoolCreateFlags              poolFlags,
		const std::vector<vk::DescriptorPoolSize>& poolSizes) : lveDevice{lveDevice}
	{
		vk::DescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.setPoolSizes(poolSizes);
		descriptorPoolInfo.setMaxSets(maxSets);
		descriptorPoolInfo.setFlags(poolFlags);

		try
		{
			descriptorPool = lveDevice.Device().createDescriptorPool(descriptorPoolInfo);
		}
		catch (const vk::SystemError& e)
		{
			throw std::runtime_error("failed to create descriptor pool: " + std::string(e.what()));
		}
	}


	LveDescriptorPool::~LveDescriptorPool()
	{
		lveDevice.Device().destroyDescriptorPool(descriptorPool, nullptr);
	}

	bool LveDescriptorPool::AllocateDescriptor(
		const vk::DescriptorSetLayout descriptorSetLayout, vk::DescriptorSet& descriptor) const
	{
		vk::DescriptorSetAllocateInfo allocInfo{};
		allocInfo.setDescriptorPool(descriptorPool);
		allocInfo.setPSetLayouts(&descriptorSetLayout);
		allocInfo.setDescriptorSetCount(1);

		try
		{
			descriptor = lveDevice.Device().allocateDescriptorSets(allocInfo)[0];
			return true;
		}
		catch (const vk::SystemError& e)
		{
			std::cerr << "Failed to allocate descriptor set: " << e.what() << std::endl;
			return false;
		}
	}


	void LveDescriptorPool::FreeDescriptors(std::vector<vk::DescriptorSet>& _descriptors) const
	{
		lveDevice.Device().freeDescriptorSets(
			descriptorPool,
			static_cast<uint32_t>(_descriptors.size()),
			_descriptors.data());
	}

	void LveDescriptorPool::ResetPool()
	{
		vkResetDescriptorPool(lveDevice.Device(), descriptorPool, 0);
	}

	// *************** Descriptor Writer *********************

	LveDescriptorWriter::LveDescriptorWriter(LveDescriptorSetLayout& _setLayout, LveDescriptorPool& _pool) : setLayout{
			_setLayout
		}, pool{_pool}
	{
	}

	LveDescriptorWriter& LveDescriptorWriter::WriteBuffer(
		uint32_t _binding, vk::DescriptorBufferInfo* _bufferInfo)
	{
		assert(setLayout.bindings.count(_binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = setLayout.bindings[_binding];

		assert(
			bindingDescription.descriptorCount == 1 &&
			"Binding single descriptor info, but binding expects multiple");

		vk::WriteDescriptorSet write{};
		write.sType           = vk::StructureType::eWriteDescriptorSet;
		write.descriptorType  = bindingDescription.descriptorType;
		write.dstBinding      = _binding;
		write.pBufferInfo     = _bufferInfo;
		write.descriptorCount = 1;

		writes.push_back(write);
		return *this;
	}

	LveDescriptorWriter& LveDescriptorWriter::WriteImage(
		uint32_t _binding, vk::DescriptorImageInfo* _imageInfo)
	{
		assert(setLayout.bindings.count(_binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = setLayout.bindings[_binding];

		assert(
			bindingDescription.descriptorCount == 1 &&
			"Binding single descriptor info, but binding expects multiple");

		vk::WriteDescriptorSet write{};
		write.sType           = vk::StructureType::eWriteDescriptorSet;
		write.descriptorType  = bindingDescription.descriptorType;
		write.dstBinding      = _binding;
		write.pImageInfo      = _imageInfo;
		write.descriptorCount = 1;

		writes.push_back(write);
		return *this;
	}

	bool LveDescriptorWriter::Build(vk::DescriptorSet& _set)
	{
		bool success = pool.AllocateDescriptor(setLayout.GetDescriptorSetLayout(), _set);
		if (!success) return false;
		Overwrite(_set);
		return true;
	}

	void LveDescriptorWriter::Overwrite(vk::DescriptorSet& _set)
	{
		for (auto& write : writes)
		{
			write.dstSet = _set;
		}
		pool.lveDevice.Device().updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
	}
} // namespace lve
