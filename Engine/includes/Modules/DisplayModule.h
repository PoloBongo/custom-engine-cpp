#pragma once
#include "Modules/Module.h"

class DisplayModule final : public Module
{
	void Init() override;
	void Start() override;
	void FixedUpdate() override;
	void Update() override;
	void PreRender() override;
	void Render() override;
	void RenderGui() override;
	void PostRender() override;
	void Release() override;
	void Finalize() override;
};

