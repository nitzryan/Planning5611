#pragma once
#include "../rendering/PlaneRenderable.h"
#include <vector>

class IKArm :
    public PlaneRenderable
{
public:
    IKArm(const Material& mat, float length, float width, float startTheta, float thetaMin, float thetaMax);
    void SetChildren(const std::vector<IKArm*> c) { children = c; }
    void UpdateTheta(const Pos3F& E, const Pos3F& D);
    void ForwardPass(const Pos3F& newPos, float startDir);
    Pos3F GetEndPos() const { return endPos; }
    std::vector<Pos3F> GetEndpoints() const;

    void Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc) override;
private:
    float thetaMin, thetaMax;
    float length;
    float width;
    float theta;
    float startDir;
    Pos3F pos;
    Pos3F endPos;
    std::vector<IKArm*> children;
};

