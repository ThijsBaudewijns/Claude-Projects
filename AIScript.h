#pragma once
#include "../../Engine/Headers/BehaviourScript.h"
#include "../../Engine/Headers/Vector2.h"
#include "../../Engine/Headers/GameObject.h"
#include "../../Engine/Headers/Engine.h"
#include "../../Engine/Headers/Input.h"
#include <iostream>

// BehaviourScript already inherits from Component which inherits from ISerializable
// So we DON'T need to inherit from ISerializable again!
class AIScript : public BehaviourScript {
public:
    AIScript() {
    }

    virtual ~AIScript() = default;

    void OnUpdate(float deltaTime) override {
        auto go = GetGameObject();
        if (!go) return;

        auto pos = Input::GetMousePosition();
        go->transform.position = Vector2(static_cast<float>(pos.first - 400), static_cast<float>(pos.second - 300));
    }
};