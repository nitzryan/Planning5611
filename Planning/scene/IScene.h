#pragma once

#include <vector>
#include "../rendering/Renderer.h"

class IScene {
public:
	virtual void Update(float dt) = 0;
	virtual void Render(Renderer& renderer) const = 0;
};