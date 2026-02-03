// main.hpp
#pragma once
#include <string>
#include <cocos2d.h>
#include "CCSpritePlus.h"
#include "GameManager.h"

extern int g_nextObjectID; // DAT_012fe018

// Forward declarations
class GameObject;
class EnhancedGameObject;
class TextGameObject;

// GameObject.hpp
#pragma once
#include "main.hpp"

class GameObject : public cocos2d::CCNode {
protected:
    // Verified offsets from disassembly
    cocos2d::CCSprite* m_secondarySprite;   // 0x2e0
    cocos2d::CCSprite* m_mainSprite;        // 0x2f0
    void*              m_physicsObject;     // 0x310
    int                m_objectID;          // 0x34
    cocos2d::CCSprite* m_colorSprite;       // 0x368
    int                m_uniqueID;          // 0x384
    bool               m_hasGlow;           // 0x289
    bool               m_highDetail;        // 0x420
    bool               m_dontFade;          // 0x4fe
    int                m_objectType;        // 0x3f4

    // Particle system data
    char* m_particleString;   // 0x438
    char* m_particleString2;  // 0x440
    int*  m_particleData;     // 0x478
    int*  m_particleData2;    // 0x488
    int*  m_particleData3;    // 0x498

public:
    GameObject();
    virtual ~GameObject();

    static void resetMID();
    void assignUniqueID();

    void createGlow(const std::string& frameName);
    void addGlow(const std::string& frameName);
    void addEmptyGlow();
    void removeGlow();
    void removeColorSprite();

    static cocos2d::CCSpriteFrame* getColorFrame(const std::string& baseFrame);
    static cocos2d::CCSpriteFrame* getGlowFrame(const std::string& baseFrame);

    bool shouldHaveGlow() const;
};

// EnhancedGameObject.hpp
class EnhancedGameObject : public GameObject {
public:
    EnhancedGameObject();
    virtual ~EnhancedGameObject();
};

// TextGameObject.hpp
class TextGameObject : public GameObject {
private:
    char* m_textContent; // 0x538
public:
    TextGameObject();
    virtual ~TextGameObject();
};

// GameObject.cpp
#include "GameObject.hpp"
#include <cstring>

int g_nextObjectID = 10; // DAT_012fe018

void GameObject::resetMID() {
    g_nextObjectID = 10;
}

void GameObject::assignUniqueID() {
    m_uniqueID = g_nextObjectID;
    m_objectID = g_nextObjectID;
    g_nextObjectID++;
}

void GameObject::createGlow(const std::string& frameName) {
    if (m_mainSprite) {
        m_mainSprite->release();
        m_mainSprite->removeFromParentAndCleanup(true);
        m_mainSprite = nullptr;
    }

    m_mainSprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName.c_str());
    if (m_mainSprite) {
        m_mainSprite->retain();
        auto mgr = GameManager::sharedState();
        if (mgr) {
            mgr->setColor(m_mainSprite);
            m_mainSprite->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
        }
        m_mainSprite->setZOrder(-1);
    }
}

bool GameObject::shouldHaveGlow() const {
    if (m_hasGlow) return false;

    auto mgr = GameManager::sharedState();
    if (!mgr) return false;

    if (mgr->m_showGhosts && m_highDetail) return false;
    if (m_dontFade) return false;

    // EXACT ranges from disassembly: 1–143, 324–541, 1724–1800
    int t = m_objectType;
    if ((t >= 1 && t <= 143) ||
        (t >= 324 && t <= 541) ||
        (t >= 1724 && t <= 1800)) {
        return true;
    }
    return false;
}

void GameObject::addGlow(const std::string& frameName) {
    if (!shouldHaveGlow()) return;

    auto glowFrame = getGlowFrame(frameName);
    if (glowFrame) {
        createGlow(glowFrame->getSpriteFrameName());
        m_hasGlow = true;
    }
}

void GameObject::addEmptyGlow() {
    if (m_hasGlow) return;
    createGlow("emptyGlow.png");
    if (m_mainSprite) {
        m_mainSprite->setOpacity(0);
    }
    m_hasGlow = true;
}

void GameObject::removeGlow() {
    if (m_mainSprite) {
        m_mainSprite->release();
        m_mainSprite->removeFromParentAndCleanup(true);
        m_mainSprite = nullptr;
    }
}

void GameObject::removeColorSprite() {
    if (m_colorSprite) {
        m_colorSprite->release();
        m_colorSprite->removeFromParentAndCleanup(true);
        m_colorSprite = nullptr;
    }
}

cocos2d::CCSpriteFrame* GameObject::getColorFrame(const std::string& baseFrame) {
    size_t dot = baseFrame.find_last_of('.');
    std::string name = (dot != std::string::npos) ? baseFrame.substr(0, dot) : baseFrame;
    return cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
        ("_color_" + name + ".png").c_str()
    );
}

cocos2d::CCSpriteFrame* GameObject::getGlowFrame(const std::string& baseFrame) {
    size_t dot = baseFrame.find_last_of('.');
    std::string name = (dot != std::string::npos) ? baseFrame.substr(0, dot) : baseFrame;
    return cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
        ("_glow_" + name + ".png").c_str()
    );
}

GameObject::~GameObject() {
    // Release all sprites
    if (m_mainSprite) { m_mainSprite->release(); m_mainSprite = nullptr; }
    if (m_colorSprite) { m_colorSprite->release(); m_colorSprite = nullptr; }
    if (m_secondarySprite) { m_secondarySprite->release(); m_secondarySprite = nullptr; }

    // Global sentinel pointer (PTR_DAT_012fdfe8 == *(CCDirector + 0xfe8))
    uintptr_t globalSentinel = *(uintptr_t*)((uintptr_t)cocos2d::CCDirector::sharedDirector() + 0xfe8);

    // Particle strings
    if (m_particleString && (uintptr_t(m_particleString) - 0x18) != globalSentinel) {
        delete[] m_particleString;
    }
    if (m_particleString2 && (uintptr_t(m_particleString2) - 0x18) != globalSentinel) {
        delete[] m_particleString2;
    }

    // Particle arrays (always owned)
    if (m_particleData) delete[] m_particleData;
    if (m_particleData2) delete[] m_particleData2;
    if (m_particleData3) delete[] m_particleData3;

    // Physics object
    if (m_physicsObject && (uintptr_t(m_physicsObject) - 0x18) != globalSentinel) {
        delete[] reinterpret_cast<char*>(uintptr_t(m_physicsObject) - 0x18);
    }

    // Nullify
    m_particleString = m_particleString2 = nullptr;
    m_particleData = m_particleData2 = m_particleData3 = nullptr;
    m_physicsObject = nullptr;
}

// EnhancedGameObject.cpp
EnhancedGameObject::EnhancedGameObject() = default;
EnhancedGameObject::~EnhancedGameObject() = default;

// TextGameObject.cpp
TextGameObject::TextGameObject() : m_textContent(nullptr) {}
TextGameObject::~TextGameObject() {
    uintptr_t globalSentinel = *(uintptr_t*)((uintptr_t)cocos2d::CCDirector::sharedDirector() + 0xfe8);
    if (m_textContent && (uintptr_t(m_textContent) - 0x18) != globalSentinel) {
        delete[] reinterpret_cast<char*>(uintptr_t(m_textContent) - 0x18);
    }
    m_textContent = nullptr;
}
