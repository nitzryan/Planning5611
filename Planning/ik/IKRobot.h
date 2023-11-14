#pragma once

#include <vector>

#include "IKBase.h"
#include "IKArm.h"

#include "../rendering/Renderer.h"

class IKRobot
{
public:
	IKRobot(std::vector<IKArm> arms, std::vector<std::vector<int>> armChildren);
	void MoveTowards(const std::vector<Pos2F>& points, float dt);
	bool IsAtPosition(const std::vector<Pos2F>& points, float dist) const;

	void RenderRobot(Renderer& renderer);
	float GetMaxReach() const;
private:
	IKBase base;
	std::vector<IKArm> arms;
	std::vector<CircleRenderable> joints;
	std::vector<Pos2F> endpoints;
};


