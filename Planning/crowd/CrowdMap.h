#pragma once

#include <vector>
#include "CrowdDest.h"
#include "CrowdNode.h"

#include "../rendering/RectRenderable.h"
#include "../rendering/Renderer.h"
#include <random>

class CrowdMap
{
public:
	CrowdMap(const Material& mat, std::mt19937& mt);
	bool ValidPathBetween(const Pos3F& p1, const Pos3F& p2) const;
	void Render(Renderer& renderer);
private:
	std::vector<CrowdNode> nodes;
	std::vector<CrowdDest> dests;
	RectRenderable floor;
};

