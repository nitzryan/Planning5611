#include "IKRobot.h"

IKRobot::IKRobot() :
	base(Pos3F(0,0,0), 0.1f, Material(ColorRGBA(0.6f, 0.2f, 0.2f, 1), 1, 0, 0, 10, -1))
{
	Material mat = Material(ColorRGBA(0.8f, 0.2f, 0.2f, 1), 1, 0, 0, 10, -1);
	arms.emplace_back(mat, 0.4f, 0.1f, 0.6f, -1.57f, 1.57f, -1);
	arms.emplace_back(mat, 0.4f, 0.1f, 0.0f, -1.57f, 1.57f, -1);
	arms.emplace_back(mat, 0.4f, 0.1f, 0.4f, 0.1f, 1.57f, 0);
	arms.emplace_back(mat, 0.4f, 0.1f, -0.4f, -1.57f, -0.1f, 1);
	armsIdx = arms.size() - 1;

	arms[0].SetChildren({ &arms[1] });
	arms[1].SetChildren({ &arms[2], &arms[3] });
	arms[0].ForwardPass(base.get_center(), 0);
}

void IKRobot::MoveTowards(const Pos3F& point)
{
}

void IKRobot::StepTowards(const std::vector<Pos3F>& points)
{
	auto endpoints = arms[armsIdx].GetEndpoints();
	std::vector<std::pair<const Pos3F&, const Pos3F&>> eds;
	eds.reserve(endpoints.size());

	for (auto& i : endpoints) {
		eds.push_back({ i.first, points[i.second] });
	}

	arms[armsIdx].UpdateTheta(eds);
	
	if (armsIdx == 0) {
		armsIdx = arms.size() - 1;
	}
	else {
		armsIdx--;
	}

	arms[0].ForwardPass(base.get_center(), 0);
}

void IKRobot::RenderRobot(Renderer& renderer)
{
	renderer.Render(base);
	for (auto& i : arms) {
		renderer.Render(i);
	}
}
