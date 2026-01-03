#pragma once

#pragma once
#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include "ISteeringBehaviour.h"
#include "AIAgent.h"

#include <iostream>

/// @file FleeBehaviour.h
/// @brief Flee steering behaviour
/// @details Moves the agent away from the target position
/// by calculating a desired velocity vector.
/// The steering force is the difference between the desired velocity
/// and the current velocity of the agent.
/// This behaviour is useful for avoiding targets or moving
/// to specific locations in the environment.
/// It can be combined with other behaviours for more complex movement patterns.

class ENGINE_API FleeBehaviour : public ISteeringBehaviour {
public:
    /// @brief Execute the Seek behaviour
    /// @param context The steering context containing parameters
    /// @return The steering force as a Vector2
    Vector2 Execute(const std::shared_ptr<SteeringContext> context) override {
        auto targetAgent = context->target_.lock();
        if (!targetAgent || !context->self_) {
            return Vector2{ 0.0f, 0.0f };
        }

        auto selfGameObject = context->self_->GetGameObject();
        auto targetGameObject = targetAgent->GetGameObject();

        if (!selfGameObject || !targetGameObject) {
            return Vector2{ 0.0f, 0.0f };
        }

        Vector2 targetPosition = targetGameObject->transform.GetWorldPosition();
        Vector2 agentPosition = selfGameObject->transform.GetWorldPosition();
        Vector2 direction = agentPosition - targetPosition;
        float distance = direction.length();

        // Only flee if within radius (0 = always flee)
        if (context->radius > 0.0f && distance > context->radius) {
            return Vector2{ 0.0f, 0.0f }; // Threat is far enough away
        }

        // Check view angle
        if (context->viewAngle < 360.0f) {
            Vector2 threatDirection = targetPosition - agentPosition;
            Vector2 forward = selfGameObject->transform.GetForward();
            float angle = std::acos(threatDirection.normalized().dot(forward)) * (180.0f / 3.14159f);
            if (angle > context->viewAngle / 2.0f) {
                return Vector2{ 0.0f, 0.0f }; // Threat is outside view
            }
        }

        Vector2 desiredVelocity = direction.normalized() * context->self_->speed;
        Vector2 currentVelocity = selfGameObject->transform.velocity;
        Vector2 steeringForce = desiredVelocity - currentVelocity;

        return steeringForce * context->weight;
    }

};