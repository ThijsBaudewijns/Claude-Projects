#include "AIScene.h"
#include "PresetBehaviour.h"
#include "SteeringContext.h"
#include "Camera.h"
#include "Sprite.h"
#include "Button.h"
#include "Text.h"
#include "../HUDCamera.h"
#include "AIScript.h"
#include "AIAgent.h"
#include "Collider.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "Texture.h"
#include "Resources.h"
#include <iostream>

AIScene::AIScene(const std::string& name) : HelperScene(name)
{
    Engine& engine = Engine::instance();
    Resources::Load<Texture>("../assets/player.png", "player");


    auto renderSystem = engine.GetSystem<RenderSystem>();
    if (!renderSystem) {
        std::cerr << "RenderSystem not available!\n";
        return;
    }

    // =========================
    // LAYER DEFINITIONS
    // =========================
    const int LAYER_GAME = 0;      // Game objects (sprites, enemies, etc.)
    const int LAYER_HUD = 1;       // HUD/UI elements


    AddGameObject(CreateCamera("MainCamera", LAYER_GAME));
    AddGameObject(CreateHUDCamera("HUDCamera"));

    // =========================
    // GAME OBJECTS (Layer 0)
    // =========================

    // CREATE OBJECT WITH AI AGENT THAT STAYS ON MOUSE POSITION, BY USING MOUSE SCRIPT
    auto mouseObj = CreateTestObject("MouseFollower", Vector2(0, 0), 4, Color(255, 0, 0, 255), LAYER_GAME);
    auto mouseAgent = mouseObj->AddComponent<AIAgent>();
    auto mouseScript = mouseObj->AddComponent<AIScript>();
    AddGameObject(mouseObj);


    //// Create a flock of 20 boids
    //std::vector<std::shared_ptr<GameObject>> flock;
    //for (int i = 0; i < 20; i++) {
    //    float x = (rand() % 400) - 200.0f;
    //    float y = (rand() % 400) - 200.0f;

    //    auto boid = CreateTestObject("Boid_" + std::to_string(i), Vector2(x, y), 10, Color(100, 200, 255, 255), LAYER_GAME);
    //    auto agent = boid->AddComponent<AIAgent>();

    //    // Add flocking behaviors with custom weights
    //    auto separation = PresetBehaviour::Separation()
    //        .SetSeparationRadius(30.0f)
    //        .SetWeight(1.5f);
    //    agent->AddSteeringContext(separation);

    //    auto alignment = PresetBehaviour::Alignment()
    //        .SetAlignmentRadius(50.0f)
    //        .SetWeight(1.0f);
    //    agent->AddSteeringContext(alignment);

    //    auto cohesion = PresetBehaviour::Cohesion()
    //        .SetCohesionRadius(750.0f)
    //        .SetWeight(1.0f);
    //    agent->AddSteeringContext(cohesion);

    //    AddGameObject(boid);
    //    flock.push_back(boid);
    //}

    //// Add a mouse that part of the flock follows
    //for (int i = 0; i < 5; i++) {
    //    flock[i]->GetComponent<AIAgent>()->AddSteeringContext(PresetBehaviour::Seek(mouseAgent).SetRadius(500));
    //}

    // Create a static obstacle
    // --- Outer walls (4 obstacles) ---
    AddGameObject(CreateObstacle(Vector2(-300, -300), Vector2(600, 20))); // bottom
    AddGameObject(CreateObstacle(Vector2(-300, 280), Vector2(600, 20))); // top
    AddGameObject(CreateObstacle(Vector2(-300, -300), Vector2(20, 600))); // left
    AddGameObject(CreateObstacle(Vector2(280, -300), Vector2(20, 600))); // right

    // --- Horizontal walls (3 obstacles) ---
    AddGameObject(CreateObstacle(Vector2(-250, -150), Vector2(200, 20)));
    AddGameObject(CreateObstacle(Vector2(-50, -50), Vector2(250, 20)));
    AddGameObject(CreateObstacle(Vector2(50, 200), Vector2(250, 20)));

    // --- Inner blocks / dead ends (3 obstacles) ---
    AddGameObject(CreateObstacle(Vector2(-150, 150), Vector2(80, 80)));
    AddGameObject(CreateObstacle(Vector2(100, 50), Vector2(80, 80)));
    AddGameObject(CreateObstacle(Vector2(-50, -250), Vector2(80, 80)));




    // ARRIVAL EXAMPLE - Agent smoothly arrives at mouse position while avoiding obstacles
    for (int i = 0; i < 10; ++i) {
        float size = 10.0f;
        // start positions spread out
        float startX = static_cast<float>((rand() % 600) - 300);
        float startY = static_cast<float>((rand() % 600) - 300);
        auto arrivingAgent = CreateTestObject("ArrivingAgent", Vector2(startX, startY), size, Color(0, 100, 0, 255), LAYER_GAME);
        auto agentCollider = arrivingAgent->AddComponent<BoxCollider>();
        agentCollider->width = size;
        agentCollider->height = size;
        auto arrivalAI = arrivingAgent->AddComponent<AIAgent>();

        arrivalAI->AddSteeringContext(
            PresetBehaviour::PathFinding(mouseAgent)
            .SetWeight(1.5f)
        );
        arrivalAI->AddSteeringContext(
            PresetBehaviour::PresetBehaviour::Separation()
            .SetSeparationRadius(15.0f)
            .SetWeight(1.0f)
        );
        arrivalAI->AddSteeringContext(
            PresetBehaviour::ObstacleAvoidance()
            .SetAvoidanceDistance(15.0f)   // Look ahead 60 units for obstacles
            .SetAvoidanceForce(2.0f)       // Strong avoidance force
            .SetWeight(1.5f)
        );

        AddGameObject(arrivingAgent);
    }

    //arrivalAI->AddSteeringContext(
//    PresetBehaviour::Arrival(mouseAgent)
//    .SetSlowingRadius(150.0f)      // Start slowing down at 150 units
//    .SetArrivalTolerance(10.0f)    // Consider arrived within 10 units
//    .SetWeight(1.0f)
//);
//   arrivalAI->AddSteeringContext(
//       PresetBehaviour::ObstacleAvoidance()
//       .SetAvoidanceDistance(60.0f)   // Look ahead 60 units for obstacles
//       .SetAvoidanceForce(2.0f)       // Strong avoidance force
//       .SetWeight(1.5f)
   //);

    //// WANDER EXAMPLE - Agent wanders randomly
    //auto wanderer = CreateTestObject("Wanderer", Vector2(100, 100), 8, Color(150, 255, 150, 255), LAYER_GAME);
    //auto wandererAI = wanderer->AddComponent<AIAgent>();
    //wandererAI->AddSteeringContext(
    //    PresetBehaviour::Wander()
    //    .SetWanderRadius(30.0f)        // Size of the wander circle
    //    .SetWanderDistance(80.0f)      // Distance of circle from agent
    //    .SetWanderJitter(15.0f)        // Randomness amount per frame
    //    .SetWeight(1.0f)
    //);
    //AddGameObject(wanderer);

    //// PURSUIT EXAMPLE - Agent predicts and chases a moving target
    //auto pursuer = CreateTestObject("Pursuer", Vector2(-200, 0), 8, Color(255, 50, 50, 255), LAYER_GAME);
    //auto pursuerAI = pursuer->AddComponent<AIAgent>();

    //// Create a target that wanders
    //auto pursuitTarget = CreateTestObject("PursuitTarget", Vector2(200, 0), 6, Color(100, 100, 255, 255), LAYER_GAME);
    //auto pursuitTargetAI = pursuitTarget->AddComponent<AIAgent>();
    //pursuitTargetAI->AddSteeringContext(PresetBehaviour::Wander());
    //AddGameObject(pursuitTarget);

    //// Pursuer chases the wandering target
    //pursuerAI->AddSteeringContext(
    //    PresetBehaviour::Pursuit(pursuitTargetAI)
    //    .SetMaxPrediction(2.0f)        // Look up to 2 seconds ahead
    //    .SetWeight(1.0f)
    //);
    //AddGameObject(pursuer);

    //// EVADE EXAMPLE - Agent runs away from pursuer
    //auto evader = CreateTestObject("Evader", Vector2(0, -150), 8, Color(255, 255, 100, 255), LAYER_GAME);
    //auto evaderAI = evader->AddComponent<AIAgent>();

    //// Evader runs from the pursuer
    //evaderAI->AddSteeringContext(
    //    PresetBehaviour::Evade(pursuerAI)
    //    .SetMaxPrediction(1.5f)        // Predict threat 1.5 seconds ahead
    //    .SetRadius(300.0f)             // Only evade when threat is within 300 units
    //    .SetWeight(2.0f)               // Higher priority
    //);

    //// Add some wander so evader doesn't just run in straight line
    //evaderAI->AddSteeringContext(
    //    PresetBehaviour::Wander()
    //    .SetWeight(0.3f)               // Lower weight than evasion
    //);
    //AddGameObject(evader);

    //// COMBINED EXAMPLE - Agent that seeks but also wanders
    //auto seekerWanderer = CreateTestObject("SeekerWanderer", Vector2(0, 150), 8, Color(200, 100, 255, 255), LAYER_GAME);
    //auto seekerWandererAI = seekerWanderer->AddComponent<AIAgent>();

    //// Seeks mouse when close, otherwise wanders
    //seekerWandererAI->AddSteeringContext(
    //    PresetBehaviour::Seek(mouseAgent)
    //    .SetRadius(200.0f)             // Only seek when mouse is within 200 units
    //    .SetWeight(1.5f)
    //);

    //seekerWandererAI->AddSteeringContext(
    //    PresetBehaviour::Wander()
    //    .SetWeight(0.5f)               // Wander has lower priority
    //);
    //AddGameObject(seekerWanderer);

    //// COMPLEX EXAMPLE - Patrol behavior using arrival
    //// Create patrol points and have agent arrive at each one in sequence
    //auto patroller = CreateTestObject("Patroller", Vector2(-250, -250), 8, Color(100, 255, 255, 255), LAYER_GAME);
    //auto patrollerAI = patroller->AddComponent<AIAgent>();

    //// You would need to implement a script that switches the target between patrol points
    //// when the agent arrives at each one, but the arrival behavior makes it smooth
    //patrollerAI->AddSteeringContext(
    //    PresetBehaviour::Arrival(mouseAgent)  // In practice, switch this target dynamically
    //    .SetSlowingRadius(100.0f)
    //    .SetArrivalTolerance(15.0f)
    //    .SetWeight(1.0f)
    //);
    //AddGameObject(patroller);

    std::cout << "Menu scene created\n";
}
