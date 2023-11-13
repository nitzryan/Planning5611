#include "RectRenderable.h"
#include "BufferWriter.h"

RectRenderable::RectRenderable(Pos3F ul, Pos3F ur, Pos3F ll, Pos3F lr, const Material& mat) :
	ul(ul), ur(ur), ll(ll), lr(lr), material(mat)
{
}

int RectRenderable::NumIndices() const
{
	return 6;
}

int RectRenderable::NumPoints() const
{
	return 4;
}

void RectRenderable::Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc)
{
	Vec3F normal = Vec3F(0, 0, 1);
	normal.Normalize();
	BufferWriter::AddPoint(vbo, vboLoc, ul, material.color, normal, 0, 0);
	BufferWriter::AddPoint(vbo, vboLoc, ur, material.color, normal, 1, 0);
	BufferWriter::AddPoint(vbo, vboLoc, ll, material.color, normal, 0, 1);
	BufferWriter::AddPoint(vbo, vboLoc, lr, material.color, normal, 1, 1);

	ebo[eboLoc] = pointCount;
	eboLoc++;
	ebo[eboLoc] = pointCount + 1;
	eboLoc++;
	ebo[eboLoc] = pointCount + 2;
	eboLoc++;
	ebo[eboLoc] = pointCount + 1;
	eboLoc++;
	ebo[eboLoc] = pointCount + 3;
	eboLoc++;
	ebo[eboLoc] = pointCount + 2;
}

bool RectRenderable::PointInRect(const Pos3F& p) const
{
	return false;
}
