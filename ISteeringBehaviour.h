/// @file ISteeringBehaviour.h
/// @brief Base interface for steering behaviours

#pragma once

#include "Vector2.h"
#include "AIAgent.h"
#include "SteeringContext.h"
#include "AISystem.h"
#include "Engine.h"
#include "PhysicsSystem.h"
#include <memory>
#include <vector>
#include <list>

class Collider;

/// @brief Abstract interface for steering behaviours
class ISteeringBehaviour {
public:
	/// @brief Virtual destructor for proper cleanup
	virtual ~ISteeringBehaviour() = default;

	/// @brief Update the behaviour on execution
	/// @details returns the steering force as a Vector2
	virtual Vector2 Execute(const std::shared_ptr<SteeringContext> context) = 0;

	/// @brief Get all agents in the scene
	std::vector<std::shared_ptr<AIAgent>> GetAgents() {
		Engine& e = Engine::instance();
		auto aiSystem = e.GetSystem<AISystem>();
		return aiSystem->GetAllAgents();
	}

	/// @brief Get all colliders in the scene
	std::list<std::shared_ptr<Collider>> GetColliders() {
		Engine& e = Engine::instance();
		if (auto physicsSystem = e.GetSystem<PhysicsSystem>())
			return physicsSystem->GetColliders();
		return {};
	}

	/// @brief Get path in the scene
	std::vector<std::shared_ptr<Vector2>> GetPath(const Vector2& start, const Vector2& end) {
		Engine& e = Engine::instance();
		if (auto physicsSystem = e.GetSystem<PhysicsSystem>()) {
			return physicsSystem->GetPath(std::make_shared<Vector2>(start), std::make_shared<Vector2>(end));
		}
		return {};
	}
};