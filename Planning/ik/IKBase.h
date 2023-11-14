#pragma once
#include "../rendering/CircleRenderable.h"

class IKBase :
    public CircleRenderable
{
public:
    IKBase(const Pos2F& center, float radius, float theta, const Material& material);
    void Move(const Vec2F& v);
    void Rotate(float theta);
    void UpdateOrientation(const std::vector<std::pair<const Pos2F&, const Pos2F&>>& eds, float theta);
    float GetTheta() const { return theta; }

    // Rendering Overrides
    int NumIndices() const override;
    int NumPoints() const override;
    void Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc) override;
private:
    float theta;
};

