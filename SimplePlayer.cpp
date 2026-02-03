#include "main.hpp"
#include "SimplePlayer.hpp"
#include "SimplePlayer.h"
#include "GameManager.h"
#include "cocos2d.h"

// ==============================================
// SIMPLEPLAYER DESTRUCTOR - FULL IMPLEMENTATION
// ==============================================

SimplePlayer::~SimplePlayer() {
    // Update vtable pointers (multiple inheritance)
    this->vtable = &SimplePlayer_vtable;                 // +0x0
    this->colorable_vtable = &Colorable_vtable;         // +0x140 offset
    this->blendable_vtable = &Blendable_vtable;         // +0x158 offset
    
    // ===== STEP 1: UNLOAD PLAYER ICONS =====
    
    // Get GameManager singleton
    GameManager* gameManager = GameManager::sharedState();
    
    // Unload icons for this player
    int playerIconID = this->m_playerIconID;            // +0x298
    gameManager->unloadIcons(playerIconID);
    
    // ===== STEP 2: REMOVE ICON DELEGATE =====
    
    // Check if we need to remove icon delegate
    bool hasIconDelegate = this->m_hasIconDelegate;     // +0x2a0
    if (hasIconDelegate) {
        gameManager->removeIconDelegate(playerIconID);
    }
    
    // ===== STEP 3: CALL PARENT DESTRUCTOR =====
    
    // SimplePlayer inherits from CCSprite
    cocos2d::CCSprite::~CCSprite();
}
