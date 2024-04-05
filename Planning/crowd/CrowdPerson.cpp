#include "CrowdPerson.h"
#include <iostream>
#include <assert.h>
#include "../geometry/CollisonDetection2D.h"

const float WALKING_SPEED = 0.5f;
const float TTC_TIME_CUTOFF = 2.5f;
const float TTC_DIST_CUTOFF = TTC_TIME_CUTOFF * WALKING_SPEED;
const float TTC_DIST_CUTTOFF_SQUARED = TTC_DIST_CUTOFF * TTC_DIST_CUTOFF;

const float WALL_AVOID_FACTOR = 0.35f;
const float AVOIDANCE_DIST = 0.07f;
const float AVOIDANCE_MAG = 0.25f;

const float RADIUS = 0.125f;
const float AVOID_IMMINENT_COLLISION_DIST = 3.0f * RADIUS;
const float AVOID_IMMINENT_COLLISION_MAG = 0.50f;
const float AVOID_IMMINENT_COL_SLOPE = -1.0f / (AVOID_IMMINENT_COLLISION_DIST - 2.0f * RADIUS);
const float AVOID_IMMINENT_COL_Y0 = 1.0f - 2.0f * RADIUS * AVOID_IMMINENT_COL_SLOPE;

const float WRONG_DIRECTION_SLOWDOWN_MULT = 25.0f;
const float WRONG_DIRECTION_SLOWDOWN_START = 0.0f; // 90 deg

CrowdPerson::CrowdPerson(Pos2F start, const Material& mat, const CrowdNode* startNode, const CrowdDest* dest, const CrowdMap* crowdMap) :
	CircleRenderable(start, RADIUS, mat),
	vel(0,0),
	dest(dest),
	crowdMap(crowdMap),
	currentNode(0)
{
	if (startNode == nullptr) {
		return;
	}
	// Traverse destination map to get best values
	auto map = dest->GetMap();
	traversalNodes.push_back(startNode);

	auto currentNode = startNode;
	while (!dest->IsInDest(currentNode->GetCenter())) {
		// Get child with best heuristic
		auto children = currentNode->GetConnectedNodes();
		float bestHeuristic = std::numeric_limits<float>::infinity();
		for (auto i : children) {
			float score = map[i];
			if (score < bestHeuristic) {
				bestHeuristic = score;
				currentNode = i;
			}
		}

		traversalNodes.push_back(currentNode);
	}
}

float CrowdPerson::GetDistFrom(const CrowdPerson& p) const
{
	return (center - p.center).GetMagnitude();
}

void CrowdPerson::CalculateDirection(float dt)
{
	if (ReadyToRecycle() || traversalNodes.empty()) { // In Destination
		return;
	}

	//// Make sure can still walk towards point
	if (!crowdMap->ValidPathBetween(center, traversalNodes[currentNode]->GetCenter(), radius)) {
		if (currentNode > 0) {
			currentNode--;
		}
		else {
			currentNode = 1; // Handle getting evicted at spawn
		}
	}

	// Check to see if can move further down the list
	for (size_t i = currentNode + 1; i < traversalNodes.size(); i++) {
		if (crowdMap->ValidPathBetween(center, traversalNodes[i]->GetCenter(), radius * 1.5f)) {
			currentNode = i;
		}
		else {
			break;
		}
	}

	// Walk towards target
	Vec2F velForce = traversalNodes[currentNode]->GetCenter() - center;
	velForce.Normalize();
	vel = vel + Vec2F::Mul(velForce, dt);
	
	// Cap vel
	float magVel = vel.GetMagnitude();
	if (magVel > WALKING_SPEED) {
		vel.Mul(WALKING_SPEED / magVel);
	}
}

