#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include "ISteeringBehaviour.h"
#include "AIAgent.h"
#include "Collider.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include <algorithm>

/// @file ObstacleAvoidanceBehaviour.h
/// @brief Obstacle avoidance steering behaviour
/// @details Projects a detection ray ahead of the agent and steers away
/// from obstacles in the path. The avoidance force is perpendicular to
/// the direction of the obstacle, scaled by proximity.
/// Uses the avoidanceDistance parameter to determine how far ahead to look
/// and avoidanceForce to scale the steering response.

class ENGINE_API ObstacleAvoidanceBehaviour : public ISteeringBehaviour {
public:
    /// @brief Execute the Obstacle Avoidance behaviour
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
        Vector2 velocity = selfGameObject->transform.velocity;

        // If not moving, no avoidance needed
        if (velocity.length() < 0.01f) {
            return Vector2{ 0.0f, 0.0f };
        }

        Vector2 forward = velocity.normalized();
        float lookAheadDistance = context->avoidanceDistance;

        // Find the closest threatening obstacle
        float closestDistance = lookAheadDistance;
        Vector2 closestObstaclePosition{ 0.0f, 0.0f };
        bool obstacleFound = false;

        for (const auto& collider : GetColliders()) {
            auto colliderGO = collider->gameObject.lock();
            if (!colliderGO) continue;

            if (!colliderGO->active) continue;

            // Skip if this is the agent's own collider
            if (colliderGO.get() == selfGameObject.get()) continue;

			// Skip other agents
			if (context->ignoreAgentsInAvoidance) {
				auto agentComponent = colliderGO->GetComponent<AIAgent>();
				if (agentComponent) continue;
			}

            Vector2 colliderPosition = colliderGO->transform.GetWorldPosition();
            Vector2 toObstacle = colliderPosition - agentPosition;

            // Project obstacle position onto agent's forward direction
            float projection = toObstacle.dot(forward);

            // Skip obstacles behind or too far ahead
            if (projection < 0.0f || projection > lookAheadDistance) continue;

            // Find closest point on the agent's forward ray to the obstacle
            Vector2 closestPointOnRay = agentPosition + forward * projection;
            float distanceToRay = (colliderPosition - closestPointOnRay).length();

            // Get obstacle radius based on collider type
            float obstacleRadius = 0.0f;

            if (auto boxCollider = std::dynamic_pointer_cast<BoxCollider>(collider)) {
                float width = boxCollider->GetWidth();
                float height = boxCollider->GetHeight();
                // Use approximate radius (half diagonal)
                obstacleRadius = std::sqrt(width * width + height * height) * 0.5f;
            }
            else if (auto circleCollider = std::dynamic_pointer_cast<CircleCollider>(collider)) {
                obstacleRadius = circleCollider->GetRadius();
            }
            // Add safety margin (agent's approximate radius)
            float safetyMargin = 10.0f; // Adjust based on your agent size

            if (auto selfCircleCollider = selfGameObject->GetComponent<CircleCollider>()) {
                safetyMargin = selfCircleCollider->GetRadius();
            } else if (auto selfBoxCollider = selfGameObject->GetComponent<BoxCollider>()) {
                float selfWidth = selfBoxCollider->GetWidth();
                float selfHeight = selfBoxCollider->GetHeight();
                safetyMargin = std::sqrt(selfWidth * selfWidth + selfHeight * selfHeight) * 0.5f;
			}

            float threatRadius = obstacleRadius + safetyMargin;

            // Check if obstacle is in our path
            if (distanceToRay < threatRadius && projection < closestDistance) {
                closestDistance = projection;
                closestObstaclePosition = colliderPosition;
                obstacleFound = true;
            }
        }

        if (!obstacleFound) {
            return Vector2{ 0.0f, 0.0f };
        }

        // Calculate avoidance force
        Vector2 toObstacle = closestObstaclePosition - agentPosition;

        // Create lateral avoidance force (perpendicular to forward direction)
        // Determine which side to avoid to (left or right)
        Vector2 right = Vector2{ forward.getY(), -forward.getX()}; // Perpendicular vector
        float side = toObstacle.dot(right);

        // Steer away from the obstacle (opposite to the side it's on)
        Vector2 avoidanceDirection = (side < 0.0f) ? right : (right * -1.0f);

        // Scale force by proximity (closer = stronger)
        float proximityFactor = 1.0f - (closestDistance / lookAheadDistance);
        float forceMagnitude = context->self_->speed * proximityFactor * context->avoidanceForce;

        Vector2 steeringForce = avoidanceDirection * forceMagnitude;

        return steeringForce * context->weight;
    }
};