#include "CrowdDest.h"

CrowdDest::CrowdDest(float lx, float rx, float uy, float dy, const Material& mat, Pos3F exitPos, Pos3F enterPos) :
	RectRenderable(Pos3F(lx, uy, 0), Pos3F(rx, uy, 0), Pos3F(lx, dy, 0), Pos3F(rx, dy, 0), mat),
	exitPos(exitPos),
	enterPos(enterPos)
{
}

Pos3F CrowdDest::GetSpawnPos(std::mt19937& mt) const
{
	float w = ur.x - ul.x;
	float h = ur.y - lr.y;

	float midX = ur.x + 0.5f * w;
	float midY = ll.y + 0.5f * h;

	std::uniform_real_distribution<float> xDist(-0.5f * w, 0.5f * w);
	std::uniform_real_distribution<float> yDist(-0.5f * h, 0.5f * h);

	return Pos3F(xDist(mt) + midX, yDist(mt) + midY, 0);
}

bool CrowdDest::IsInDest(Pos3F pos) const
{
	return (pos.x > ul.x) && (pos.x < lr.x) && (pos.y > ll.y) && (pos.y < ul.y);
}

void CrowdDest::CreateDStarHeuristic(const CrowdNode* startNode)
{
	nodeHeuristics[startNode] = 0.0f;

	auto children = startNode->GetConnectedNodes();
	for (auto i : children) {
		CreateDStarHeuristic(startNode, i, 0.0f);
	}
}

void CrowdDest::CreateDStarHeuristic(const CrowdNode* parent, const CrowdNode* child, float parentScore)
{
	float dist = parent->GetDistFrom(child);
	float score = parentScore + dist;

	// Check if node has been visited
	auto it = nodeHeuristics.find(child);

	if (it != nodeHeuristics.end()) { // Exists
		if (it->second < score) { // Better path to node exists
			return; // Stop looking
		}
	}

	// Either first time node has been visited, or this path is a better one
	nodeHeuristics[child] = score;

	auto children = child->GetConnectedNodes();
	for (auto i : children) {
		if (i == parent) { // Don't go back to parent
			continue;
		}
		CreateDStarHeuristic(child, i, score);
	}
}
