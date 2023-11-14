#include "IKBase.h"
#include "../rendering/BufferWriter.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const float MAX_TRANS_SPEED = 0.5f;
const float MAX_ROT_SPEED = 0.5f;

IKBase::IKBase(const Pos2F& center, float radius, float theta, const Material& material) :
	CircleRenderable(center, radius, material),
	theta(theta)
{
}

void IKBase::Move(const Vec2F& v)
{
	center.Add(v);
	GenerateRenderPoints();
}

void IKBase::Rotate(float t)
{
	theta += t;
	if (theta < 0) {
		theta += 2 * (float)M_PI;
	}
	else if (theta > 2 * (float)M_PI) {
		theta -= 2 * (float)M_PI;
	}
}

void IKBase::UpdateOrientation(const std::vector<std::pair<const Pos2F&, const Pos2F&>>& eds, float dt)
{
	// Translate
	Vec2F totalDelta = Vec2F(0, 0);
	for (auto& i : eds) {
		totalDelta.Add(i.second - i.first);
	}

	totalDelta.Mul(1.0f / eds.size());

	if (totalDelta.GetMagnitude() > dt * MAX_TRANS_SPEED) {
		totalDelta.Normalize();
		Move(Vec2F::Mul(totalDelta, MAX_TRANS_SPEED * dt));
	}
	else { // Entire movement possible with speed constraints
		Move(totalDelta);
	}

	// Rotate
	float deltaTheta = 0;
	for (auto& ed : eds) {
		Vec2F RE = ed.first.Subtract(center);
		Vec2F RD = ed.second.Subtract(center);
		RE.Normalize();
		RD.Normalize();
		float dot = Vec2F::Dot(RE, RD);
		if (dot > 1.0f) { // Numerical error can cause acosf to return NaN
			dot = 1.0f;
		}

		float a = acosf(dot);

		float cross2D = RE.x * RD.y - RE.y * RD.x;
		if (cross2D < 0) {
			deltaTheta -= a;
		}
		else {
			deltaTheta += a;
		}
	}

	deltaTheta /= eds.size();

	if (abs(deltaTheta) > dt * MAX_ROT_SPEED) {
		if (deltaTheta > 0) {
			Rotate(dt * MAX_ROT_SPEED);
		}
		else {
			Rotate(-dt * MAX_ROT_SPEED);
		}
	}
	else {
		Rotate(deltaTheta);
	}
}

int IKBase::NumIndices() const
{
	return CircleRenderable::NumIndices() + 3;
}

int IKBase::NumPoints() const
{
	return CircleRenderable::NumPoints() + 3;
}

void IKBase::Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc)
{
	

	/*vboLoc += 12 * CircleRenderable::NumPoints();
	pointCount += CircleRenderable::NumPoints();
	eboLoc += CircleRenderable::NumIndices();*/

	// Add triangle to VBO
	ColorRGBA lookColor = ColorRGBA(0.9f, 0.9f, 0.9f, 1.0f);
	BufferWriter::AddPoint(vbo, vboLoc, center, lookColor, Vec3F(0, 0, 1), -1.0f, -1.0f);
	BufferWriter::AddPoint(vbo, vboLoc, Pos2F::Add(center, Vec2F::Mul(Vec2F(cosf(theta - 0.25f), sinf(theta - 0.25f)), radius)), lookColor, Vec3F(0, 0, 1), -1.0f, -1.0f);
	BufferWriter::AddPoint(vbo, vboLoc, Pos2F::Add(center, Vec2F::Mul(Vec2F(cosf(theta + 0.25f), sinf(theta + 0.25f)), radius)), lookColor, Vec3F(0, 0, 1), -1.0f, -1.0f);

	// Adsd triangle to EBO
	ebo[eboLoc] = pointCount;
	eboLoc++;
	pointCount++;
	ebo[eboLoc] = pointCount;
	eboLoc++;
	pointCount++;
	ebo[eboLoc] = pointCount;
	eboLoc++;
	pointCount++;

	//vboLoc += 12 * 3;

	CircleRenderable::Render(vbo, vboLoc, pointCount, ebo, eboLoc);
}
