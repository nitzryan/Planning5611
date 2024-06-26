#pragma once

#include "../crowd/CrowdMap.h"
#include <random>
#include "../rendering/Renderer.h"
#include "IScene.h"
#include "../crowd/CrowdPerson.h"

class CrowdScene5 : public IScene
{
public:
	CrowdScene5();
	void Render(Renderer& renderer) override;
	void Update(float dt) override;
private:
	std::mt19937 mt;
	CrowdMap map;
	std::vector<CrowdPerson> people;
};

