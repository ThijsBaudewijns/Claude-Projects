#pragma once

#include "../HelperScene.h"
#include "../../Engine/Headers/RenderSystem.h"
#include "../../Engine/Headers/ScriptSystem.h"

class AIScene : public HelperScene {
public:
    AIScene(const std::string& name);
    virtual ~AIScene() = default;
};
