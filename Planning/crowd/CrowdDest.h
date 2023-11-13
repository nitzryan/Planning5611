#pragma once

#include "../rendering/RectRenderable.h"
#include "CrowdNode.h"
#include <random>
#include <map>

class CrowdDest :
    public RectRenderable
{
public:
    CrowdDest(float lx, float rx, float uy, float dy, const Material& mat, Pos3F exitPos, Pos3F enterPos);
    Pos3F GetSpawnPos(std::mt19937& mt) const;
    bool IsInDest(Pos3F pos) const;
    void CreateDStarHeuristic(const CrowdNode* startNode);
    const std::map<const CrowdNode*, float>& GetMap() const { return nodeHeuristics; }
    Pos3F GetExitPos() const { return exitPos; }
    Pos3F GetEnterPos() const { return enterPos; }
private:
    Pos3F exitPos;
    Pos3F enterPos;
    std::map<const CrowdNode*, float> nodeHeuristics;

    void CreateDStarHeuristic(const CrowdNode* parent, const CrowdNode* child, float parentScore);
};

