#pragma once

#include <vector>

#include "IKBase.h"
#include "IKArm.h"

#include "../rendering/Renderer.h"

class IKRobot
{
public:
	IKRobot();
	void MoveTowards(const Pos3F& point);
	void StepTowards(const std::vector<Pos3F>& points);

	void RenderRobot(Renderer& renderer);
private:
	IKBase base;
	std::vector<IKArm> arms;
	size_t armsIdx;
	std::vector<Pos3F> endpoints;
};