void CrowdPerson::ComputeTTC(std::vector<CrowdPerson>& people, size_t startIdx, float dt)
{
	for (size_t i = startIdx; i < people.size(); i++) {
		if (people[i].ReadyToRecycle()) { // Person exists in memory, but not on the map
			continue;
		}
		
		// Self should be startIdx - 1, so don't need to check for self

		CrowdPerson& p = people[i];
		Vec2F w = center - p.center;

		if (w.GetMagnitudeSquared() >= TTC_DIST_CUTTOFF_SQUARED) { // To far away to consider
			continue;
		}
		
		if (w.GetMagnitude() < 2 * radius) { // Collision
			// Get Direction of collision vector
			float magDir = w.GetMagnitude();
			w.Normalize();
			// Reflect velocities across this vector
			/*float v1Perp = Vec2F::Dot(vel, w);
			float v2Perp = Vec2F::Dot(p.vel, w);
			float vPerp = v1Perp - v2Perp;
			vPerp /= 2;

			vel.Add(Vec2F::Mul(w, -vPerp));
			p.vel.Add(Vec2F::Mul(w, vPerp));*/
			vel = Vec2F(0, 0);
			p.vel = Vec2F(0, 0);

			w.Mul(0.0050f + (2 * radius - magDir));

			center = center + w;
			p.center = p.center - w;

			// Set velocity to seperate
			w.Normalize();
			w.Mul(0.01f);
			vel = w;
			p.vel = Vec2F::Mul(w, -1);

			continue;
		}

		//// Add avoidance force, to prevent walking right next to another
		//// If vel is same direction, TTC will not impact walking dist
		if (w.GetMagnitude() < 2 * radius + AVOIDANCE_DIST) {
			Vec2F wNorm = w.GetNormalized();
			float f = 2 * radius + AVOIDANCE_DIST - w.GetMagnitude();
			wNorm.Mul(AVOIDANCE_MAG * dt * ((AVOIDANCE_DIST - f) / AVOIDANCE_DIST));
			vel = vel + wNorm;
			p.vel = p.vel - wNorm;
		}

		Vec2F v = (vel - p.vel);

		// Do Quadratic equation
		float a = Vec2F::Dot(v, v);
		if (a == 0) {
			continue;
		}
		float b = Vec2F::Dot(w, v);
		float radWBuffer = radius + 0.001f;
		float c = Vec2F::Dot(w, w) - 4 * radWBuffer * radWBuffer; // Assume both radiuses are the same, (Ra + Rb)^2 actually

		float det = b * b - a * c;
		if (det < 0) { // They never collide
			continue;
		}

		float t1 = (-b - det) / (a);
		float t2 = (-b + det) / (a);

		if (t1 < 0 && t2 < 0) { // Collision if time moves backwards
			continue;
		}

		if (t1 < 0 || t2 < 0) { // Currently inside of buffer, but not actually colliding
			t1 = std::max(t1, t2);
			t2 = std::max(t1, t2);
		}

		// Must have 2 positive
		float t = std::min(t1, t2);
		if (t > TTC_TIME_CUTOFF) {
			continue;
		}

		if (t == 0) {
			t = 0.01f;
		}

		// Fast forward to collision
		Pos2F thisColPoint = center + Vec2F::Mul(vel, t);
		Pos2F otherColPoint = p.center + Vec2F::Mul(p.vel, t);
		Vec2F dir = thisColPoint - otherColPoint;
		Vec2F magDir = dir.GetNormalized();

		float fAvoid = (TTC_TIME_CUTOFF - t) / t; // Know that t != 0, as it would be caught by the collision check
		fAvoid /= dir.GetMagnitude();
		dir.Mul(fAvoid * dt);
		vel = vel + dir;
		p.vel = p.vel - dir;

		// Make sure walking speed doesn't go above max
		float velMag = vel.GetMagnitude();
		float pVelMag = p.vel.GetMagnitude();
		if (velMag > WALKING_SPEED) {
			vel.Mul(WALKING_SPEED / velMag);
		}
		if (pVelMag > WALKING_SPEED) {
			p.vel.Mul(WALKING_SPEED / pVelMag);
		}
	}
}

