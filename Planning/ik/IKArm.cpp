#include "IKArm.h"

#include <math.h>

const float MAX_ROT_SPEED = 0.6f;

bool IKArm::IgnoreJointLimits = false;

IKArm::IKArm(const Material& mat, float length, float width, float startTheta, float thetaMin, float thetaMax, int endpointIdx) :
	PlaneRenderable(Pos3F(0,0,0), Pos3F(0, 0, 0), Pos3F(0, 0, 0), Pos3F(0, 0, 0), mat),
	thetaMin(thetaMin),
	thetaMax(thetaMax),
	length(length),
	width(width),
	theta(startTheta),
	startDir(0),
	endpointIdx(endpointIdx),
	pos(Pos3F(0, 0, 0)),
	endPos(Pos3F(0, 0, 0))
{
}

void IKArm::UpdateTheta(const std::vector<std::pair<const Pos3F&, const Pos3F&>>& eds, float dt)
{
	float deltaTheta = 0;

	for (auto& ed : eds) {
		Vec3F RE = ed.first.Subtract(pos);
		Vec3F RD = ed.second.Subtract(pos);
		RE.Normalize();
		RD.Normalize();
		float dot = Vec3F::Dot(RE, RD);
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

void IKArm::ForwardPass(const Pos3F& newPos, float startDir)
{
	pos = newPos;
	this->startDir = startDir;

	float endDir = startDir + theta;
	endPos = Pos3F::Add(newPos, Vec3F::Mul(Vec3F(cosf(endDir), sinf(endDir), 0), length));

	for (auto i : children) {
		i->ForwardPass(endPos, endDir);
	}
}

std::vector<std::pair<Pos3F, int>> IKArm::GetEndpoints() const
{
	if (children.empty()) {
		return { { endPos, endpointIdx } };
	}

	std::vector<std::pair<Pos3F, int>> endpoints;
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

	Vec3F w = Vec3F::Mul(Vec3F(-sinf(angle), cosf(angle), 0), width * 0.5f);
	Vec3F l = Vec3F::Mul(Vec3F(cosf(angle), sinf(angle), 0), length);

	p1 = Pos3F::Add(pos, w);
	p2 = Pos3F::Add(pos, Vec3F::Mul(w, -1));
	p3 = Pos3F::Add(p1, l);
	p4 = Pos3F::Add(p2, l);

	PlaneRenderable::Render(vbo, vboLoc, pointCount, ebo, eboLoc);
}
