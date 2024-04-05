#pragma once

#include <functional>

#include "Module.h"
//#include "TimeModule.h"
#include "Managers/WindowManager.h"


class ImGuiManager final : public Module
{
	void Init() override;
	void Start() override;
	void Update() override;
	void Render() override;
	void Finalize() override;

	//TimeModule* timeModule = nullptr;

	vk::Fence _immFence;
	vk::CommandBuffer _immCommandBuffer;
	vk::CommandPool _immCommandPool;

	void immediate_submit(std::function<void(vk::CommandBuffer cmd)>&& function);

protected:
	vk::Device device;
	vk::Queue graphicsQueue;

	WindowManager* windowManager = nullptr;
	
	~ImGuiManager() = default;
};