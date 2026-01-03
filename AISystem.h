/// @file AISystem.h
/// @brief AIAgent system for managing and updating Agents

#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include "ISystem.h"
#include <vector>
#include <memory>
#include <map>
#include <string>

class AIAgent;
class ISteeringBehaviour;

/// @brief Manages AIAgent lifecycles and updates.
class ENGINE_API AISystem : public ISystem {
public:
    AISystem() = default;
    ~AISystem() override = default;

    /// @brief Initialize agents resources.
    void Initialize() override;
    /// @brief Tick all agents with variable timestep.
    /// @param deltaTime Seconds since last frame.
    void Update(float deltaTime) override;
    /// @brief Shutdown and clear registered agents.
    void Shutdown() override;

    /// @brief Register a agent instance with the system.
    /// @param Agent to add.
    void RegisterAgent(std::shared_ptr<AIAgent> agent);
    /// @brief Unregister a agent instance.
    /// @param Agent to remove.
    void UnregisterAgent(std::shared_ptr<AIAgent> agent);

	/// @brief Get all registered agents.
	std::vector<std::shared_ptr<AIAgent>> GetAllAgents() const { return agents_; }

    /// @brief Register a behaviour instance with the system.
    /// @param Behaviour and identifier to add.
    void RegisterBehaviour(std::shared_ptr<ISteeringBehaviour> behaviour, std::string identifier);

private:
    std::vector<std::shared_ptr<AIAgent>> agents_;
    std::vector<std::shared_ptr<AIAgent>> pendingAgentsToAdd_;
    std::vector<std::shared_ptr<AIAgent>> pendingAgentsToRemove_;

    // identifier & behaviour
	std::map<std::string, std::shared_ptr<ISteeringBehaviour>> behaviours_;
    std::map<std::string, std::shared_ptr<ISteeringBehaviour>> pendingBehavioursToAdd_;
};
