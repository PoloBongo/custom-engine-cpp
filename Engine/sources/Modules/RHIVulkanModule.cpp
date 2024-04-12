#include "Modules/RHIVulkanModule.h"

#include "Transform.h"
#include "ModuleManager.h"
#include "GameObject/PreGameObject/CubeGameObject.h"
#include "GameObject/PreGameObject/LightGameObject.h"
#include "GameObject/PreGameObject/PlaneGameObject.h"
#include "Modules/TimeModule.h"
#include "Modules/WindowModule.h"
#include "Scene/SceneManager.h"

RHIVulkanModule::RHIVulkanModule()
{
}

RHIVulkanModule::~RHIVulkanModule()
{
	Release();
}
void RHIVulkanModule::Init()
{
	p_lveWindow = moduleManager->GetModule<WindowModule>()->GetWindow();
	p_lveDevice = new lve::LveDevice{ *p_lveWindow };
	p_lveRenderer = new lve::LveRenderer{ *p_lveWindow, *p_lveDevice };
}

void RHIVulkanModule::Start()
{
	builder = new lve::LveDescriptorPool::Builder{ *p_lveDevice };
	builder->SetMaxSets(6)//lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT)
		.AddPoolSize(vk::DescriptorType::eUniformBuffer, lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT)
		.AddPoolSize(vk::DescriptorType::eCombinedImageSampler, lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	globalPool = builder->Build();

	camera = new lve::LveCamera{};
	uboBuffers.resize(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	for (int i = 0; i < uboBuffers.size(); i++)
	{
		uboBuffers[i] = std::make_unique<lve::LveBuffer>(
			*p_lveDevice,
			sizeof(lve::GlobalUbo),
			1,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible);

		uboBuffers[i]->Map();
	}

	const auto global_set_layout = lve::LveDescriptorSetLayout::Builder(*p_lveDevice)
		.AddBinding(0, vk::DescriptorType::eUniformBuffer,
			vk::ShaderStageFlagBits::eAllGraphics)
		.AddBinding(1, vk::DescriptorType::eCombinedImageSampler,
			vk::ShaderStageFlagBits::eFragment)
		.Build();

	texture1 = new lve::LveTexture(*p_lveDevice, "../Textures/coconut.jpg");
	texture2 = new lve::LveTexture(*p_lveDevice, "../Textures/meme.png");
	texture3 = new lve::LveTexture(*p_lveDevice, "../Textures/viking_room.png");
	

	vk::DescriptorImageInfo imageInfo{};
	imageInfo.sampler = texture1->getSampler();
	imageInfo.imageView = texture1->getImageView();
	imageInfo.imageLayout = texture1->getImageLayout();

	vk::DescriptorImageInfo imageInfo2{};
	imageInfo2.sampler = texture2->getSampler();
	imageInfo2.imageView = texture2->getImageView();
	imageInfo2.imageLayout = texture2->getImageLayout();

	vk::DescriptorImageInfo imageInfo3{};
	imageInfo3.sampler = texture3->getSampler();
	imageInfo3.imageView = texture3->getImageView();
	imageInfo3.imageLayout = texture3->getImageLayout();

	globalDescriptorSets.resize(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < globalDescriptorSets.size(); i++)
	{
		auto buffer_info = uboBuffers[i]->DescriptorInfo();
		lve::LveDescriptorWriter(*global_set_layout, *globalPool)
			.WriteBuffer(0, &buffer_info)
			.WriteImage(1, &imageInfo)
			.Build(globalDescriptorSets[i]);
	}

	tex1DescriptorSets.resize(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < tex1DescriptorSets.size(); i++)
	{
		auto buffer_info = uboBuffers[i]->DescriptorInfo();
		lve::LveDescriptorWriter(*global_set_layout, *globalPool)
			.WriteBuffer(0, &buffer_info)
			.WriteImage(1, &imageInfo2)
			.Build(tex1DescriptorSets[i]);
	}

	tex2DescriptorSets.resize(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < tex2DescriptorSets.size(); i++)
	{
		auto buffer_info = uboBuffers[i]->DescriptorInfo();
		lve::LveDescriptorWriter(*global_set_layout, *globalPool)
			.WriteBuffer(0, &buffer_info)
			.WriteImage(1, &imageInfo3)
			.Build(tex2DescriptorSets[i]);
	}


	simpleRenderSystem = new lve::SimpleRenderSystem{
		*p_lveDevice, p_lveRenderer->GetSwapChainRenderPass(), global_set_layout->GetDescriptorSetLayout()
	};
	pointLightSystem = new lve::PointLightSystem{
		*p_lveDevice, p_lveRenderer->GetSwapChainRenderPass(), global_set_layout->GetDescriptorSetLayout()
	};

	viewerObject = new GameObject(GameObject::CreateGameObject());
	viewerObject->GetTransform()->SetPosition(glm::vec3{ 0.f,0.f,-2.5f });

}

void RHIVulkanModule::FixedUpdate()
{
}

void RHIVulkanModule::Update()
{
	gameObjects = moduleManager->GetModule<SceneManager>()->GetCurrentScene()->GetAllGameObject();

	cameraController.MoveInPlaneXZ(p_lveWindow->GetGlfwWindow(), TimeModule::GetDeltaTime(), *viewerObject);
	camera->SetViewYXZ(viewerObject->GetPosition(), viewerObject->GetRotation());

	const float aspect = p_lveRenderer->GetAspectRatio();
	camera->SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

	ubo.projection = camera->GetProjection();
	ubo.view = camera->GetView();

	pointLightSystem->Update(gameObjects, ubo);
}

void RHIVulkanModule::PreRender()
{
	currentCommandBuffer.reset(new vk::CommandBuffer(p_lveRenderer->BeginFrame()));
	if (currentCommandBuffer)
	{
		frameIndex = p_lveRenderer->GetFrameIndex();
		const float frame_time = TimeModule::GetDeltaTime();
		// update

		uboBuffers[frameIndex]->WriteToBuffer(&ubo);
		uboBuffers[frameIndex]->Flush();
		p_lveRenderer->BeginSwapChainRenderPass(*currentCommandBuffer);
	}
	
}

void RHIVulkanModule::Render()
{
	simpleRenderSystem->RenderGameObjects(gameObjects, *camera, *currentCommandBuffer, globalDescriptorSets[frameIndex], tex1DescriptorSets[frameIndex], tex2DescriptorSets[frameIndex]);      //render shadow casting objects
	pointLightSystem->Render(gameObjects, *camera, *currentCommandBuffer, globalDescriptorSets[frameIndex]);                 //render shadow casting objects
}

void RHIVulkanModule::RenderGui()
{
}

void RHIVulkanModule::PostRender()
{
	p_lveRenderer->EndSwapChainRenderPass(*currentCommandBuffer);
	p_lveRenderer->EndFrame(currentCommandBuffer.get());
}

void RHIVulkanModule::Release()
{
}

void RHIVulkanModule::Finalize()
{
}
