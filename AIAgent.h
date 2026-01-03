#pragma once
#include "Component.h"
#include "Vector2.h"
#include <memory>
#include <vector>

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

class ISteeringBehaviour;
class SteeringContext;

/// @brief Base class for user-defined AIAgents.
class ENGINE_API AIAgent : public Component {
public:
    AIAgent() = default;
    ~AIAgent() = default;

    /// @brief Called once on first enable.
    void OnStart();
    /// @brief Called every frame with variable timestep.
    /// @param dt Delta time in seconds.
    void OnUpdate(float dt);
    /// @brief Called when the AIAgent is being destroyed.
    void OnDestroy();

	/// @brief Add a steering context to this agent.
    void AddSteeringContext(const std::shared_ptr<SteeringContext>& context);

    /// @brief Remove a steering context from this agent.
    void RemoveSteeringContext(const std::shared_ptr<SteeringContext>& context);

    /// @brief Get the steering context from this agent by identifier.
	/// @param identifier The identifier of the steering context.
	std::shared_ptr<SteeringContext> GetSteeringContext(const std::string identifier) const;

	float speed = 200.0f; ///< Movement speed of the agent in units per second.
	float maxForce = 1000.0f; ///< Maximum steering force that can be applied to the agent.
	Vector2 lastDesiredVelocity; ///< The last desired velocity calculated for this agent.

private: 
    std::vector<std::shared_ptr<SteeringContext>> pendingToAdd_;
    std::vector<std::shared_ptr<SteeringContext>> pendingToRemove_;
    std::vector<std::shared_ptr<SteeringContext>> contexts_;   
};
