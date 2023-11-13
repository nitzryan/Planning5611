#include "CrowdSim.h"

CrowdSim::CrowdSim() :
	mt(5611),
	map(Material(ColorRGBA(0.8f, 0.8f, 0.8f, 1.0f), 1.0f, 0.0f, 0.0f, 10.0f, -1), mt)
{
}

void CrowdSim::Render(Renderer& renderer)
{
	map.Render(renderer);
}
