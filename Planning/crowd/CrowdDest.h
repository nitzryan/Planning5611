#pragma once

#include "../rendering/RectRenderable.h"
#include "CrowdNode.h"
#include <random>
#include <map>
#include "../geometry/Pos2F.h"
#include "../rendering/Renderer.h"

class CrowdDest :
    public RectRenderable
{
public:
    CrowdDest(float lx, float rx, float uy, float dy, const Material& mat, Pos2F exitPos, Pos2F enterPos);
    Pos2F GetSpawnPos(std::mt19937& mt) const;
    bool IsInDest(const Pos2F& pos) const;
    void CreateDStarHeuristic(const CrowdNode* startNode);
    const std::map<const CrowdNode*, float>& GetMap() const { return nodeHeuristics; }
    Pos2F GetExitPos() const { return exitPos; }
    Pos2F GetEnterPos() const { return enterPos; }
    bool BlockPathBetween(const Pos2F& p1, const Pos2F& p2, float cushion) const;
    
    std::vector<const RectRenderable*> GetWalls() const { return { &wall1, &wall2, &wall3, &wall4, &wall5 }; }
    void Render(Renderer& renderer);
private:
    Pos2F exitPos;
    Pos2F enterPos;
    Pos2F entryNoColPoint;
    RectRenderable wall1, wall2, wall3, wall4, wall5;
    std::map<const CrowdNode*, float> nodeHeuristics;

    void CreateDStarHeuristic(const CrowdNode* parent, const CrowdNode* child, float parentScore);
};

