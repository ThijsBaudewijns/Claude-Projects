#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include "ISteeringBehaviour.h"
#include "AIAgent.h"

/// @file EvadeBehaviour.h
/// @brief Evade steering behaviour
/// @details Predicts the future position of a pursuing target and steers away from it.
/// Uses intelligent prediction similar to pursuit: if the threat can reach the agent
/// within the prediction time, evades directly. Otherwise, calculates the optimal
/// evasion point based on relative velocities and distances.
/// This creates realistic fleeing behavior where the agent anticipates the threat's movement.

class ENGINE_API EvadeBehaviour : public ISteeringBehaviour {
public:
    /// @brief Execute the Evade behaviour
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

        // Calculate relative velocity (from target's perspective)
        Vector2 relativeVelocity = selfVelocity - targetVelocity;
        float targetSpeed = targetVelocity.length();
        float selfSpeed = context->self_->speed;

        // Smart prediction calculation for evasion
        float predictionTime = 0.0f;

        // Check if target is approaching
        float relativeHeading = toTarget.normalized().dot(targetVelocity.normalized());

        // If target is behind us and we're moving away, shorter prediction
        if (relativeHeading < -0.95f) {
            predictionTime = distance / (selfSpeed + targetSpeed);
        }
        else {
            // Calculate interception time (when threat would reach us)
            // We want to predict where the threat will be when it could catch us

            Vector2 toAgent = -toTarget; // Direction from target to agent

            // Solve quadratic for interception time
            float a = relativeVelocity.dot(relativeVelocity) - (targetSpeed * targetSpeed);
            float b = 2.0f * toAgent.dot(relativeVelocity);
            float c = toAgent.dot(toAgent);

            if (std::abs(a) < 0.001f) {
                // Matched velocities
                predictionTime = distance / targetSpeed;
            }
            else {
                float discriminant = b * b - 4.0f * a * c;

                if (discriminant >= 0.0f) {
                    // Calculate when threat could intercept
                    float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
                    float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);

                    if (t1 > 0.0f) {
                        predictionTime = t1;
                    }
                    else if (t2 > 0.0f) {
                        predictionTime = t2;
                    }
                    else {
                        // Threat can't catch us - use distance-based prediction
                        predictionTime = distance / targetSpeed;
                    }
                }
                else {
                    // Threat cannot intercept - evade from closest approach point
                    predictionTime = -b / (2.0f * a);
                    if (predictionTime < 0.0f) {
                        predictionTime = 0.0f;
                    }
                }
            }
        }

        // Clamp prediction time to max prediction
        predictionTime = (std::min)(predictionTime, context->maxPrediction);

        // Predict future position of threat
        Vector2 predictedPosition;
        if (predictionTime < 0.1f) {
            // Threat is very close - evade from current position
            predictedPosition = targetPosition;
        }
        else {
            // Predict where threat will be
            predictedPosition = targetPosition + (targetVelocity * predictionTime);
        }

        // Calculate steering AWAY from predicted position (opposite of pursuit)
        Vector2 directionAwayFromPredicted = agentPosition - predictedPosition;

        // If we're too close, add extra urgency
        float urgencyMultiplier = 1.0f;
        if (distance < 50.0f) {
            urgencyMultiplier = 2.0f - (distance / 50.0f); // 1.0 to 2.0 based on proximity
        }

        Vector2 desiredVelocity = directionAwayFromPredicted.normalized() * selfSpeed * urgencyMultiplier;
        Vector2 currentVelocity = selfGameObject->transform.velocity;
        Vector2 steeringForce = desiredVelocity - currentVelocity;

        return steeringForce * context->weight;
    }
};