#include "CrowdDest.h"
#include "../geometry/Line2F.h"
#include "../geometry/CollisonDetection2D.h"

const float DOOR_SIZE = 0.50f;
const Material wallMaterial = Material(ColorRGBA(0.2f, 0.2f, 0.2f, 1.0f), 1.0f, 0.0f, 0.0f, 10.0f, -1);

CrowdDest::CrowdDest(float lx, float rx, float uy, float dy, const Material& mat, Pos2F exitPos, Pos2F enterPos) :
	RectRenderable(Pos2F(lx, uy), Pos2F(rx, uy), Pos2F(lx, dy), Pos2F(rx, dy), mat),
	exitPos(exitPos),
	enterPos(enterPos),
	entryNoColPoint((exitPos.x + enterPos.x)/2, (exitPos.y + enterPos.y)/2),
	wall1(exitPos, exitPos, exitPos, exitPos, wallMaterial),
	wall2(exitPos, exitPos, exitPos, exitPos, wallMaterial),
	wall3(exitPos, exitPos, exitPos, exitPos, wallMaterial),
	wall4(exitPos, exitPos, exitPos, exitPos, wallMaterial),
	wall5(exitPos, exitPos, exitPos, exitPos, wallMaterial)
{
	float delta = 0.05f;

	// Determine which wall the entry/exit is on
	if (enterPos.y > uy) { // Top
		wall1 = RectRenderable(Pos2F(lx, uy), entryNoColPoint + Vec2F(-DOOR_SIZE, 0), Pos2F(lx, uy - delta), entryNoColPoint + Vec2F(-DOOR_SIZE, -0.05f), wallMaterial);
		wall2 = RectRenderable(entryNoColPoint + Vec2F(DOOR_SIZE, 0), Pos2F(rx, uy), entryNoColPoint + Vec2F(DOOR_SIZE, -0.05f), Pos2F(rx, uy - delta), wallMaterial);
		wall3 = RectRenderable(Pos2F(lx, uy), Pos2F(lx + delta, uy), Pos2F(lx, dy), Pos2F(lx + delta, dy), wallMaterial); // Left
		wall4 = RectRenderable(Pos2F(lx, dy + delta), Pos2F(rx, dy + delta), Pos2F(lx, dy), Pos2F(rx, dy), wallMaterial); // Bot
		wall5 = RectRenderable(Pos2F(rx - delta, uy), Pos2F(rx, uy), Pos2F(rx - delta, dy), Pos2F(rx, dy), wallMaterial); // Right
	}
	else if (enterPos.x > rx) { // Right
		wall1 = RectRenderable(Pos2F(rx - delta, uy), Pos2F(rx, uy), entryNoColPoint + Vec2F(-delta, DOOR_SIZE), entryNoColPoint + Vec2F(0, DOOR_SIZE), wallMaterial);
		wall2 = RectRenderable(entryNoColPoint + Vec2F(-delta, -DOOR_SIZE), entryNoColPoint + Vec2F(0, -DOOR_SIZE), Pos2F(rx - delta, dy), Pos2F(rx, dy), wallMaterial);
		wall3 = RectRenderable(Pos2F(lx, uy), Pos2F(lx + delta, uy), Pos2F(lx, dy), Pos2F(lx + delta, dy), wallMaterial); // Left
		wall4 = RectRenderable(Pos2F(lx, dy + delta), Pos2F(rx, dy + delta), Pos2F(lx, dy), Pos2F(rx, dy), wallMaterial); // Bot
		wall5 = RectRenderable(Pos2F(lx, uy), Pos2F(rx, uy), Pos2F(lx, uy - delta), Pos2F(rx, uy - delta), wallMaterial); // Top
	}
	else if (enterPos.x < lx) { // Left
		wall1 = RectRenderable(Pos2F(lx, uy), Pos2F(lx + delta, uy), entryNoColPoint + Vec2F(0, DOOR_SIZE), entryNoColPoint + Vec2F(delta, DOOR_SIZE), wallMaterial);
		wall2 = RectRenderable(entryNoColPoint + Vec2F(0, -DOOR_SIZE), entryNoColPoint + Vec2F(delta, -DOOR_SIZE), Pos2F(lx, dy), Pos2F(lx + delta, dy), wallMaterial);
		wall3 = RectRenderable(Pos2F(lx, uy), Pos2F(rx, uy), Pos2F(lx, uy - delta), Pos2F(rx, uy - delta), wallMaterial); // Top
		wall4 = RectRenderable(Pos2F(lx, dy + delta), Pos2F(rx, dy + delta), Pos2F(lx, dy), Pos2F(rx, dy), wallMaterial); // Bot
		wall5 = RectRenderable(Pos2F(rx - delta, uy), Pos2F(rx, uy), Pos2F(rx - delta, dy), Pos2F(rx, dy), wallMaterial); // Right
	}
	else if (enterPos.y < dy) { // Bot
		wall1 = RectRenderable(Pos2F(lx, dy + delta), entryNoColPoint + Vec2F(-DOOR_SIZE, delta), Pos2F(lx, dy), entryNoColPoint + Vec2F(-DOOR_SIZE, 0), wallMaterial);
		wall2 = RectRenderable(entryNoColPoint + Vec2F(DOOR_SIZE, delta), Pos2F(rx, dy + delta), entryNoColPoint + Vec2F(DOOR_SIZE, 0), Pos2F(rx, dy), wallMaterial);
		wall3 = RectRenderable(Pos2F(lx, uy), Pos2F(lx + delta, uy), Pos2F(lx, dy), Pos2F(lx + delta, dy), wallMaterial); // Left
		wall4 = RectRenderable(Pos2F(lx, uy), Pos2F(rx, uy), Pos2F(lx, uy - delta), Pos2F(rx, uy - delta), wallMaterial); // Top
		wall5 = RectRenderable(Pos2F(rx - delta, uy), Pos2F(rx, uy), Pos2F(rx - delta, dy), Pos2F(rx, dy), wallMaterial); // Right
	}
}

Pos2F CrowdDest::GetSpawnPos(std::mt19937& mt) const
{
	float w = ur.x - ul.x;
	float h = ur.y - lr.y;

	float midX = ul.x + 0.5f * w;
	float midY = ll.y + 0.5f * h;

	std::uniform_real_distribution<float> xDist(-0.4f * w, 0.4f * w);
	std::uniform_real_distribution<float> yDist(-0.4f * h, 0.4f * h);

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
	if (result && (colPoint - entryNoColPoint).GetMagnitude() > DOOR_SIZE) {
		return true;
	}
	result = CollisionBetween(bot, test, colPoint);
	if (result && (colPoint - entryNoColPoint).GetMagnitude() > DOOR_SIZE) {
		return true;
	}
	result = CollisionBetween(left, test, colPoint);
	if (result && (colPoint - entryNoColPoint).GetMagnitude() > DOOR_SIZE) {
		return true;
	}
	result = CollisionBetween(right, test, colPoint);
	if (result && (colPoint - entryNoColPoint).GetMagnitude() > DOOR_SIZE) {
		return true;
	}

	return false;
}

void CrowdDest::Render(Renderer& renderer)
{
	renderer.Render(wall1);
	renderer.Render(wall2);
	renderer.Render(wall3);
	renderer.Render(wall4);
	renderer.Render(wall5);
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
