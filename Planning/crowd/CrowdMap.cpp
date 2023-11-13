#include "CrowdMap.h"

CrowdMap::CrowdMap(const Material& mat, std::mt19937& mt) :
	floor(Pos3F(-7.0f, 7.0f, 0.0f), Pos3F(7.0f, 7.0f, 0.0f), Pos3F(-7.0f, -7.0f, 0.0f), Pos3F(7.0f, -7.0f, 0.0f), mat)
{
	// Create Destinations
	dests.reserve(5);
	Material destMat = Material(ColorRGBA(0.9f, 0.6f, 0.6f, 0.5f), 1.0f, 0.0f, 0.0f, 10.0f, -1);
	dests.emplace_back(-5.0f, 5.0f, 7.0f, 5.0f, destMat, Pos3F(0.0f, 5.2f, 0.0f), Pos3F(0.0f, 4.8f, 0.0f));

	destMat.color = ColorRGBA(0.6f, 0.9f, 0.6f, 0.6f);
	dests.emplace_back(-5.0f, 5.0f, -5.0f, -7.0f, destMat, Pos3F(0.0f, -5.2f, 0.0f), Pos3F(0.0f, -4.8f, 0.0f));

	destMat.color = ColorRGBA(0.6f, 0.6f, 0.9f, 0.6f);
	dests.emplace_back(-7.0f, -5.0f, 5.0f, -5.0f, destMat, Pos3F(-5.2f, 0.0f, 0.0f), Pos3F(-4.8f, 0.0f, 0.0f));

	destMat.color = ColorRGBA(0.9f, 0.9f, 0.3f, 0.6f);
	dests.emplace_back(5.0f, 7.0f, 5.0f, -5.0f, destMat, Pos3F(5.2f, 0.0f, 0.0f), Pos3F(4.8f, 0.0f, 0.0f));

	destMat.color = ColorRGBA(0.8f, 0.1f, 0.8f, 0.6f);
	dests.emplace_back(-2.0f, 2.0f, 2.0f, -2.0f, destMat, Pos3F(0.0f, 1.8f, 0.0f), Pos3F(0.0f, 2.2f, 0.0f));

	nodes.reserve(2 * dests.size() + 50);
	Material nodeMat = destMat;
	nodeMat.color = ColorRGBA(0.3f, 0.3f, 0.3f, 0.7f);
	for (auto& i : dests) {
		nodes.emplace_back(i.GetEnterPos(), nodeMat);
		nodes.emplace_back(i.GetExitPos(), nodeMat);
	}

	std::uniform_real_distribution<float> nodeDist(-5.0f, 5.0f);
	for (int i = 0; i < 50; i++) {
		bool intersects = false;
		Pos3F p;
		do {
			intersects = false;
			float x = nodeDist(mt);
			float y = nodeDist(mt);

			p = Pos3F(x, y, 0.0f);

			// Check that it doesn't intersect

			for (auto& d : dests) {
				if (d.IsInDest(p)) {
					intersects = true;
					break;
				}
			}
		} while (intersects);
		
		nodes.emplace_back(p, nodeMat);
	}
}

bool CrowdMap::ValidPathBetween(const Pos3F& p1, const Pos3F& p2) const
{
	return false;
}

void CrowdMap::Render(Renderer& renderer)
{
	for (auto& i : nodes) {
		renderer.Render(i);
	}
	for (auto& i : dests) {
		renderer.Render(i);
	}
	renderer.Render(floor);
}
