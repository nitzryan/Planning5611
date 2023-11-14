#include "CrowdPerson.h"
#include <iostream>

CrowdPerson::CrowdPerson(Pos2F start, const Material& mat, const CrowdNode* startNode, const CrowdDest* dest, const CrowdMap* crowdMap) :
	CircleRenderable(start, 0.125f, mat),
	vel(0,0),
	dest(dest),
	crowdMap(crowdMap),
	currentNode(0)
{
	if (startNode == nullptr) {
		return;
	}
	// Traverse destination map to get best values
	auto map = dest->GetMap();
	traversalNodes.push_back(startNode);

	auto currentNode = startNode;
	while (!dest->IsInDest(currentNode->GetCenter())) {
		// Get child with best heuristic
		auto children = currentNode->GetConnectedNodes();
		float bestHeuristic = std::numeric_limits<float>::infinity();
		for (auto i : children) {
			float score = map[i];
			if (score < bestHeuristic) {
				bestHeuristic = score;
				currentNode = i;
			}
		}

		traversalNodes.push_back(currentNode);
	}
}

float CrowdPerson::GetDistFrom(const CrowdPerson& p) const
{
	return (center - p.center).GetMagnitude();
}

void CrowdPerson::Update(float dt, const std::vector<CrowdPerson>& people)
{
	if (ReadyToRecycle() || traversalNodes.empty()) { // In Destination
		return;
	}

	// Check to see if can move further down the list
	for (size_t i = currentNode + 1; i < traversalNodes.size(); i++) {
		if (crowdMap->ValidPathBetween(center, traversalNodes[i]->GetCenter(), radius)) {
			currentNode = i;
		}
		else {
			break;
		}
	}

	// Add stuff to avoid people later

	vel = traversalNodes[currentNode]->GetCenter() - center;
	vel.Normalize();
	vel.Mul(0.50f);

	center.Add(Vec2F::Mul(vel, dt));
	CircleRenderable::GenerateRenderPoints();
}

bool CrowdPerson::ReadyToRecycle() const
{
	return dest->PointInRect(center);
}

