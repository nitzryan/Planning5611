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
    void Update(float dt, const std::vector<CrowdPerson>& people);
    bool ReadyToRecycle() const;
private:
    Vec2F vel;
    const CrowdDest* dest;
    const CrowdMap* crowdMap;

    std::vector<const CrowdNode*> traversalNodes;
    size_t currentNode;
};

