#pragma once
#include "../rendering/PlaneRenderable.h"
#include <vector>

class IKArm :
    public PlaneRenderable
{
public:
    IKArm(const Material& mat, float length, float width, float startTheta, float thetaMin, float thetaMax, int endpointIdx);
    void SetChildren(const std::vector<IKArm*> c) { children = c; }
    void UpdateTheta(const std::vector<std::pair<const Pos3F&, const Pos3F&>>& EDs);
    void ForwardPass(const Pos3F& newPos, float startDir);
    Pos3F GetEndPos() const { return endPos; }
    std::vector<std::pair<Pos3F, int>> GetEndpoints() const;

    void Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc) override;
private:
    float thetaMin, thetaMax;
    float length;
    float width;
    float theta;
    float startDir;
    int endpointIdx;
    Pos3F pos;
    Pos3F endPos;
    std::vector<IKArm*> children;
};

