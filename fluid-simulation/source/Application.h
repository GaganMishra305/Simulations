#pragma once

#include <vector>
#include "Mouse.h"
#include "Renderer.h"
#include "Fluid.h"

struct Application 
{
private:
	Renderer* renderer = nullptr;
	Mouse* mouse = nullptr;
	Fluid* fluid = nullptr;

	bool isRunning = false;

	Uint32 lastUpdateTime;
public:
	Application() = default;
	~Application() = default;
	
	bool IsRunning() const;

	void Setup(int gridWidth, int gridHeight, int cellSize);
	void Input();
	void Update();
	void Render() const;
	void Destroy();
};