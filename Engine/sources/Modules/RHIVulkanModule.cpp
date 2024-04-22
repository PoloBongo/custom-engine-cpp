#include "Modules/RHIVulkanModule.h"

#include "ModuleManager.h"
#include "Transform.h"
#include "GameObject/PreGameObject/CubeGameObject.h"
#include "GameObject/PreGameObject/LightGameObject.h"
#include "GameObject/PreGameObject/PlaneGameObject.h"
#include "Modules/TimeModule.h"
#include "Scene/SceneManager.h"

RHIVulkanModule::RHIVulkanModule()
{
}

RHIVulkanModule::~RHIVulkanModule()
{
	Release();
}
void RHIVulkanModule:: Init()
{
	windowModule = moduleManager->GetModule<WindowModule>();
	p_lveDevice = new lve::LveDevice{ windowModule };
	p_lveRenderer = new lve::LveRenderer{ windowModule, *p_lveDevice };
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
	texture2 = new lve::LveTexture(*p_lveDevice, "../Textures/unnamed.png");
	texture3 = new lve::LveTexture(*p_lveDevice, "../Textures/viking_room.png");
	texture4 = new lve::LveTexture(*p_lveDevice, "../Textures/grass.jpg");
	

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

	vk::DescriptorImageInfo imageInfo4{};
	imageInfo4.sampler = texture4->getSampler();
	imageInfo4.imageView = texture4->getImageView();
	imageInfo4.imageLayout = texture4->getImageLayout();

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

	ListTextures.push_back(imageInfo);
	ListTextures.push_back(imageInfo2);
	ListTextures.push_back(imageInfo3);
	ListTextures.push_back(imageInfo4);

	tex3DescriptorSets.resize(lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	

	ListDescriptors.push_back(&globalDescriptorSets);
	ListDescriptors.push_back(&tex1DescriptorSets);
	ListDescriptors.push_back(&tex2DescriptorSets);
	ListDescriptors.push_back(&tex3DescriptorSets);

	ListDescriptors.push_back(new std::vector<vk::DescriptorSet>);
	ListDescriptors.back()->resize(2);

	// **********************************************************
	// POUR RESIZE LA POOL (recopie tout pour en faire un autre)

	builder = new lve::LveDescriptorPool::Builder{ *p_lveDevice };
	builder->SetMaxSets(ListDescriptors.size() * 2)//lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT)
		.AddPoolSize(vk::DescriptorType::eUniformBuffer, lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT)
		.AddPoolSize(vk::DescriptorType::eCombinedImageSampler, lve::LveSwapChain::MAX_FRAMES_IN_FLIGHT);

	globalPool = builder->Build();
	int i = 0;
	for (std::vector<vk::DescriptorSet>* descriptorSet : ListDescriptors)
	{
		for (size_t j = 0; j < descriptorSet->size(); j++) 
		{
			int tex = i;
			if (i > ListTextures.size()-1) {
				tex = 0;
			}

			// Bug sur le 4 eme descriptorSet
			auto buffer_info = uboBuffers[j]->DescriptorInfo();
			lve::LveDescriptorWriter(*global_set_layout, *globalPool)
				.WriteBuffer(0, &buffer_info)
				.WriteImage(1, &ListTextures[tex])
				.Build((*descriptorSet)[j]);
		}
		i++;
	}
	// ****************************************************
	

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

	cameraController.MoveInPlaneXZ(windowModule->GetGlfwWindow(), TimeModule::GetDeltaTime(), *viewerObject);
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
	simpleRenderSystem->RenderGameObjects(gameObjects, *camera, *currentCommandBuffer, &ListDescriptors, frameIndex);      //render shadow casting objects
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
