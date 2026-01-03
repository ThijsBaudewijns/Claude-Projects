#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include "ISteeringBehaviour.h"
#include "AIAgent.h"

/// @file SeparationBehaviour.h
/// @brief Separation steering behaviour for flocking
/// @details Moves the agent away from nearby neighbors to maintain personal space.
/// This prevents agents from crowding together.

class ENGINE_API SeparationBehaviour : public ISteeringBehaviour {
public:
    /// @brief Execute the Separation behaviour
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
        Vector2 steeringForce = Vector2::Zero();
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
            Vector2 toOther = otherPosition - agentPosition;
            float distance = toOther.length();

            // Check if within separation radius
            if (distance > 0.0f && distance < context->separationRadius) {
                // The closer the neighbor, the stronger the repulsion
                Vector2 awayFromOther = (agentPosition - otherPosition).normalized();
                awayFromOther = awayFromOther / distance; // Weight by inverse distance
                steeringForce += awayFromOther;
                neighborCount++;
            }
        }

        // Average the steering force
        if (neighborCount > 0) {
            steeringForce = steeringForce / static_cast<float>(neighborCount);
            steeringForce = steeringForce.normalized() * context->self_->speed;

            Vector2 currentVelocity = selfGameObject->transform.velocity;
            steeringForce = steeringForce - currentVelocity;
        }

        return steeringForce * context->weight;
    }
};