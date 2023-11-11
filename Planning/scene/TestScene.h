#pragma once

#include "IScene.h"
#include "../rendering/SphereRenderable.h"

class TestScene : public IScene {
public:
	TestScene() :
		sphere(Pos3F(0,0,0), .5)
	{

	}
	void Update(float dt) override {}
	void Render(Renderer& renderer) const override {
		renderer.Render(sphere);
	}
private:
	SphereRenderable sphere;
};