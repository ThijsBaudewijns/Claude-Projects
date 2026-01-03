#include "../Headers/AISystem.h"
#include "../Headers/AIAgent.h"

void AISystem::Initialize() {
	
}

void AISystem::Update(float deltaTime) {
	// Add pending agents
	for (const auto& agent : pendingAgentsToAdd_) {
		agents_.push_back(agent);
		agent->OnStart();
	}
	pendingAgentsToAdd_.clear();
	// Update all agents
	for (const auto& agent : agents_) {
		if (agent->active)
			agent->OnUpdate(deltaTime);
	}
	// Remove pending agents
	for (const auto& agentToRemove : pendingAgentsToRemove_) {
		agents_.erase(std::remove(agents_.begin(), agents_.end(), agentToRemove), agents_.end());
		agentToRemove->OnDestroy();
	}
	pendingAgentsToRemove_.clear();

	// Add pending behaviours
	for (auto& behaviour : pendingBehavioursToAdd_) {
		behaviours_.try_emplace(
			behaviour.first,
			std::move(behaviour.second)
		);
	}
	pendingBehavioursToAdd_.clear();
}

void AISystem::Shutdown() {
	for (const auto& agent : agents_) {
		agent->OnDestroy();
	}
	agents_.clear();
	pendingAgentsToAdd_.clear();
	pendingAgentsToRemove_.clear();
}

void AISystem::RegisterAgent(std::shared_ptr<AIAgent> agent) {
	pendingAgentsToAdd_.push_back(agent);
}

void AISystem::UnregisterAgent(std::shared_ptr<AIAgent> agent) {
	pendingAgentsToRemove_.push_back(agent);
}

void AISystem::RegisterBehaviour(std::shared_ptr<ISteeringBehaviour> behaviour, std::string identifier) {
	pendingBehavioursToAdd_[identifier] = behaviour;
}