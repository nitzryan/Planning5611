#include "CrowdMap.h"

const int RAND_NODES = 50;

CrowdMap::CrowdMap(const Material& mat, std::mt19937& mt, std::vector<CrowdDest> destinations) :
	dests(destinations), floor(Pos2F(-6.25f, 6.25f), Pos2F(6.25f, 6.25f), Pos2F(-6.25f, -6.25f), Pos2F(6.25f, -6.25f), mat)
{
	// Create Destinations
	/*dests.reserve(5);
	Material destMat = Material(ColorRGBA(0.9f, 0.6f, 0.6f, 0.5f), 1.0f, 0.0f, 0.0f, 10.0f, -1);
	dests.emplace_back(-5.0f, 5.0f, 6.25f, 5.0f, destMat, Pos2F(0.0f, 5.2f), Pos2F(0.0f, 4.8f));

	destMat.color = ColorRGBA(0.6f, 0.9f, 0.6f, 0.6f);
	dests.emplace_back(-5.0f, 5.0f, -5.0f, -6.25f, destMat, Pos2F(0.0f, -5.2f), Pos2F(0.0f, -4.8f));

	destMat.color = ColorRGBA(0.6f, 0.6f, 0.9f, 0.6f);
	dests.emplace_back(-6.25f, -5.0f, 5.0f, -5.0f, destMat, Pos2F(-5.2f, 0.0f), Pos2F(-4.8f, 0.0f));

	destMat.color = ColorRGBA(0.9f, 0.9f, 0.3f, 0.6f);
	dests.emplace_back(5.0f, 6.25f, 5.0f, -5.0f, destMat, Pos2F(5.2f, 0.0f), Pos2F(4.8f, 0.0f));

	destMat.color = ColorRGBA(0.8f, 0.1f, 0.8f, 0.6f);
	dests.emplace_back(-2.0f, 2.0f, 2.0f, -2.0f, destMat, Pos2F(0.0f, 1.8f), Pos2F(0.0f, 2.2f));*/

	nodes.reserve(2 * dests.size() + RAND_NODES);
	Material nodeMat = dests[0].GetMaterial();
	nodeMat.color = ColorRGBA(0.3f, 0.3f, 0.3f, 0.7f);
	for (auto& i : dests) {
		nodes.emplace_back(i.GetEnterPos(), nodeMat);
		nodes.emplace_back(i.GetExitPos(), nodeMat);
	}

	std::uniform_real_distribution<float> nodeDist(-4.7f, 4.7f);
	for (int i = 0; i < RAND_NODES; i++) {
		bool intersects = false;
		Pos2F p(0,0);
		do {
			intersects = false;
			float x = nodeDist(mt);
			float y = nodeDist(mt);

			p = Pos2F(x, y);

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

	ConnectNodes();

	for (int i = 0; i < dests.size(); i++) {
		dests[i].CreateDStarHeuristic(&nodes[1 + 2 * i]);
	}
}

bool CrowdMap::ValidPathBetween(const Pos2F& p1, const Pos2F& p2, float cushion) const
{
	for (auto& i : dests) {
		if (i.BlockPathBetween(p1, p2, cushion)) {
			return false;
		}
	}

	return true;
}

void CrowdMap::Render(Renderer& renderer)
{
	for (auto& i : nodes) {
		renderer.Render(i);
	}

	Material lineMaterial = Material(ColorRGBA(0.1f, 0.1f, 0.1f, 0.3f), 1.0f, 0.0f, 0.0f, 10.0f, -1);
	for (auto& i : nodes) {
		auto children = i.GetConnectedNodes();
		for (auto j : children) {
			auto dir = (i.GetCenter() - j->GetCenter());
			Vec2F w = Vec2F(-dir.y, dir.x);
			w.Normalize();
			w.Mul(0.01f);
			RectRenderable r = RectRenderable(i.GetCenter() + w, i.GetCenter() - w, j->GetCenter() + w, j->GetCenter() - w, lineMaterial);
			renderer.Render(r);
		}
	}
	for (auto& i : dests) {
		i.Render(renderer);
	}
	for (auto& i : dests) {
		renderer.Render(i);
	}

	renderer.Render(floor);
}

const CrowdDest* CrowdMap::GetRandomDest(std::mt19937& mt) const
{
	std::uniform_int_distribution<int> rSeed(0, (int)(dests.size()) - 1);
	return &dests[rSeed(mt)];
}

const CrowdDest* CrowdMap::GetSpawnConditions(std::mt19937& mt, Pos2F& startPos, const CrowdNode*& startNode)
{
	std::uniform_int_distribution<int> rSeed(0, (int)(dests.size() - 1));
	int startDest = rSeed(mt);
	int endDest;
	do {
		endDest = rSeed(mt);
	} while (endDest == startDest);

	startPos = dests[startDest].GetSpawnPos(mt);
	startNode = &nodes[startDest * 2 + 1];
	return &dests[endDest];
}

std::vector<const RectRenderable*> CrowdMap::GetWalls() const
{
	std::vector<const RectRenderable*> walls;
	walls.reserve(dests.size() * 5);
	for (auto& i : dests) {
		auto destWalls = i.GetWalls();
		for (auto& j : destWalls) {
			walls.push_back(j);
		}
	}

	return walls;
}

void CrowdMap::ConnectNodes()
{
	for (size_t i = 0; i < nodes.size(); i++) {
		if (i < 2 * dests.size()) {
			if (i % 2 == 1) { // Exit positions can only connect to enter positions
				continue;
			}
		}
		for (size_t j = i + 1; j < nodes.size(); j++) {
			float dist = (nodes[i].GetCenter() - nodes[j].GetCenter()).GetMagnitude();
			if (dist < 4.0f) {
				if (ValidPathBetween(nodes[i].GetCenter(), nodes[j].GetCenter(), 0.125f)) {
					nodes[i].ConnectNode(&nodes[j]);
					nodes[j].ConnectNode(&nodes[i]);
				}
			}
		}
	}
}
