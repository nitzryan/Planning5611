#pragma once
#include "../rendering/RectRenderable.h"
#include "../geometry/Pos2F.h"
#include <vector>

class IKArm :
    public RectRenderable
{
public:
    IKArm(const Material& mat, float length, float width, float startTheta, float thetaMin, float thetaMax, int endpointIdx);
    void SetChildren(const std::vector<IKArm*> c) { children = c; }
    void UpdateTheta(const std::vector<std::pair<const Pos2F&, const Pos2F&>>& eds, float dt);
    void ForwardPassFromCurrent();
    void ForwardPass(const Pos2F& newPos, float startDir);
    Pos2F GetEndPos() const { return endPos; }
    Pos2F GetBasePos() const { return pos; }
    std::vector<std::pair<Pos2F, int>> GetEndpoints() const;
    int GetEndpointIdx() const { return endpointIdx; }
    float GetLength() const { return length; }

    void Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc) override;
    static bool IgnoreJointLimits;
private:
    float thetaMin, thetaMax;
    float length;
    float width;
    float theta;
    float startDir;
    int endpointIdx;
    Pos2F pos;
    Pos2F endPos;
    std::vector<IKArm*> children;
};

