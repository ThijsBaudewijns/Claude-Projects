#include "../Headers/PresetBehaviour.h"

#include "../Headers/SeekBehaviour.h"
#include "../Headers/FleeBehaviour.h"
#include "../Headers/ArrivalBehaviour.h"
#include "../Headers/PursuitBehaviour.h"
#include "../Headers/EvadeBehaviour.h"
#include "../Headers/WanderBehaviour.h"
#include "../Headers/SeparationBehaviour.h"
#include "../Headers/AlignmentBehaviour.h"
#include "../Headers/ObstacleAvoidanceBehaviour.h"
#include "../Headers/CohesionBehaviour.h"
#include "../Headers/PathFollowingBehaviour.h"

PresetBehaviour::ContextBuilder PresetBehaviour::Seek(std::shared_ptr<AIAgent> target) {
    auto context = std::make_shared<SteeringContext>();
    std::shared_ptr<ISteeringBehaviour> behaviour = std::make_shared<SeekBehaviour>();
    context->behaviour_ = behaviour;
    context->radius = 0.0f;
    context->weight = 1.0f;
    context->viewAngle = 360.0f;
    context->active_ = true;
    context->target_ = target;
    RegisterBehaviour(behaviour, "SeekBehaviour");
    return ContextBuilder(context);
}

PresetBehaviour::ContextBuilder PresetBehaviour::Flee(std::shared_ptr<AIAgent> target) {
    auto context = std::make_shared<SteeringContext>();
    std::shared_ptr<ISteeringBehaviour> behaviour = std::make_shared<FleeBehaviour>();
    context->behaviour_ = behaviour;
    context->radius = 0.0f;
    context->weight = 1.0f;
    context->viewAngle = 360.0f;
    context->active_ = true;
    context->target_ = target;
    RegisterBehaviour(behaviour, "FleeBehaviour");
    return ContextBuilder(context);
}

PresetBehaviour::ContextBuilder PresetBehaviour::Arrival(std::shared_ptr<AIAgent> target) {
    auto context = std::make_shared<SteeringContext>();
    std::shared_ptr<ISteeringBehaviour> behaviour = std::make_shared<ArrivalBehaviour>();
    context->behaviour_ = behaviour;
    context->radius = 0.0f;
    context->weight = 1.0f;
    context->viewAngle = 360.0f;
    context->slowingRadius = 100.0f;
    context->arrivalTolerance = 5.0f;
    context->active_ = true;
    context->target_ = target;
    RegisterBehaviour(behaviour, "ArrivalBehaviour");
    return ContextBuilder(context);
}

PresetBehaviour::ContextBuilder PresetBehaviour::Pursuit(std::shared_ptr<AIAgent> target) {
    auto context = std::make_shared<SteeringContext>();
    std::shared_ptr<ISteeringBehaviour> behaviour = std::make_shared<PursuitBehaviour>();
    context->behaviour_ = behaviour;
    context->radius = 0.0f;
    context->weight = 1.0f;
    context->viewAngle = 360.0f;
    context->maxPrediction = 1.0f;
    context->active_ = true;
    context->target_ = target;
    RegisterBehaviour(behaviour, "PursuitBehaviour");
    return ContextBuilder(context);
}

PresetBehaviour::ContextBuilder PresetBehaviour::Evade(std::shared_ptr<AIAgent> target) {
    auto context = std::make_shared<SteeringContext>();
    std::shared_ptr<ISteeringBehaviour> behaviour = std::make_shared<EvadeBehaviour>();
    context->behaviour_ = behaviour;
    context->radius = 0.0f;
    context->weight = 1.0f;
    context->viewAngle = 360.0f;
    context->maxPrediction = 1.0f;
    context->active_ = true;
    context->target_ = target;
    RegisterBehaviour(behaviour, "EvadeBehaviour");
    return ContextBuilder(context);
}

PresetBehaviour::ContextBuilder PresetBehaviour::Wander() {
    auto context = std::make_shared<SteeringContext>();
    std::shared_ptr<ISteeringBehaviour> behaviour = std::make_shared<WanderBehaviour>();
    context->behaviour_ = behaviour;
    context->weight = 1.0f;
    context->wanderRadius = 50.0f;
    context->wanderDistance = 100.0f;
    context->wanderJitter = 10.0f;
    context->active_ = true;
    RegisterBehaviour(behaviour, "WanderBehaviour");
    return ContextBuilder(context);
}

PresetBehaviour::ContextBuilder PresetBehaviour::Separation() {
    auto context = std::make_shared<SteeringContext>();
    std::shared_ptr<ISteeringBehaviour> behaviour = std::make_shared<SeparationBehaviour>();
    context->behaviour_ = behaviour;
    context->separationRadius = 25.0f;
    context->weight = 1.0f;
    context->active_ = true;
    RegisterBehaviour(behaviour, "SeparationBehaviour");
    return ContextBuilder(context);
}

PresetBehaviour::ContextBuilder PresetBehaviour::Alignment() {
    auto context = std::make_shared<SteeringContext>();
    std::shared_ptr<ISteeringBehaviour> behaviour = std::make_shared<AlignmentBehaviour>();
    context->behaviour_ = behaviour;
    context->alignmentRadius = 50.0f;
    context->weight = 1.0f;
    context->active_ = true;
    RegisterBehaviour(behaviour, "AlignmentBehaviour");
    return ContextBuilder(context);
}

PresetBehaviour::ContextBuilder PresetBehaviour::Cohesion() {
    auto context = std::make_shared<SteeringContext>();
    std::shared_ptr<ISteeringBehaviour> behaviour = std::make_shared<CohesionBehaviour>();
    context->behaviour_ = behaviour;
    context->cohesionRadius = 75.0f;
    context->weight = 1.0f;
    context->active_ = true;
    RegisterBehaviour(behaviour, "CohesionBehaviour");
    return ContextBuilder(context);
}

PresetBehaviour::ContextBuilder PresetBehaviour::ObstacleAvoidance() {
    auto context = std::make_shared<SteeringContext>();
    std::shared_ptr<ISteeringBehaviour> behaviour = std::make_shared<ObstacleAvoidanceBehaviour>();
    context->behaviour_ = behaviour;
	context->avoidanceDistance = 50.0f;
	context->avoidanceForce = 1.5f;
	context->ignoreAgentsInAvoidance = true;
    context->weight = 1.0f;
    context->active_ = true;
    RegisterBehaviour(behaviour, "ObstacleAvoidanceBehaviour");
    return ContextBuilder(context);
}

PresetBehaviour::ContextBuilder PresetBehaviour::PathFinding(std::shared_ptr<AIAgent> target) {
    auto context = std::make_shared<SteeringContext>();
    std::shared_ptr<ISteeringBehaviour> behaviour = std::make_shared<PathFollowingBehaviour>();
    context->behaviour_ = behaviour;
	context->pathAheadDistance = 25.0f;
	context->pathRadius = 10.0f;
    context->weight = 1.0f;
    context->active_ = true;
    context->target_ = target;
    RegisterBehaviour(behaviour, "PathfindingBehaviour");
    return ContextBuilder(context);
}

void PresetBehaviour::RegisterBehaviour(const std::shared_ptr<ISteeringBehaviour>& behaviour, const std::string& identifier) {
    Engine& engine = Engine::instance();
    auto aiSystem = engine.GetSystem<AISystem>();
    if (aiSystem) {
        aiSystem->RegisterBehaviour(behaviour, identifier);
    }
}