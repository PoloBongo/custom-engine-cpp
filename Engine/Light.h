#pragma once
#include "Component.h"

class Light final : public Component
{
	public:
		/*void Start() override;
		void Physics(const float& _delta) override;
		void Update(const float& _delta) override;

		//Render Methods
		void Render(lve::LveWindow* _window) override;
		void RenderGui(lve::LveWindow* _window) override;
		void RenderBackground(lve::LveWindow* _window) override;
		*/
		float lightIntensity = 1.0f;
};

