#pragma once

#include "../rendering/CircleRenderable.h"
#include <vector>

class CrowdNode :
    public CircleRenderable
{
public:
    CrowdNode(Pos2F p, const Material& mat);
    static int NextId;
    int GetId() const { return id; }
    std::vector<const CrowdNode*> GetConnectedNodes() const { return connectedNodes; }
    void ConnectNode(const CrowdNode* nodeId) { connectedNodes.push_back(nodeId); }
    float GetDistFrom(const CrowdNode* node) const;
private:
    int id;
    std::vector<const CrowdNode*> connectedNodes;
};