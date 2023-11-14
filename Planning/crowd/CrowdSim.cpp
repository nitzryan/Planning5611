#include "CrowdSim.h"

const int MAX_PEOPLE = 30;
const float AVERAGE_SPAWN_TIME = 2.0f;
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
	float cutoff = 1.0f - (dt / AVERAGE_SPAWN_TIME);

	// Calculate Movement
	for (auto& i : people) {
		i.CalculateDirection();
	}

	// Adjust movement to avoid other people
	for (size_t i = 0; i < people.size(); i++) {
		if (people[i].ReadyToRecycle()) {
			continue;
		}
		
		people[i].ComputeTTC(people, i + 1, dt);
	}

	// Adjust movement to avoid walls
	auto walls = map.GetWalls();
	for (auto& i : people) {
		for (auto j : walls) {
			i.AvoidWall(*j, dt);
		}
	}

	// Move of create people
	for (size_t i = 0; i < people.size(); i++) {
		if (people[i].ReadyToRecycle()) { // Chance to spawn
			if (randPerson(mt) > cutoff) {
				Pos2F pos(0, 0);
				const CrowdNode* node;
				const CrowdDest* dest = map.GetSpawnConditions(mt, pos, node);
				people[i] = CrowdPerson(pos, personMat, node, dest, &map);
			}
		}
		else { // Move towards goal
			people[i].Move(dt);
		}
	}
}
