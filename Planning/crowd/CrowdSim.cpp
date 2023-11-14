#include "CrowdSim.h"

const int MAX_PEOPLE = 100;
const Material personMat = Material(ColorRGBA(0.8f, 0.5f, 0.2f, 1.0f), 1.0f, 0.0f, 0.0f, 10.0f, -1);

CrowdSim::CrowdSim() :
	mt(5611),
	map(Material(ColorRGBA(0.8f, 0.8f, 0.8f, 1.0f), 1.0f, 0.0f, 0.0f, 10.0f, -1), mt)
{
	people.reserve(MAX_PEOPLE);
	auto dest = map.GetRandomDest(mt);
	for (int i = 0; i < MAX_PEOPLE; i++) {
		people.emplace_back(dest->GetSpawnPos(mt), personMat, nullptr, dest, &map);
	}
}

void CrowdSim::Render(Renderer& renderer)
{
	for (auto& i : people) {
		if (!i.ReadyToRecycle()) {
			renderer.Render(i);
		}
	}
	map.Render(renderer);
}

void CrowdSim::Update(float dt)
{
	std::uniform_real_distribution<float> randPerson(0.0f, 1.0f);

	for (size_t i = 0; i < people.size(); i++) {
		if (people[i].ReadyToRecycle()) { // Chance to spawn
			if (randPerson(mt) > (0.50f * dt)) {
				Pos2F pos(0, 0);
				const CrowdNode* node;
				const CrowdDest* dest = map.GetSpawnConditions(mt, pos, node);
				people[i] = CrowdPerson(pos, personMat, node, dest, &map);
			}
		}
		else { // Move towards goal
			people[i].Update(dt, people);
		}
	}
}
