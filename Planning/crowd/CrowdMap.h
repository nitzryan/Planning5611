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
	bool ValidPathBetween(const Pos2F& p1, const Pos2F& p2, float cushion) const;
	void Render(Renderer& renderer);
	const CrowdDest* GetRandomDest(std::mt19937& mt) const;
	const CrowdDest* GetSpawnConditions(std::mt19937& mt, Pos2F& startPos, const CrowdNode*& startNode);

	std::vector<const RectRenderable*> GetWalls() const;
private:
	std::vector<CrowdNode> nodes;
	std::vector<CrowdDest> dests;
	RectRenderable floor;

	void ConnectNodes();
};

