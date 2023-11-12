#include "IKRobot.h"

IKRobot::IKRobot() :
	base(Pos3F(0,0,0), 0.1f, Material(ColorRGBA(0.6f, 0.2f, 0.2f, 1), 1, 0, 0, 10, -1))
{
	Material mat = Material(ColorRGBA(0.8f, 0.2f, 0.2f, 1), 1, 0, 0, 10, -1);
	arms.emplace_back(mat, 0.4f, 0.1f, 0.f, -1.57f, 1.57f);
	arms.emplace_back(mat, 0.4f, 0.1f, 0.2f, -1.57f, 1.57f);
	arms.emplace_back(mat, 0.4f, 0.1f, 0.4f, -1.57f, 1.57f);
	armsIdx = arms.size() - 1;

	arms[0].SetChildren({ &arms[1] });
	arms[1].SetChildren({ &arms[2] });
	arms[0].ForwardPass(base.get_center(), 0);
}

void IKRobot::MoveTowards(const Pos3F& point)
{
}

void IKRobot::StepTowards(const Pos3F& point)
{
	arms[armsIdx].UpdateTheta(arms.back().GetEndPos(), point);
	
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
