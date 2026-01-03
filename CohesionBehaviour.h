#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include "ISteeringBehaviour.h"
#include "AIAgent.h"

/// @file CohesionBehaviour.h
/// @brief Cohesion steering behaviour for flocking
/// @details Steers the agent toward the average position of nearby neighbors.
/// This keeps the flock together as a group.

class ENGINE_API CohesionBehaviour : public ISteeringBehaviour {
public:
    /// @brief Execute the Cohesion behaviour
    /// @param context The steering context containing parameters
    /// @return The steering force as a Vector2
    Vector2 Execute(const std::shared_ptr<SteeringContext> context) override {
        if (!context->self_) {
            return Vector2{ 0.0f, 0.0f };
        }

        auto selfGameObject = context->self_->GetGameObject();
        if (!selfGameObject) {
            return Vector2{ 0.0f, 0.0f };
        }

        Vector2 agentPosition = selfGameObject->transform.GetWorldPosition();
        Vector2 centerOfMass = Vector2::Zero();
        int neighborCount = 0;

        // Get all agents in the scene
        auto allAgents = GetAgents();

        for (const auto& otherAgent : allAgents) {
            // Skip self
            if (otherAgent.get() == context->self_) {
                continue;
            }

            auto otherGameObject = otherAgent->GetGameObject();
            if (!otherGameObject) {
                continue;
            }

            Vector2 otherPosition = otherGameObject->transform.GetWorldPosition();
            float distance = agentPosition.distanceTo(otherPosition);

            // Check if within cohesion radius
            if (distance > 0.0f && distance < context->cohesionRadius) {
                centerOfMass += otherPosition;
                neighborCount++;
            }
        }

        Vector2 steeringForce = Vector2::Zero();

        // Steer toward center of mass
        if (neighborCount > 0) {
            centerOfMass = centerOfMass / static_cast<float>(neighborCount);

            // Desired velocity toward center of mass
            Vector2 desired = (centerOfMass - agentPosition).normalized() * context->self_->speed;

            Vector2 currentVelocity = selfGameObject->transform.velocity;
            steeringForce = desired - currentVelocity;
        }

        return steeringForce * context->weight;
    }
};