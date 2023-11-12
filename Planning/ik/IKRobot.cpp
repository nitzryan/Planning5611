#include "IKRobot.h"

IKRobot::IKRobot() :
	base(Pos3F(0,0,0), 0.1f, 0.0f, Material(ColorRGBA(0.2f, 0.2f, 0.2f, 1), 1, 0, 0, 10, -1))
{
	Material matShared = Material(ColorRGBA(0.6f, 0.6f, 0.6f, 1), 1, 0, 0, 10, -1);
	Material mat0 = Material(ColorRGBA(0.8f, 0.2f, 0.2f, 1), 1, 0, 0, 10, -1);
	Material mat1 = Material(ColorRGBA(0.1f, 0.2f, 0.8f, 1), 1, 0, 0, 10, -1);
	arms.emplace_back(matShared, 0.4f, 0.1f, 0.6f, -1.57f, 1.57f, -1);
	arms.emplace_back(matShared, 0.4f, 0.1f, 0.0f, -1.57f, 1.57f, -1);
	arms.emplace_back(mat0, 0.4f, 0.1f, 0.4f, 0.1f, 1.57f, 0);
	arms.emplace_back(mat1, 0.4f, 0.1f, -0.4f, -1.57f, -0.1f, 1);
	arms.emplace_back(mat0, 0.5f, 0.1f, 0.0f, -1.57f, 1.57f, 0);
	arms.emplace_back(mat1, 0.2f, 0.1f, 0.0f, -1.57f, 1.57f, 1);
	arms.emplace_back(mat1, 0.3f, 0.1f, 0.0f, -1.57f, 1.57f, 1);

	//armsIdx = arms.size() - 1;

	arms[0].SetChildren({ &arms[1] });
	arms[1].SetChildren({ &arms[2], &arms[3] });
	arms[2].SetChildren({ &arms[4] });
	arms[3].SetChildren({ &arms[5] });
	arms[5].SetChildren({ &arms[6] });
	arms[0].ForwardPass(base.GetCenter(), 0);

	// Create joints to render
	joints.reserve(arms.size() - 1);
	Material jointMat = Material(ColorRGBA(0.2f, 0.2f, 0.2f, 1.0f), 1.0f, 0.0f, 0.0f, 10.0f, -1);
	for (size_t i = 1; i < arms.size(); i++) {
		joints.emplace_back(arms[i].GetBasePos(), 0.025f, jointMat);
		joints.back().SetThetaSlices(10);
	}
}

void IKRobot::MoveTowards(const std::vector<Pos3F>& points, float dt)
{
	std::vector<std::pair<Pos3F, int>> endpoints;
	std::vector<std::pair<const Pos3F&, const Pos3F&>> eds;
	for (int i = (int)arms.size() - 1; i >= 0; i--) {
		endpoints = arms[i].GetEndpoints();
		eds.clear();
		eds.reserve(endpoints.size());

		for (auto& i : endpoints) {
			eds.push_back({ i.first, points[i.second] });
		}

		arms[i].UpdateTheta(eds, dt);
		arms[i].ForwardPassFromCurrent();
	}

	// endpoints/eds will be for arms[0]
	base.UpdateOrientation(eds, dt);

	arms[0].ForwardPass(base.GetCenter(), 0);
}

bool IKRobot::IsAtPosition(const std::vector<Pos3F>& points, float dist) const
{
	auto endpoints = arms[0].GetEndpoints();
	for (size_t i = 0; i < endpoints.size(); i++) {
		if ((points[i] - endpoints[i].first).GetMagnitude() > dist) {
			return false;
		}
	}

	return true;
}

//void IKRobot::StepTowards(const std::vector<Pos3F>& points)
//{
//	// Move Base
//	if (armsIdx == arms.size()) {
//		auto endpoints = arms[0].GetEndpoints();
//		std::vector<std::pair<const Pos3F&, const Pos3F&>> eds;
//		eds.reserve(endpoints.size());
//
//		for (auto& i : endpoints) {
//			eds.push_back({ i.first, points[i.second] });
//		}
//		base.UpdateOrientation(eds);
//	}
//	// Move Arms
//	else { 
//		auto endpoints = arms[armsIdx].GetEndpoints();
//		std::vector<std::pair<const Pos3F&, const Pos3F&>> eds;
//		eds.reserve(endpoints.size());
//
//		for (auto& i : endpoints) {
//			eds.push_back({ i.first, points[i.second] });
//		}
//
//		arms[armsIdx].UpdateTheta(eds);
//	}
//	
//	if (armsIdx == 0) {
//		armsIdx = arms.size();
//	}
//	else {
//		armsIdx--;
//	}
//
//	arms[0].ForwardPass(base.get_center(), 0);
//}

void IKRobot::RenderRobot(Renderer& renderer)
{
	// Update Joints
	for (size_t i = 0; i < joints.size(); i++) {
		joints[i].SetCenter(arms[i + 1].GetBasePos());
	}

	for (auto& i : joints) {
		renderer.Render(i);
	}

	// Render Base and Arms
	renderer.Render(base);
	for (auto& i : arms) {
		renderer.Render(i);
	}
}

float IKRobot::GetMaxReach() const
{
	float limb0Length = 0;
	float limb1Length = 0;

	for (auto& i : arms) {
		if (i.GetEndpointIdx() == 0) {
			limb0Length += i.GetLength();
		}
		else if (i.GetEndpointIdx() == 1) {
			limb1Length += i.GetLength();
		}
	}
	return limb0Length + limb1Length;
}
