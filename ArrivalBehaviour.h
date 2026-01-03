#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include "ISteeringBehaviour.h"
#include "AIAgent.h"

/// @file ArrivalBehaviour.h
/// @brief Arrival steering behaviour
/// @details Moves the agent towards a target position and slows down as it approaches.
/// Uses a slowing radius to begin deceleration and an arrival tolerance to determine
/// when the agent has successfully reached the target.
/// The steering force smoothly reduces as the agent gets closer to the target.

class ENGINE_API ArrivalBehaviour : public ISteeringBehaviour {
public:
    /// @brief Execute the Arrival behaviour
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
        Vector2 direction = targetPosition - agentPosition;
        float distance = direction.length();

        // If within arrival tolerance, we've arrived - no steering force needed
        if (distance < context->arrivalTolerance) {
            return Vector2{ 0.0f, 0.0f };
        }

        // Check radius constraint (0 = no limit)
        if (context->radius > 0.0f && distance > context->radius) {
            return Vector2{ 0.0f, 0.0f };
        }

        // Check view angle constraint (360 = see everything)
        if (context->viewAngle < 360.0f) {
            Vector2 forward = selfGameObject->transform.GetForward();
            float angle = std::acos(direction.normalized().dot(forward)) * (180.0f / 3.14159f);
            if (angle > context->viewAngle / 2.0f) {
                return Vector2{ 0.0f, 0.0f };
            }
        }

        // Calculate desired speed based on distance
        float desiredSpeed = context->self_->speed;

        // If within slowing radius, reduce speed proportionally
        if (distance < context->slowingRadius) {
            desiredSpeed = context->self_->speed * (distance / context->slowingRadius);
        }

        // Calculate steering force
        Vector2 desiredVelocity = direction.normalized() * desiredSpeed;
        Vector2 currentVelocity = selfGameObject->transform.velocity;
        Vector2 steeringForce = desiredVelocity - currentVelocity;

        return steeringForce * context->weight;
    }
};