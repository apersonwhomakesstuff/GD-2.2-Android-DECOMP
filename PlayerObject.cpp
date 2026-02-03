// PlayerObject.cpp
#include "PlayerObject.hpp"
#include "GameManager.hpp"

PlayerObject::PlayerObject() {
    // Initialize all members to zero/false
    memset(reinterpret_cast<void*>(this), 0, sizeof(PlayerObject));
}

PlayerObject::~PlayerObject() {
    // Call base destructor first
    GameObject::~GameObject();

    // Release owned arrays
    if (m_slopeObjects) {
        m_slopeObjects->release();
        m_slopeObjects = nullptr;
    }
    if (m_touchingObjects) {
        m_touchingObjects->release();
        m_touchingObjects = nullptr;
    }
    if (m_activeParticles) {
        m_activeParticles->release();
        m_activeParticles = nullptr;
    }

    // Delete checkpoint data if not globally managed
    uintptr_t globalSentinel = *(uintptr_t*)((uintptr_t)cocos2d::CCDirector::sharedDirector() + 0xfe8);
    if (m_checkpointData && (uintptr_t(m_checkpointData) - 0x18) != globalSentinel) {
        delete[] reinterpret_cast<char*>(uintptr_t(m_checkpointData) - 0x18);
        m_checkpointData = nullptr;
    }
}

PlayerObject* PlayerObject::create(int playerID, int iconID, GJBaseGameLayer* layer, cocos2d::CCLayer* camera, bool isDual) {
    auto* player = new PlayerObject();
    if (player && player->init(playerID, iconID, layer, camera, isDual)) {
        player->autorelease();
        return player;
    }
    delete player;
    return nullptr;
}

// --- Core Methods (Representative Samples) ---

void PlayerObject::bumpPlayer(float yVel, int type, bool force, GameObject* obj) {
    if (m_isDashing) return;
    if (m_isPlatformer && type == 0x2c) {
        propellPlayer(type);
        return;
    }
    if (m_isSpider) {
        // Spider logic
        setYVelocity(-yVel);
        return;
    }
    // Standard bump
    setYVelocity(yVel);
}

void PlayerObject::ringJump(GameObject* obj) {
    if (m_isFlying) return;
    if (m_isDashing) return;
    setYVelocity(1.0f); // Approximate
    m_isFlying = true;
}

int PlayerObject::flipMod(int value) {
    return ((value % 2) + 2) % 2;
}

bool PlayerObject::isFlying() {
    return m_isFlying;
}

bool PlayerObject::isInBasicMode() {
    return !m_isShip && !m_isBall && !m_isSpider && !m_isSwing;
}

bool PlayerObject::isSafeMode() {
    return GameManager::sharedState()->m_safeMode;
}

void PlayerObject::setYVelocity(double vel) {
    m_yVelocity = static_cast<float>(vel);
}

void PlayerObject::addToYVelocity(double delta) {
    m_yVelocity += static_cast<float>(delta);
}

void PlayerObject::flipGravity(PlayerButton dir) {
    m_gravity = (m_gravity == 1) ? -1 : 1;
    m_isUpsideDown = !m_isUpsideDown;
}

// --- Collision Methods ---

void PlayerObject::collidedWithObject(float yDelta, GameObject* obj, const cocos2d::CCRect& rect, bool fromTop) {
    if (obj->m_objectType == 1810) { // Ring
        ringJump(obj);
        return;
    }
    if (fromTop) {
        m_isOnGround = true;
        m_isFlying = false;
        m_yVelocity = 0;
    }
}

void PlayerObject::preSlopeCollision(float yDelta, GameObject* obj) {
    // Handle slope logic
    float slopeY = slopeYPos(obj);
    m_lastSafeY = slopeY;
}

float PlayerObject::slopeYPos(GameObject* other) {
    // Simplified: return average Y of object rect
    return other->getPositionY();
}

bool PlayerObject::isFacingDown() {
    return m_gravity == -1;
}

// --- Input Methods ---

void PlayerObject::pushButton(PlayerButton button) {
    if (m_controlsLocked) return;
    switch (button) {
        case PlayerButton::Jump:
            if (!m_isFlying) {
                setYVelocity(1.0f);
                m_isFlying = true;
            }
            break;
        default: break;
    }
}

void PlayerObject::releaseButton(PlayerButton button) {
    // No-op for jump; handled on press
}

void PlayerObject::releaseAllButtons() {
    m_hasJustJumped = false;
}

void PlayerObject::enablePlayerControls() {
    m_controlsLocked = false;
}

void PlayerObject::disablePlayerControls() {
    m_controlsLocked = true;
    releaseAllButtons();
}

void PlayerObject::lockPlayer() {
    if (!m_hasJustJumped) {
        m_hasJustJumped = true;
        // Fade out streak, etc.
    }
}

// --- Particles ---

void PlayerObject::setupStreak() {
    if (!m_streak) {
        m_streak = cocos2d::CCParticleSystemQuad::create("playerStreak.plist");
        if (m_streak) {
            m_streak->retain();
            m_playerSprite->addChild(m_streak);
        }
    }
}

void PlayerObject::placeStreakPoint() {
    if (m_streak) {
        m_streak->resetSystem();
    }
}

void PlayerObject::fadeOutStreak2() {
    if (m_streak) {
        m_streak->stopSystem();
    }
}

void PlayerObject::deactivateParticle(cocos2d::CCParticleSystem* particle) {
    if (particle) {
        particle->stopSystem();
    }
}

// --- Checkpoints ---

void PlayerObject::loadFromCheckpoint(void* checkpoint) {
    // Restore position, velocity, mode, etc.
    // Full implementation would deserialize all fields
}

// --- Utility ---

void PlayerObject::resetCollisionLog() {
    m_collisionLog.clear();
}

void PlayerObject::animatePlatformerJump() {
    // Play jump animation
}

void PlayerObject::runRotateAction(bool clockwise, float duration) {
    auto rotate = cocos2d::CCRotateBy::create(duration, clockwise ? 360 : -360);
    m_playerSprite->runAction(rotate);
}

void PlayerObject::showEndAnimation() {
    // Trigger death or finish animation
}

void PlayerObject::touchedObject(GameObject* obj) {
    // Handle touch logic (e.g., orbs, pads)
}
