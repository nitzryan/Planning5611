#include "IKArm.h"

#include <math.h>

const float MAX_ROT_SPEED = 1.5f;

bool IKArm::IgnoreJointLimits = false;

IKArm::IKArm(const Material& mat, float length, float width, float startTheta, float thetaMin, float thetaMax, int endpointIdx) :
	RectRenderable(Pos2F(0,0), Pos2F(0, 0), Pos2F(0, 0), Pos2F(0, 0), mat),
	thetaMin(thetaMin),
	thetaMax(thetaMax),
	length(length),
	width(width),
	theta(startTheta),
	startDir(0),
	endpointIdx(endpointIdx),
	pos(Pos2F(0, 0)),
	endPos(Pos2F(0, 0))
{
}

void IKArm::UpdateTheta(const std::vector<std::pair<const Pos2F&, const Pos2F&>>& eds, float dt)
{
	float deltaTheta = 0;

	for (auto& ed : eds) {
		Vec2F RE = ed.first.Subtract(pos);
		Vec2F RD = ed.second.Subtract(pos);
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

	// Cap Speed
	if (abs(deltaTheta) > MAX_ROT_SPEED * dt) {
		if (deltaTheta > 0) {
			deltaTheta = MAX_ROT_SPEED * dt;
		}
		else {
			deltaTheta = -MAX_ROT_SPEED * dt;
		}
	}

	theta += deltaTheta;

	// Joint Limits
	if (IKArm::IgnoreJointLimits) {
		return;
	}

	if (theta > thetaMax) {
		theta = thetaMax;
	}
	else if (theta < thetaMin) {
		theta = thetaMin;
	}
}

void IKArm::ForwardPassFromCurrent()
{
	ForwardPass(pos, startDir);
}

void IKArm::ForwardPass(const Pos2F& newPos, float startDir)
{
	pos = newPos;
	this->startDir = startDir;

	float endDir = startDir + theta;
	endPos = Pos2F::Add(newPos, Vec2F::Mul(Vec2F(cosf(endDir), sinf(endDir)), length));

	for (auto i : children) {
		i->ForwardPass(endPos, endDir);
	}
}

std::vector<std::pair<Pos2F, int>> IKArm::GetEndpoints() const
{
	if (children.empty()) {
		return { { endPos, endpointIdx } };
	}

	std::vector<std::pair<Pos2F, int>> endpoints;
	for (auto i : children) {
		auto childEndpoints = i->GetEndpoints();
		for (auto& j : childEndpoints) {
			endpoints.push_back(j);
		}
	}

	return endpoints;
}

void IKArm::Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc)
{
	float angle = theta + startDir;

	Vec2F w = Vec2F::Mul(Vec2F(-sinf(angle), cosf(angle)), width * 0.5f);
	Vec2F l = Vec2F::Mul(Vec2F(cosf(angle), sinf(angle)), length);

	ul = Pos2F::Add(pos, w);
	ur = Pos2F::Add(pos, Vec2F::Mul(w, -1));
	ll = Pos2F::Add(ul, l);
	lr = Pos2F::Add(ur, l);

	RectRenderable::Render(vbo, vboLoc, pointCount, ebo, eboLoc);
}
