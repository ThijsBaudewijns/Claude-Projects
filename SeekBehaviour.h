#pragma once

#pragma once
#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include "ISteeringBehaviour.h"
#include "AIAgent.h"
/// @file SeekBehaviour.h
/// @brief Seek steering behaviour
/// @details Moves the agent towards a target position
/// by calculating a desired velocity vector.
/// The steering force is the difference between the desired velocity
/// and the current velocity of the agent.
/// This behaviour is useful for pursuing targets or moving
/// to specific locations in the environment.
/// It can be combined with other behaviours for more complex movement patterns.

class ENGINE_API SeekBehaviour : public ISteeringBehaviour {
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
            Vector2 direction = targetPosition - agentPosition;
            float distance = direction.length();

            // Check radius constraint (0 = no limit)
            if (context->radius > 0.0f && distance > context->radius) {
                return Vector2{ 0.0f, 0.0f }; // Target is out of range
            }

            // Check view angle constraint (360 = see everything)
            if (context->viewAngle < 360.0f) {
                Vector2 forward = selfGameObject->transform.GetForward(); // You'll need to implement this
                float angle = std::acos(direction.normalized().dot(forward)) * (180.0f / 3.14159f);
                if (angle > context->viewAngle / 2.0f) {
                    return Vector2{ 0.0f, 0.0f }; // Target is outside view angle
                }
            }

            // Calculate steering force
            Vector2 desiredVelocity = direction.normalized() * context->self_->speed;
            Vector2 currentVelocity = selfGameObject->transform.velocity;
            Vector2 steeringForce = desiredVelocity - currentVelocity;

            return steeringForce * context->weight;
        }

};