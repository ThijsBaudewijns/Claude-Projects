#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include "ISteeringBehaviour.h"
#include "AIAgent.h"

/// @file PathFollowingBehaviour.h
/// @brief Path following steering behaviour
/// @details Makes the agent follow a calculated path from its current position to a target.
/// The behaviour looks ahead on the path and steers towards the nearest point within the
/// pathAheadDistance. Uses pathRadius to determine when a waypoint is reached.
/// If no target is set, returns zero force. The path is recalculated each frame.

class ENGINE_API PathFollowingBehaviour : public ISteeringBehaviour {
public:
    /// @brief Execute the Path Following behaviour
    /// @param context The steering context containing parameters
    /// @return The steering force as a Vector2
    Vector2 Execute(const std::shared_ptr<SteeringContext> context) override
    {
        auto targetAgent = context->target_.lock();
        if (!targetAgent || !context->self_) {
            return Vector2{ 0.0f, 0.0f };
        }

        auto selfGO = context->self_->GetGameObject();
        auto targetGO = targetAgent->GetGameObject();
        if (!selfGO || !targetGO) {
            return Vector2{ 0.0f, 0.0f };
        }

        Vector2 agentPos = selfGO->transform.GetWorldPosition();
        Vector2 targetPos = targetGO->transform.GetWorldPosition();

        auto path = GetPath(agentPos, targetPos);
        if (path.size() < 2) {
            return Vector2{ 0.0f, 0.0f };
        }

        // ------------------------------------------------------------
        // 1. Find nearest point on the path (projection-based)
        // ------------------------------------------------------------
        Vector2 nearestPoint = agentPos;
        size_t nearestSegment = 0;
        float minDistSq = FLT_MAX;

        for (size_t i = 0; i < path.size() - 1; ++i) {
            Vector2 a = *path[i];
            Vector2 b = *path[i + 1];

            Vector2 p = GetClosestPointOnSegment(agentPos, a, b);
            float distSq = (p - agentPos).lengthSquared();

            if (distSq < minDistSq) {
                minDistSq = distSq;
                nearestPoint = p;
                nearestSegment = i;
            }
        }

        // ------------------------------------------------------------
        // 2. Walk forward along the path by look-ahead distance
        // ------------------------------------------------------------
        float remaining = context->pathAheadDistance;
        Vector2 currentPoint = nearestPoint;
        size_t segment = nearestSegment;

        while (remaining > 0.0f) {
            Vector2 segStart = (segment == nearestSegment)
                ? currentPoint
                : *path[segment];

            Vector2 segEnd = (segment + 1 < path.size())
                ? *path[segment + 1]
                : segStart;

            Vector2 segVec = segEnd - segStart;
            float segLen = segVec.length();

            if (segLen < 0.0001f) {
                break;
            }

            if (segLen > remaining) {
                Vector2 dir = segVec / segLen;
                currentPoint = segStart + dir * remaining;
                break;
            }

            remaining -= segLen;
            currentPoint = segEnd;

            if (++segment >= path.size() - 1) {
                break; // end of non-looped path
            }
        }

        Vector2 targetPoint = currentPoint;

        // ------------------------------------------------------------
        // 3. Seek to that point (no angle logic, no projection bias)
        // ------------------------------------------------------------
        Vector2 toTarget = targetPoint - agentPos;
        float distance = toTarget.length();
        if (distance < 0.001f) {
            return Vector2{ 0.0f, 0.0f };
        }

        Vector2 desiredVelocity =
            toTarget.normalized() * context->self_->speed;

        // Slow down near final destination (not look-ahead point)
        float distToFinal = (targetPos - agentPos).length();
        if (distToFinal < context->slowingRadius) {
            desiredVelocity *= (distToFinal / context->slowingRadius);
        }

        Vector2 currentVelocity =
            context->self_->GetTransform()->velocity;

        Vector2 steering =
            (desiredVelocity - currentVelocity) * context->weight;

        return steering;
    }




private:
    /// @brief Get the closest point on a line segment to a given point
    /// @param point The point to find the closest point to
    /// @param lineStart The start of the line segment
    /// @param lineEnd The end of the line segment
    /// @return The closest point on the segment
    Vector2 GetClosestPointOnSegment(const Vector2& point, const Vector2& lineStart, const Vector2& lineEnd) {
        Vector2 line = lineEnd - lineStart;
        float lineLength = line.length();

        // Handle degenerate case where segment has zero length
        if (lineLength < 0.0001f) {
            return lineStart;
        }

        Vector2 lineDirection = line / lineLength;
        Vector2 toPoint = point - lineStart;

        // Project point onto line
        float projection = toPoint.dot(lineDirection);

        // Clamp to segment bounds
        if (projection <= 0.0f) {
            return lineStart;
        }
        if (projection >= lineLength) {
            return lineEnd;
        }

        return lineStart + lineDirection * projection;
    }
};