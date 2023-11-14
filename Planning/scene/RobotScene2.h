#pragma once

#include "IScene.h"
#include "../ik/IKRobot.h"

#include <random>

class RobotScene2 : public IScene {
public:
	RobotScene2() :
		robot({
			IKArm(matShared, 0.2f, 0.1f, 0.6f, -1.57f, 1.57f, -1),
			IKArm(matShared, 0.2f, 0.1f, 0.0f, -1.57f, 1.57f, -1),
			IKArm(matShared, 0.2f, 0.1f, -0.2f, -1.57f, 1.57f, -1),
			IKArm(matShared, 0.2f, 0.1f, 0.2f, -1.57f, 1.57f, -1),
			IKArm(mat0, 0.2f, 0.1f, 0.4f, 0.1f, 1.57f, 0),
			IKArm(mat0, 0.2f, 0.1f, 0.0f, -1.57f, 1.57f, 0),
			IKArm(mat0, 0.2f, 0.1f, 0.0f, -1.57f, 1.57f, 0),
			IKArm(mat0, 0.2f, 0.1f, 0.0f, -1.57f, 1.57f, 0),
			IKArm(mat1, 0.2f, 0.1f, -0.4f, -1.57f, -0.1f, 1),
			IKArm(mat1, 0.2f, 0.1f, 0.0f, -1.57f, 1.57f, 1),
			IKArm(mat1, 0.2f, 0.1f, 0.0f, -1.57f, 1.57f, 1),
			IKArm(mat1, 0.2f, 0.1f, 0.0f, -1.57f, 1.57f, 1),
			}, { {1}, {2}, {3}, {4,8}, {5}, {6}, {7}, {}, {9}, {10}, {11}, {} }),
		circle0(Pos2F(0.2f, -0.8f), 0.05f, Material(ColorRGBA(0.8f, 0.2f, 0.2f, 1), 1, 0, 0, 10, -1)),
		circle1(Pos2F(-0.2f, -0.8f), 0.05f, Material(ColorRGBA(0.1f, 0.2f, 0.8f, 1), 1, 0, 0, 10, -1))
	{
		mt = std::mt19937(5611);
	}
	void Update(float dt) override {
		if (robot.IsAtPosition({ circle0.GetCenter(), circle1.GetCenter() }, 0.01f)) {
			RandomizeCirclePositions();
		}

		robot.MoveTowards({ circle0.GetCenter(), circle1.GetCenter() }, dt);
	}
	void Render(Renderer& renderer) override {
		robot.RenderRobot(renderer);
		renderer.Render(circle0);
		renderer.Render(circle1);
	}

	void RandomizeCirclePositions() {
		std::uniform_real_distribution<float> r(-1.0f, 1.0f);

		float mag;
		do {
			circle0.SetCenter(Pos2F(r(mt), r(mt)));
			circle1.SetCenter(Pos2F(r(mt), r(mt)));
			mag = (circle0.GetCenter() - circle1.GetCenter()).GetMagnitude();
		} while (mag > robot.GetMaxReach());
	}
private:
	IKRobot robot;
	CircleRenderable circle0, circle1;

	std::mt19937 mt;
};