#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include "ISteeringBehaviour.h"
#include "AIAgent.h"

/// @file AlignmentBehaviour.h
/// @brief Alignment steering behaviour for flocking
/// @details Steers the agent to match the average heading of nearby neighbors.
/// This creates coordinated group movement.

class ENGINE_API AlignmentBehaviour : public ISteeringBehaviour {
public:
    /// @brief Execute the Alignment behaviour
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
        Vector2 averageVelocity = Vector2::Zero();
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

            // Check if within alignment radius
            if (distance > 0.0f && distance < context->alignmentRadius) {
                averageVelocity += otherGameObject->transform.velocity;
                neighborCount++;
            }
        }

        Vector2 steeringForce = Vector2::Zero();

        // Calculate steering to match average velocity
        if (neighborCount > 0) {
            averageVelocity = averageVelocity / static_cast<float>(neighborCount);

            // Desired velocity is the average velocity
            Vector2 desiredVelocity = averageVelocity.normalized() * context->self_->speed;

            Vector2 currentVelocity = selfGameObject->transform.velocity;
            steeringForce = desiredVelocity - currentVelocity;
        }

        return steeringForce * context->weight;
    }
};