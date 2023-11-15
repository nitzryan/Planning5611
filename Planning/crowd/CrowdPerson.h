#pragma once

#include "../rendering/CircleRenderable.h"
#include "CrowdNode.h"
#include "CrowdDest.h"
#include "CrowdMap.h"

class CrowdPerson :
    public CircleRenderable
{
public:
    CrowdPerson(Pos2F start, const Material& mat, const CrowdNode* startNode, const CrowdDest* dest, const CrowdMap* crowdMap);
    float GetDistFrom(const CrowdPerson& p) const;
    void CalculateDirection(float dt);
    void ComputeTTC(std::vector<CrowdPerson>& people, size_t startIdx, float dt);
    void Move(float dt);
    void AvoidWall(const RectRenderable& rect, float dt);
    bool ReadyToRecycle() const;
private:
    Vec2F vel;
    const CrowdDest* dest;
    const CrowdMap* crowdMap;

    std::vector<const CrowdNode*> traversalNodes;
    size_t currentNode;
};

