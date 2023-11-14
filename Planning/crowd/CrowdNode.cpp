#include "CrowdNode.h"

int CrowdNode::NextId = 0;

CrowdNode::CrowdNode(Pos2F p, const Material& mat) :
	CircleRenderable(p, 0.05f, mat)
{
	id = CrowdNode::NextId;
	CrowdNode::NextId++;
}

float CrowdNode::GetDistFrom(const CrowdNode* node) const
{
	return (center - node->center).GetMagnitude();
}
