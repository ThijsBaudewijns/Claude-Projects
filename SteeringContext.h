/// @file SteeringContext.h
/// @brief Base for steering behaviour contexts

#pragma once

#include <memory>
#include <string>
#include <vector>

class ISteeringBehaviour;
class AIAgent;

/// @brief Base for steering behaviour contexts
class SteeringContext {
public:
    /// @brief Virtual destructor for proper cleanup
    virtual ~SteeringContext() = default;

    std::shared_ptr<ISteeringBehaviour> behaviour_;
    std::weak_ptr<AIAgent> target_;
    AIAgent* self_; // TODO: possible weak ptr?
    bool active_ = true;

    // Basic steering parameters
    float radius = 50.0f;              // Detection/influence radius
    float weight = 1.0f;               // Weight/priority of this behavior
    float viewAngle = 360.0f;          // Field of view in degrees
    std::string identifier = "DefaultContext";

    // Flocking parameters
    float separationRadius = 25.0f;    // Personal space radius for separation
    float alignmentRadius = 50.0f;     // Radius to consider neighbors for alignment
    float cohesionRadius = 75.0f;      // Radius to consider neighbors for cohesion

    // Arrival parameters
    float slowingRadius = 100.0f;      // Radius at which to start slowing down
    float arrivalTolerance = 5.0f;     // Distance considered "arrived"

    // Wander parameters
    float wanderRadius = 50.0f;        // Radius of wander circle
    float wanderDistance = 100.0f;     // Distance of wander circle from agent
    float wanderJitter = 10.0f;        // Max random displacement per frame

    // Pursuit/Evade parameters
    float maxPrediction = 1.0f;        // Max time to predict target position (seconds)

    // Obstacle avoidance parameters
    float avoidanceDistance = 50.0f;   // How far ahead to look for obstacles
    float avoidanceForce = 1.5f;       // Multiplier for avoidance steering
	bool ignoreAgentsInAvoidance = true; // Whether to ignore other agents as obstacles

    // Path following parameters
    float pathRadius = 10.0f;          // Radius around path to follow
    float pathAheadDistance = 25.0f;   // How far ahead to look on path

	// Group behavior parameters
	std::shared_ptr<std::vector<std::shared_ptr<AIAgent>>> groupMembers_; // If set, only consider these agents
	std::shared_ptr<std::vector<std::shared_ptr<AIAgent>>> ignoreAgents_; // Agents to ignore in calculations
};