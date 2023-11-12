#pragma once

#include "IScene.h"
#include "../ik/IKRobot.h"

class TestScene : public IScene {
public:
	TestScene() :
		robot(),
		sphere(Pos3F(0.6f, 0, 0), 0.05f, Material(ColorRGBA(0.2f, 0.8f, 0.1f, 1.0f), 1.f, 0, 0, 10.f, -1))
	{

	}
	void Update(float dt) override {}
	void Render(Renderer& renderer) override {
		robot.RenderRobot(renderer);
		renderer.Render(sphere);
	}
	void Step() {
		robot.StepTowards(sphere.get_center());
	}
private:
	IKRobot robot;
	SphereRenderable sphere;
};