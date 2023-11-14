#include "CrowdDest.h"
#include "../geometry/Line2F.h"
#include "../geometry/CollisonDetection2D.h"

const float DoorSize = 0.15f;

CrowdDest::CrowdDest(float lx, float rx, float uy, float dy, const Material& mat, Pos2F exitPos, Pos2F enterPos) :
	RectRenderable(Pos2F(lx, uy), Pos2F(rx, uy), Pos2F(lx, dy), Pos2F(rx, dy), mat),
	exitPos(exitPos),
	enterPos(enterPos),
	entryNoColPoint((exitPos.x + enterPos.x)/2, (exitPos.y + enterPos.y)/2)
{

}

Pos2F CrowdDest::GetSpawnPos(std::mt19937& mt) const
{
	float w = ur.x - ul.x;
	float h = ur.y - lr.y;

	float midX = ul.x + 0.5f * w;
	float midY = ll.y + 0.5f * h;

	std::uniform_real_distribution<float> xDist(-0.5f * w, 0.5f * w);
	std::uniform_real_distribution<float> yDist(-0.5f * h, 0.5f * h);

	return Pos2F(xDist(mt) + midX, yDist(mt) + midY);
}

bool CrowdDest::IsInDest(const Pos2F& pos) const
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

bool CrowdDest::BlockPathBetween(const Pos2F& p1, const Pos2F& p2, float cushion) const
{
	Pos2F ulCushion = ul + Vec2F(-cushion, cushion);
	Pos2F urCushion = ur + Vec2F(cushion, cushion);
	Pos2F lrCushion = lr + Vec2F(cushion, -cushion);
	Pos2F llCushion = ll + Vec2F(-cushion, -cushion);

	Line2F top = Line2F(ulCushion , urCushion);
	Line2F left = Line2F(ulCushion, llCushion);
	Line2F right = Line2F(urCushion, lrCushion);
	Line2F bot = Line2F(lrCushion, llCushion);
	Line2F test = Line2F(p1, p2);

	bool result;
	Pos2F colPoint(0,0);
	
	result = CollisionBetween(top, test, colPoint);
	if (result && (colPoint - entryNoColPoint).GetMagnitude() > DoorSize) {
		return true;
	}
	result = CollisionBetween(bot, test, colPoint);
	if (result && (colPoint - entryNoColPoint).GetMagnitude() > DoorSize) {
		return true;
	}
	result = CollisionBetween(left, test, colPoint);
	if (result && (colPoint - entryNoColPoint).GetMagnitude() > DoorSize) {
		return true;
	}
	result = CollisionBetween(right, test, colPoint);
	if (result && (colPoint - entryNoColPoint).GetMagnitude() > DoorSize) {
		return true;
	}

	return false;
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
