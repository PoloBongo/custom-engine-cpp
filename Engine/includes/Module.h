#pragma once

class ModuleManager;

class Module
{
	friend class ModuleManager;

protected:
	~Module() = default;

public:
	virtual void Init() {}
	virtual void Start() {}
	virtual void FixedUpdate() {}
	virtual void Update() {}
	virtual void PreRender() {}
	virtual void Render() {}
	virtual void RenderGui() {}
	virtual void PostRender() {}
	virtual void Release() {}
	virtual void Finalize() {}

protected:
	ModuleManager* moduleManager = nullptr;
};