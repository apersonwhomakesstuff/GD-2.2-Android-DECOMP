#pragma once

// Shared project header for reverse-engineered Geometry Dash components.
// This header is included by every translation unit to centralize common
// definitions and document the project scope.

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// Project source index:
// - GJBaseGameLayer.cpp / .hpp: core layer logic (player creation, effects).
// - GJEffectManager.cpp / .hpp: effect manager destructor and containers.
// - GameObject.cpp / .hpp: GameObject destructor and cleanup.
// - GameLevelManager.cpp / .hpp: local level lookups and username caching.
// - LevelEditorLayer.cpp / .hpp: editor layer interface outline.
// - PlayerObject.cpp / .hpp: PlayerObject destructor and cleanup.
// - SimplePlayer.cpp / .hpp: SimplePlayer destructor and cleanup.
// - getGameObjectPhysics.cpp / .hpp: physics table lookup/insert logic.
