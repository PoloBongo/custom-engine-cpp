#include "Modules/ImGUIModule.h"
#include "lve_renderer.h"
#include "ModuleManager.h"
#include "Modules/WindowModule.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

void ImGuiModule::ImmediateSubmit(std::function<void(vk::CommandBuffer _cmd)>&& _function) const
{
	const vk::Device device         = windowModule->GetDevice()->Device();
	const vk::Queue  graphics_queue = windowModule->GetDevice()->GraphicsQueue();

	device.resetFences(immFence);
	immCommandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);

	const vk::CommandBuffer cmd = immCommandBuffer;

	vk::CommandBufferBeginInfo cmd_begin_info{};
	cmd_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
	cmd_begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	cmd.begin(cmd_begin_info);

	_function(cmd);

	cmd.end();

	vk::CommandBufferSubmitInfo cmd_info;
	cmd_info.sType         = vk::StructureType::eCommandBufferSubmitInfo;
	cmd_info.pNext         = nullptr;
	cmd_info.commandBuffer = cmd;
	cmd_info.deviceMask    = 0;

	vk::SubmitInfo2 submitInfo{};
	submitInfo.commandBufferInfoCount = 1;
	submitInfo.pCommandBufferInfos    = &cmd_info;
	constexpr auto dispatcher         = vk::DispatchLoaderDynamic();
	// submit command buffer to the queue and execute it.
	//  _renderFence will now block until the graphic commands finish execution
	graphics_queue.submit2KHR(submitInfo, immFence, dispatcher);

	device.waitForFences(immFence, VK_TRUE, 9999999999);
}

void ImGuiModule::Init()
{
	Module::Init();

	windowModule = moduleModule->GetModule<WindowModule>();

	device                                             = windowModule->GetDevice()->Device();
	graphicsQueue                                      = windowModule->GetDevice()->GraphicsQueue();
	const lve::QueueFamilyIndices queue_family_indices = windowModule->GetDevice()->FindPhysicalQueueFamilies();

	// Création du pool de commandes
	const vk::CommandPoolCreateInfo command_pool_info(
		vk::CommandPoolCreateFlags(),       // Flags de création
		queue_family_indices.graphicsFamily // Indice de la famille de file d'attente de commandes
	);
	immCommandPool = device.createCommandPool(command_pool_info);

	// Allocation du tampon de commande pour les soumissions immédiates
	const vk::CommandBufferAllocateInfo cmd_alloc_info(
		immCommandPool,                   // Pool de commandes
		vk::CommandBufferLevel::ePrimary, // Niveau du tampon de commande
		1                                 // Nombre de tampons à allouer
	);
	immCommandBuffer = device.allocateCommandBuffers(cmd_alloc_info)[0];

	// Ajout de la fonction de suppression du pool de commandes à la file de suppression principale
	//_mainDeletionQueue.push_back([=]() {
	//	device.destroyCommandPool(_immCommandPool);
	//});
}

void ImGuiModule::Start()
{
	Module::Start();
	//timeModule = moduleModule->GetModule<TimeModule>();

	//ImGui::CreateContext();

	const vk::DescriptorPoolSize pool_sizes[] = {
		{vk::DescriptorType::eSampler, 1000},
		{vk::DescriptorType::eCombinedImageSampler, 1000},
		{vk::DescriptorType::eSampledImage, 1000},
		{vk::DescriptorType::eStorageImage, 1000},
		{vk::DescriptorType::eUniformTexelBuffer, 1000},
		{vk::DescriptorType::eStorageTexelBuffer, 1000},
		{vk::DescriptorType::eUniformBuffer, 1000},
		{vk::DescriptorType::eStorageBuffer, 1000},
		{vk::DescriptorType::eUniformBufferDynamic, 1000},
		{vk::DescriptorType::eStorageBufferDynamic, 1000},
		{vk::DescriptorType::eInputAttachment, 1000}
	};

	vk::DescriptorPoolCreateInfo pool_info = {};
	pool_info.sType                        = vk::StructureType::eDescriptorPoolCreateInfo;
	pool_info.flags                        = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	pool_info.maxSets                      = 1000;
	pool_info.poolSizeCount                = static_cast<uint32_t>(std::size(pool_sizes));
	pool_info.pPoolSizes                   = pool_sizes;

	vk::DescriptorPool im_gui_pool;
	if (windowModule->GetDevice()->Device().createDescriptorPool(&pool_info, nullptr, &im_gui_pool) !=
	    vk::Result::eSuccess)
		throw std::runtime_error("Impossible de creer la pool de imgui!");

	// 2: initialize imgui library

	// this initializes the core structures of imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// this initializes imgui for SDL
	ImGui_ImplGlfw_InitForVulkan(windowModule->GetWindow()->GetGlfwWindow(), true);

	// this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance                  = windowModule->GetDevice()->GetInstance();
	init_info.PhysicalDevice            = windowModule->GetDevice()->GetPhysicalDevice();
	init_info.Device                    = device;
	init_info.Queue                     = graphicsQueue;
	init_info.DescriptorPool            = im_gui_pool;
	init_info.MinImageCount             = 3;
	init_info.ImageCount                = 3;
	init_info.RenderPass                = windowModule->GetRenderer()->GetSwapChainRenderPass();
	//init_info.UseDynamicRendering = VK_TRUE;
	//init_info.ColorAttachmentFormat = _swapchainImageFormat;

	init_info.MSAASamples = static_cast<VkSampleCountFlagBits>(vk::SampleCountFlagBits::e1);

	ImGui_ImplVulkan_Init(&init_info);

	// execute a gpu command to upload imgui font textures
	//immediate_submit([&](vk::CommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(); });

	// clear font textures from cpu data
	//ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void ImGuiModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void ImGuiModule::Update()
{
	Module::Update();

	//ImGui_ImplGlfw_ProcessEvent(&e);

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	//imgui commands
}

void ImGuiModule::PreRender()
{
	Module::PreRender();
}

void ImGuiModule::Render()
{
	Module::Render();

	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), windowModule->GetRenderer()->GetCurrentCommandBuffer());
}

void ImGuiModule::RenderGui()
{
	Module::RenderGui();
}

void ImGuiModule::PostRender()
{
	Module::PostRender();
}

void ImGuiModule::Release()
{
	Module::Release();
}

void ImGuiModule::Finalize()
{
	Module::Finalize();


	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
}


void ImGuiModule::GetGui()
{
	ImGui::Begin("Scene");

	if (ImGui::Button("New GameObject")) std::cout << "Pressed Create GameObject." << std::endl;

	ImGui::End();
}
