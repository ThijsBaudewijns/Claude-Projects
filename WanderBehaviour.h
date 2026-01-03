#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include "ISteeringBehaviour.h"
#include "AIAgent.h"
#include <random>

/// @file WanderBehaviour.h
/// @brief Wander steering behaviour
/// @details Creates random wandering movement by projecting a circle in front of the agent
/// and selecting a random point on that circle as a target.
/// The target point is jittered each frame to create organic, unpredictable movement.
/// This behaviour is useful for creating ambient movement, patrol patterns, or exploration.

class ENGINE_API WanderBehaviour : public ISteeringBehaviour {
private:
    Vector2 wanderTarget_ = Vector2{ 0.0f, 0.0f };
    static std::random_device rd_;
    static std::mt19937 gen_;
    static std::uniform_real_distribution<float> dis_;

public:
    WanderBehaviour() {
        // Initialize wander target to a random point on the circle
        float angle = dis_(gen_) * 2.0f * 3.14159f;
        wanderTarget_ = Vector2{ std::cos(angle), std::sin(angle) };
    }

    /// @brief Execute the Wander behaviour
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

        // Add random jitter to wander target
        float jitterX = (dis_(gen_) * 2.0f - 1.0f) * context->wanderJitter;
        float jitterY = (dis_(gen_) * 2.0f - 1.0f) * context->wanderJitter;
        wanderTarget_ += Vector2{ jitterX, jitterY };

        // Normalize and scale to wander radius
        wanderTarget_ = wanderTarget_.normalized() * context->wanderRadius;

        // Get agent's current direction
        Vector2 currentVelocity = selfGameObject->transform.velocity;
        Vector2 forward = currentVelocity.length() > 0.01f
            ? currentVelocity.normalized()
            : selfGameObject->transform.GetForward();

        // Project circle center in front of agent
        Vector2 circleCenter = forward * context->wanderDistance;

        // Calculate world space target position
        Vector2 targetWorld = circleCenter + wanderTarget_;

        // Calculate desired velocity towards the target
        Vector2 desiredVelocity = targetWorld.normalized() * context->self_->speed;

        // Calculate steering force
        Vector2 steeringForce = desiredVelocity - currentVelocity;

        return steeringForce * context->weight;
    }
};

// Static member initialization
std::random_device WanderBehaviour::rd_;
std::mt19937 WanderBehaviour::gen_(rd_());
std::uniform_real_distribution<float> WanderBehaviour::dis_(0.0f, 1.0f);