void CrowdPerson::Move(float dt)
{
	center.Add(Vec2F::Mul(vel, dt));
	CircleRenderable::GenerateRenderPoints();
}

void CrowdPerson::AvoidWall(const RectRenderable& rect, float dt)
{
	auto ll = rect.GetLL();
	auto ur = rect.GetUR();

	auto closestPoint = Pos2F(
	    clamp(ll.x, ur.x, center.x),
	    clamp(ll.y, ur.y, center.y)
	);
	
	Vec2F vec = Vec2F(0, 0);
	vec = center - closestPoint;
	if (vec.x != vec.x) {
		vec.x = vec.x;
	}
	auto dist = vec.GetMagnitude();

	if (dist > 2 * radius) { // Too far away to concern
		return;
	}	

	if (dist == 0) { // Completely inside
		float distLeft = abs(ll.x - center.x);
		float distRight = abs(ur.x - center.x);
		float distTop = abs(ur.y - center.y);
		float distBot = abs(ll.y - center.y);
		if (distLeft < distRight && distLeft < distTop && distLeft < distBot) {
			center.x = ll.x - radius - 0.01f;
		}
		else if (distRight < distTop && distRight < distBot) {
			center.x = ur.x + radius + 0.01f;
		}
		else if (distTop < distBot) {
			center.y = ur.y + radius + 0.01f;
		}
		else {
			center.y = ll.y - radius - 0.01f;
		}
		return;
	}
	vec.Normalize();
	if (dist < radius) { // Colliding
		float disp = radius + 0.01f - dist;
		center = center + Vec2F::Mul(vec, disp);
		if (center.x != center.x) {
			center.x = center.x;
		}
	}
	else { // Not colliding, but force away
		float disp = dist - radius;
		float fAvoid = (radius - disp) / disp;
		fAvoid *= dt;
		fAvoid *= WALL_AVOID_FACTOR;

		vel = vel + Vec2F::Mul(vec, fAvoid);

		float velMag = vel.GetMagnitude();
		if (velMag > WALKING_SPEED) {
			vel.Mul(WALKING_SPEED / velMag);
		}
	}
}

void CrowdPerson::AvoidImminentCollision(const CrowdPerson& p, float dt)
{
	Pos2F nextPos = center + Vec2F::Mul(vel, dt);
	Vec2F centerToPerson = p.center + Vec2F::Mul(p.vel, 0) - nextPos;
	float distMag = centerToPerson.GetMagnitude();
	if (distMag < AVOID_IMMINENT_COLLISION_DIST) {
		centerToPerson.Normalize();
		float velDeltaMag = AVOID_IMMINENT_COLLISION_MAG * (AVOID_IMMINENT_COL_SLOPE * distMag + AVOID_IMMINENT_COL_Y0);
		vel = Vec2F::Sub(vel, (Vec2F::Mul(centerToPerson, dt * velDeltaMag)));

		float velMag = vel.GetMagnitude();
		if (velMag > WALKING_SPEED) {
			vel.Mul(WALKING_SPEED / velMag);
		}
	}
}

void CrowdPerson::AvoidWrongDirectionMovement(float dt)
{
	if (traversalNodes.empty()) {
		return;
	}
	Vec2F nextDirection = traversalNodes[currentNode]->GetCenter() - center;
	nextDirection.Normalize();

	Vec2F dir = vel.GetNormalized();
	float vecDot = Vec2F::Dot(nextDirection, dir);
	if (vecDot < WRONG_DIRECTION_SLOWDOWN_START) {
		float slowdownMag = WRONG_DIRECTION_SLOWDOWN_MULT * dt * -vecDot;
		vel = Vec2F::Mul(vel, 1.0f - slowdownMag);
	}
}

bool CrowdPerson::ReadyToRecycle() const
{
	return dest->PointInRect(center);
}

