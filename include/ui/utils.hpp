#pragma once

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/UI/Button.hpp"

namespace AnyText::UI {

    UnityEngine::UI::Button* createIconButton(UnityEngine::Transform* parent, UnityEngine::Sprite* sprite, std::function<void()> onClick);

}