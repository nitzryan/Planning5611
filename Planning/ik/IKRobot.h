#pragma once

#include <vector>

#include "IKBase.h"
#include "IKArm.h"

#include "../rendering/Renderer.h"

class IKRobot
{
public:
	IKRobot();
	void MoveTowards(const std::vector<Pos2F>& points, float dt);
	bool IsAtPosition(const std::vector<Pos2F>& points, float dist) const;
	//void StepTowards(const std::vector<Pos3F>& points);

	void RenderRobot(Renderer& renderer);
	float GetMaxReach() const;
private:
	IKBase base;
	std::vector<IKArm> arms;
	std::vector<CircleRenderable> joints;
	//size_t armsIdx;
	std::vector<Pos2F> endpoints;
};


