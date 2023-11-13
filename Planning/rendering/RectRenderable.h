#pragma once
#include "IRenderable.h"
#include "../geometry/Pos3F.h"

class RectRenderable :
    public IRenderable
{
public:
	RectRenderable(Pos3F ul, Pos3F ur, Pos3F ll, Pos3F lr, const Material& mat);
	int NumIndices() const override;
	int NumPoints() const override;
	void Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc) override;
	bool PointInRect(const Pos3F& p) const;
	Material GetMaterial() const override { return material; }
protected:
	Material material;
	Pos3F ul, ur, ll, lr;
};

