#include "CrowdPerson.h"

CrowdPerson::CrowdPerson(Pos3F start, const Material& mat, const CrowdNode* startNode, const CrowdDest& dest, const CrowdMap& crowdMap) :
	CircleRenderable(start, 0.05f, mat),
	vel(0,0,0),
	dest(dest),
	crowdMap(crowdMap),
	currentNode(0)
{
	// Traverse destination map to get best values
	auto map = dest.GetMap();
	traversalNodes.push_back(startNode);

	auto currentNode = startNode;
	while (!dest.IsInDest(currentNode->GetCenter())) {
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
	if (ReadyToRecycle()) { // In Destination
		return;
	}

	// Check to see if can move further down the list
	for (size_t i = currentNode + 1; i < traversalNodes.size(); i++) {
		if (crowdMap.ValidPathBetween(center, traversalNodes[i]->GetCenter())) {
			currentNode = i;
		}
		else {
			break;
		}
	}

	// Add stuff to avoid people later

	vel = center - traversalNodes[currentNode]->GetCenter();
	vel.Normalize();
	vel.Mul(0.25f);

	center.Add(Vec3F::Mul(vel, dt));
	CircleRenderable::GenerateRenderPoints();
}

bool CrowdPerson::ReadyToRecycle() const
{
	return dest.PointInRect(center);
}
