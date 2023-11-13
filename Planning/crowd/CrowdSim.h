#pragma once

#include "CrowdMap.h"
#include <random>
#include "../rendering/Renderer.h"
#include "../scene/IScene.h"

class CrowdSim : public IScene
{
public:
	CrowdSim();
	void Render(Renderer& renderer) override;
	void Update(float dt) override {}
private:
	std::mt19937 mt;
	CrowdMap map;
};

