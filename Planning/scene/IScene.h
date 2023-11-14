#pragma once

#include <vector>
#include "../rendering/Renderer.h"

const Material matShared = Material(ColorRGBA(0.6f, 0.6f, 0.6f, 1), 1, 0, 0, 10, -1);
const Material mat0 = Material(ColorRGBA(0.8f, 0.2f, 0.2f, 1), 1, 0, 0, 10, -1);
const Material mat1 = Material(ColorRGBA(0.1f, 0.2f, 0.8f, 1), 1, 0, 0, 10, -1);

class IScene {
public:
	virtual void Update(float dt) = 0;
	virtual void Render(Renderer& renderer) = 0;
};