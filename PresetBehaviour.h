#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include <memory>
#include "SteeringContext.h"
#include "ISteeringBehaviour.h"

/// @brief Factory for predefined steering behaviours
class ENGINE_API PresetBehaviour {
public:
    class ContextBuilder {
    private:
        std::shared_ptr<SteeringContext> context_;

    public:
        ContextBuilder(std::shared_ptr<SteeringContext> context) : context_(context) {}

        ContextBuilder& SetRadius(float radius) {
            context_->radius = radius;
            return *this;
        }

        ContextBuilder& SetWeight(float weight) {
            context_->weight = weight;
            return *this;
        }

        ContextBuilder& SetViewAngle(float angle) {
            context_->viewAngle = angle;
            return *this;
        }

        ContextBuilder& SetSeparationRadius(float radius) {
            context_->separationRadius = radius;
            return *this;
        }

        ContextBuilder& SetAlignmentRadius(float radius) {
            context_->alignmentRadius = radius;
            return *this;
        }

        ContextBuilder& SetCohesionRadius(float radius) {
            context_->cohesionRadius = radius;
            return *this;
        }

        ContextBuilder& SetSlowingRadius(float radius) {
            context_->slowingRadius = radius;
            return *this;
        }

        ContextBuilder& SetArrivalTolerance(float tolerance) {
            context_->arrivalTolerance = tolerance;
            return *this;
        }

        ContextBuilder& SetWanderRadius(float radius) {
            context_->wanderRadius = radius;
            return *this;
        }

        ContextBuilder& SetWanderDistance(float distance) {
            context_->wanderDistance = distance;
            return *this;
        }

        ContextBuilder& SetWanderJitter(float jitter) {
            context_->wanderJitter = jitter;
            return *this;
        }

        ContextBuilder& SetMaxPrediction(float prediction) {
            context_->maxPrediction = prediction;
            return *this;
        }

        ContextBuilder& SetAvoidanceForce(float force) {
            context_->avoidanceForce = force;
            return *this;
        }

        ContextBuilder& SetAvoidanceDistance(float distance) {
            context_->avoidanceDistance = distance;
            return *this;
        }

        ContextBuilder& SetAvoidanceDistance(bool ignoreAgentsInAvoidance) {
            context_->ignoreAgentsInAvoidance = ignoreAgentsInAvoidance;
            return *this;
        }

        ContextBuilder& SetPathRadius(bool pathRadius) {
            context_->pathRadius = pathRadius;
            return *this;
        }

        ContextBuilder& SetPathAheadDistance(bool pathAheadDistance) {
            context_->pathAheadDistance = pathAheadDistance;
            return *this;
		}

        std::shared_ptr<SteeringContext> Build() {
            return context_;
        }

        /// @brief Implicit conversion to shared_ptr<SteeringContext>
        operator std::shared_ptr<SteeringContext>() const {
            return context_;
        }
    };

    // Basic behaviors
    static ContextBuilder Seek(std::shared_ptr<AIAgent> target);
    static ContextBuilder Flee(std::shared_ptr<AIAgent> target);
    static ContextBuilder Arrival(std::shared_ptr<AIAgent> target);

    // Advanced behaviors
    static ContextBuilder Pursuit(std::shared_ptr<AIAgent> target);
    static ContextBuilder Evade(std::shared_ptr<AIAgent> target);
    static ContextBuilder Wander();
    static ContextBuilder ObstacleAvoidance();
    static ContextBuilder PathFinding(std::shared_ptr<AIAgent> target);

    // Flocking behaviors
    static ContextBuilder Separation();
    static ContextBuilder Alignment();
    static ContextBuilder Cohesion();

private:
    static void RegisterBehaviour(const std::shared_ptr<ISteeringBehaviour>& behaviour, const std::string& identifier);
};