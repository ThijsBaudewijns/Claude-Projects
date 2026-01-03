#include "../Headers/AIAgent.h"
#include "../Headers/Vector2.h"
#include "../Headers/ISteeringBehaviour.h"
#include "../Headers/SteeringContext.h"

#include <iostream>
#include <algorithm>
#include <cmath>

void AIAgent::OnStart() {}

void AIAgent::OnUpdate(float dt) {
	// Process pending additions
	for (const auto& behaviour : pendingToAdd_) {
		contexts_.push_back(behaviour); // add to system
	}
	pendingToAdd_.clear();

	// Process pending removals
	for (const auto& behaviour : pendingToRemove_) {
		auto it = std::find(contexts_.begin(), contexts_.end(), behaviour);  // add to system
		if (it != contexts_.end()) {
			contexts_.erase(it);
		}
	}
	pendingToRemove_.clear();

	// Sum steering forces (accelerations)
	Vector2 steering(0.0f, 0.0f);

	for (const auto& context : contexts_) {
		if (!context->active_) continue;
		if (auto behaviour = context->behaviour_) {
			steering += behaviour->Execute(context);
		}
	}

	// Clamp acceleration
	float len = steering.length();
	if (len > maxForce) {
		steering = (steering / len) * maxForce;
	}

	// Integrate
	auto gameObject = GetGameObject();
	if (!gameObject) return;

	gameObject->transform.velocity += steering * dt;

	// Apply drag
	float drag = 2.0f;
	gameObject->transform.velocity *= (std::max)(0.0f, 1.0f - drag * dt);


	// Integrate position
	gameObject->transform.position +=
		gameObject->transform.velocity * dt;


}

void AIAgent::OnDestroy() {
}

void AIAgent::AddSteeringContext(const std::shared_ptr<SteeringContext>& context) {
	pendingToAdd_.push_back(context);
	context->self_ = this;
}

void AIAgent::RemoveSteeringContext(const std::shared_ptr<SteeringContext>& context) {
	pendingToRemove_.push_back(context);
}

std::shared_ptr<SteeringContext> AIAgent::GetSteeringContext(const std::string identifier) const {
	for (const auto& context : contexts_) {
			if (context->identifier == identifier) {
				return context;
			}
	}
	return nullptr;
}