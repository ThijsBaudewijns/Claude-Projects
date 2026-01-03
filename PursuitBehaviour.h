#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include "ISteeringBehaviour.h"
#include "AIAgent.h"

/// @file PursuitBehaviour.h
/// @brief Pursuit steering behaviour
/// @details Predicts the future position of a moving target and steers towards it.
/// Uses intelligent prediction: if the target can be reached within the prediction time,
/// seeks directly. Otherwise, calculates the optimal interception point based on
/// relative velocities and distances. This creates more realistic chasing behavior
/// than simple seek, as the agent "leads" the target.

class ENGINE_API PursuitBehaviour : public ISteeringBehaviour {
public:
    /// @brief Execute the Pursuit behaviour
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
        Vector2 toTarget = targetPosition - agentPosition;
        float distance = toTarget.length();

        // Check radius constraint (0 = no limit)
        if (context->radius > 0.0f && distance > context->radius) {
            return Vector2{ 0.0f, 0.0f };
        }

        // Check view angle constraint (360 = see everything)
        if (context->viewAngle < 360.0f) {
            Vector2 forward = selfGameObject->transform.GetForward();
            float angle = std::acos(toTarget.normalized().dot(forward)) * (180.0f / 3.14159f);
            if (angle > context->viewAngle / 2.0f) {
                return Vector2{ 0.0f, 0.0f };
            }
        }

        Vector2 targetVelocity = targetGameObject->transform.velocity;
        Vector2 selfVelocity = selfGameObject->transform.velocity;

        // Calculate relative velocity
        Vector2 relativeVelocity = targetVelocity - selfVelocity;
        float targetSpeed = targetVelocity.length();
        float selfSpeed = context->self_->speed;

        // Smart prediction calculation
        float predictionTime = 0.0f;

        // Check if target is moving towards or away from us
        float relativeHeading = toTarget.normalized().dot(targetVelocity.normalized());

        // If target is ahead and facing us, we can use simpler prediction
        if (relativeHeading > 0.95f) {
            // Target is moving away in roughly the same direction we're facing
            predictionTime = distance / (selfSpeed + targetSpeed);
        }
        else {
            // Use more sophisticated prediction for interception
            // Calculate if we can intercept within max prediction time

            // Solve for interception: |targetPos + targetVel*t - (agentPos + agentVel*t)| = 0
            // This simplifies to a quadratic equation
            float a = relativeVelocity.dot(relativeVelocity) - (selfSpeed * selfSpeed);
            float b = 2.0f * toTarget.dot(relativeVelocity);
            float c = toTarget.dot(toTarget);

            // If a is near zero, velocities are matched - use simple prediction
            if (std::abs(a) < 0.001f) {
                predictionTime = distance / selfSpeed;
            }
            else {
                float discriminant = b * b - 4.0f * a * c;

                if (discriminant >= 0.0f) {
                    // We can intercept - find the earliest positive time
                    float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
                    float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);

                    if (t1 > 0.0f) {
                        predictionTime = t1;
                    }
                    else if (t2 > 0.0f) {
                        predictionTime = t2;
                    }
                    else {
                        // Both solutions negative - use distance-based prediction
                        predictionTime = distance / selfSpeed;
                    }
                }
                else {
                    // Cannot intercept - aim for closest approach
                    predictionTime = -b / (2.0f * a);
                    if (predictionTime < 0.0f) {
                        predictionTime = 0.0f;
                    }
                }
            }
        }

        // Clamp prediction time to max prediction
        predictionTime = (std::min)(predictionTime, context->maxPrediction);

        // If prediction time is very small, just seek directly
        Vector2 predictedPosition;
        if (predictionTime < 0.1f) {
            predictedPosition = targetPosition;
        }
        else {
            // Predict future position
            predictedPosition = targetPosition + (targetVelocity * predictionTime);
        }

        // Calculate steering towards predicted position
        Vector2 directionToPredicted = predictedPosition - agentPosition;
        Vector2 desiredVelocity = directionToPredicted.normalized() * selfSpeed;
        Vector2 currentVelocity = selfGameObject->transform.velocity;
        Vector2 steeringForce = desiredVelocity - currentVelocity;

        return steeringForce * context->weight;
    }
};