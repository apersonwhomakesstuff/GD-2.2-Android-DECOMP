// main.hpp (shared header)
#pragma once

#include <string>
#include <cocos2d.h>
#include "CCSpritePlus.h"

// Global counter for unique object IDs
extern int g_nextObjectID;

// GameObject.hpp
#pragma once
#include "main.hpp"

class GameObject : public cocos2d::CCNode {
protected:
    cocos2d::CCSprite* m_mainSprite;      // 0x2f0
    cocos2d::CCSprite* m_colorSprite;     // 0x368
    cocos2d::CCSprite* m_secondarySprite; // 0x2e0
    char* m_particleString;               // 0x438
    char* m_particleString2;              // 0x440
    int* m_particleData;                  // 0x478
    int* m_particleData2;                 // 0x488
    int* m_particleData3;                 // 0x498
    void* m_physicsObject;                // 0x310
    int m_objectID;                       // 0x34
    int m_uniqueID;                       // 0x384
    bool m_hasGlow;                       // 0x289
    bool m_highDetail;                    // 0x420
    bool m_dontFade;                      // 0x4fe
    int m_objectType;                     // 0x3f4

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

    // Helper to determine if object should have glow based on type
    bool shouldHaveGlow() const;
};

// EnhancedGameObject.hpp
#pragma once
#include "GameObject.hpp"

class EnhancedGameObject : public GameObject {
public:
    EnhancedGameObject();
    virtual ~EnhancedGameObject();
};

// TextGameObject.hpp
#pragma once
#include "GameObject.hpp"

class TextGameObject : public GameObject {
private:
    char* m_textContent; // 0x538

public:
    TextGameObject();
    virtual ~TextGameObject();
};

// GameObject.cpp
#include "GameObject.hpp"
#include "GameManager.hpp"

int g_nextObjectID = 10; // DAT_012fe018

// Static method to reset the object ID counter
void GameObject::resetMID() {
    g_nextObjectID = 10;
}

void GameObject::assignUniqueID() {
    m_uniqueID = g_nextObjectID;
    m_objectID = g_nextObjectID;
    g_nextObjectID++;
}

void GameObject::createGlow(const std::string& frameName) {
    // Remove existing glow if present
    if (m_mainSprite) {
        m_mainSprite->release();
        m_mainSprite->removeFromParentAndCleanup(true);
        m_mainSprite = nullptr;
    }

    // Create new glow sprite
    m_mainSprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName.c_str());
    if (m_mainSprite) {
        m_mainSprite->retain();
        
        // Set proper rendering properties
        auto vtable = *(void**)this;
        auto setColorFunc = *(void**)((uintptr_t)vtable + 0x730);
        auto setBlendFuncFunc = *(void**)((uintptr_t)vtable + 0x7b0);
        
        // Call virtual methods to configure sprite
        ((void(*)(GameObject*, cocos2d::CCSprite*))setColorFunc)(this, m_mainSprite);
        ((void(*)(cocos2d::CCSprite*, void*))setBlendFuncFunc)(m_mainSprite, nullptr);
        
        // Set lowest priority for rendering
        m_mainSprite->setZOrder(-1);
    }
}

bool GameObject::shouldHaveGlow() const {
    // Skip if already has glow or in special modes
    if (m_hasGlow) return false;
    
    auto mgr = GameManager::sharedState();
    if (mgr->m_showGhosts && m_highDetail) return false;
    if (m_dontFade) return false;
    
    // Object types that should have glow effects
    switch (m_objectType) {
        // Common objects with glow
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
        case 48:
        case 49:
        case 50:
        case 51:
        case 52:
        case 53:
        case 54:
        case 55:
        case 56:
        case 57:
        case 58:
        case 59:
        case 60:
        case 61:
        case 62:
        case 63:
        case 64:
        case 65:
        case 66:
        case 67:
        case 68:
        case 69:
        case 70:
        case 71:
        case 72:
        case 73:
        case 74:
        case 75:
        case 76:
        case 77:
        case 78:
        case 79:
        case 80:
        case 81:
        case 82:
        case 83:
        case 84:
        case 85:
        case 86:
        case 87:
        case 88:
        case 89:
        case 90:
        case 91:
        case 92:
        case 93:
        case 94:
        case 95:
        case 96:
        case 97:
        case 98:
        case 99:
        case 100:
        case 101:
        case 102:
        case 103:
        case 104:
        case 105:
        case 106:
        case 107:
        case 108:
        case 109:
        case 110:
        case 111:
        case 112:
        case 113:
        case 114:
        case 115:
        case 116:
        case 117:
        case 118:
        case 119:
        case 120:
        case 121:
        case 122:
        case 123:
        case 124:
        case 125:
        case 126:
        case 127:
        case 128:
        case 129:
        case 130:
        case 131:
        case 132:
        case 133:
        case 134:
        case 135:
        case 136:
        case 137:
        case 138:
        case 139:
        case 140:
        case 141:
        case 142:
        case 143:
        case 324:
        case 325:
        case 326:
        case 328:
        case 330:
        case 331:
        case 333:
        case 335:
        case 337:
        case 338:
        case 339:
        case 340:
        case 341:
        case 342:
        case 343:
        case 344:
        case 345:
        case 346:
        case 347:
        case 348:
        case 349:
        case 350:
        case 351:
        case 352:
        case 353:
        case 354:
        case 355:
        case 356:
        case 357:
        case 358:
        case 359:
        case 360:
        case 361:
        case 362:
        case 363:
        case 364:
        case 365:
        case 366:
        case 367:
        case 368:
        case 369:
        case 370:
        case 371:
        case 372:
        case 373:
        case 374:
        case 375:
        case 376:
        case 377:
        case 378:
        case 379:
        case 380:
        case 381:
        case 382:
        case 383:
        case 384:
        case 385:
        case 386:
        case 387:
        case 388:
        case 389:
        case 390:
        case 391:
        case 392:
        case 393:
        case 394:
        case 395:
        case 396:
        case 397:
        case 398:
        case 399:
        case 400:
        case 401:
        case 402:
        case 403:
        case 404:
        case 405:
        case 406:
        case 407:
        case 408:
        case 409:
        case 410:
        case 411:
        case 412:
        case 413:
        case 414:
        case 415:
        case 416:
        case 417:
        case 418:
        case 419:
        case 420:
        case 421:
        case 422:
        case 423:
        case 424:
        case 425:
        case 426:
        case 427:
        case 428:
        case 429:
        case 430:
        case 431:
        case 432:
        case 433:
        case 434:
        case 435:
        case 436:
        case 437:
        case 438:
        case 439:
        case 440:
        case 441:
        case 442:
        case 443:
        case 444:
        case 445:
        case 446:
        case 447:
        case 448:
        case 449:
        case 450:
        case 451:
        case 452:
        case 453:
        case 454:
        case 455:
        case 456:
        case 457:
        case 458:
        case 459:
        case 460:
        case 461:
        case 462:
        case 463:
        case 464:
        case 465:
        case 466:
        case 467:
        case 468:
        case 469:
        case 470:
        case 471:
        case 472:
        case 473:
        case 474:
        case 475:
        case 476:
        case 477:
        case 478:
        case 479:
        case 480:
        case 481:
        case 482:
        case 483:
        case 484:
        case 485:
        case 486:
        case 487:
        case 488:
        case 489:
        case 490:
        case 491:
        case 492:
        case 493:
        case 494:
        case 495:
        case 496:
        case 497:
        case 498:
        case 499:
        case 500:
        case 501:
        case 502:
        case 503:
        case 504:
        case 505:
        case 506:
        case 507:
        case 508:
        case 509:
        case 510:
        case 511:
        case 512:
        case 513:
        case 514:
        case 515:
        case 516:
        case 517:
        case 518:
        case 519:
        case 520:
        case 521:
        case 522:
        case 523:
        case 524:
        case 525:
        case 526:
        case 527:
        case 528:
        case 529:
        case 530:
        case 531:
        case 532:
        case 533:
        case 534:
        case 535:
        case 536:
        case 537:
        case 538:
        case 539:
        case 540:
        case 541:
        case 1724:
        case 1725:
        case 1726:
        case 1727:
        case 1728:
        case 1729:
        case 1730:
        case 1731:
        case 1732:
        case 1733:
        case 1734:
        case 1735:
        case 1736:
        case 1737:
        case 1738:
        case 1739:
        case 1740:
        case 1741:
        case 1742:
        case 1743:
        case 1744:
        case 1745:
        case 1746:
        case 1747:
        case 1748:
        case 1749:
        case 1750:
        case 1751:
        case 1752:
        case 1753:
        case 1754:
        case 1755:
        case 1756:
        case 1757:
        case 1758:
        case 1759:
        case 1760:
        case 1761:
        case 1762:
        case 1763:
        case 1764:
        case 1765:
        case 1766:
        case 1767:
        case 1768:
        case 1769:
        case 1770:
        case 1771:
        case 1772:
        case 1773:
        case 1774:
        case 1775:
        case 1776:
        case 1777:
        case 1778:
        case 1779:
        case 1780:
        case 1781:
        case 1782:
        case 1783:
        case 1784:
        case 1785:
        case 1786:
        case 1787:
        case 1788:
        case 1789:
        case 1790:
        case 1791:
        case 1792:
        case 1793:
        case 1794:
        case 1795:
        case 1796:
        case 1797:
        case 1798:
        case 1799:
        case 1800:
            return true;
        default:
            return false;
    }
}

void GameObject::addGlow(const std::string& frameName) {
    if (m_hasGlow) return;
    
    if (!shouldHaveGlow()) return;
    
    // Construct glow frame name with proper formatting
    std::string baseFrame = frameName;
    std::string colorFrame = getColorFrame(baseFrame);
    std::string glowFrame = getGlowFrame(baseFrame);
    
    createGlow(glowFrame);
    m_hasGlow = true;
}

void GameObject::addEmptyGlow() {
    if (m_hasGlow) return;
    
    createGlow("emptyGlow.png");
    if (m_mainSprite) {
        m_mainSprite->setOpacity(0); // Make invisible but maintain physics
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
    // Format: "_color_" + baseFrame (e.g., "_001.png" -> "_color_001.png")
    size_t dotPos = baseFrame.find_last_of('.');
    std::string frameNum = (dotPos != std::string::npos) ? baseFrame.substr(0, dotPos) : baseFrame;
    return cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
        ("_color_" + frameNum + ".png").c_str()
    );
}

cocos2d::CCSpriteFrame* GameObject::getGlowFrame(const std::string& baseFrame) {
    // Format: "_glow_" + baseFrame (e.g., "_001.png" -> "_glow_001.png")
    size_t dotPos = baseFrame.find_last_of('.');
    std::string frameNum = (dotPos != std::string::npos) ? baseFrame.substr(0, dotPos) : baseFrame;
    return cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
        ("_glow_" + frameNum + ".png").c_str()
    );
}

GameObject::~GameObject() {
    // Release all owned resources
    if (m_mainSprite) {
        m_mainSprite->release();
        m_mainSprite = nullptr;
    }
    
    if (m_colorSprite) {
        m_colorSprite->release();
        m_colorSprite = nullptr;
    }
    
    if (m_secondarySprite) {
        m_secondarySprite->release();
        m_secondarySprite = nullptr;
    }
    
    // Free particle system resources
    if (m_particleString) {
        delete[] m_particleString;
        m_particleString = nullptr;
    }
    
    if (m_particleString2) {
        delete[] m_particleString2;
        m_particleString2 = nullptr;
    }
    
    if (m_particleData) {
        delete[] m_particleData;
        m_particleData = nullptr;
    }
    
    if (m_particleData2) {
        delete[] m_particleData2;
        m_particleData2 = nullptr;
    }
    
    if (m_particleData3) {
        delete[] m_particleData3;
        m_particleData3 = nullptr;
    }
    
    // Clean up physics object if owned by this GameObject
    if (m_physicsObject) {
        // Check if we own this physics object before deleting
        uintptr_t baseAddr = (uintptr_t)cocos2d::CCDirector::sharedDirector();
        uintptr_t physicsAddr = (uintptr_t)m_physicsObject - 0x18;
        if (physicsAddr != *(uintptr_t*)(baseAddr + 0xfe8)) {
            delete (char*)physicsAddr;
        }
        m_physicsObject = nullptr;
    }
    
    // Call parent destructor
    CCSpritePlus::~CCSpritePlus();
}

// EnhancedGameObject.cpp
#include "EnhancedGameObject.hpp"

EnhancedGameObject::~EnhancedGameObject() {
    // No additional cleanup needed beyond base class
    GameObject::~GameObject();
}

// TextGameObject.cpp
#include "TextGameObject.hpp"

TextGameObject::~TextGameObject() {
    // Clean up text content if owned by this object
    if (m_textContent) {
        uintptr_t textAddr = (uintptr_t)m_textContent - 0x18;
        uintptr_t baseAddr = (uintptr_t)cocos2d::CCDirector::sharedDirector();
        if (textAddr != *(uintptr_t*)(baseAddr + 0xfe8)) {
            delete (char*)textAddr;
        }
        m_textContent = nullptr;
    }
    
    // Call base destructor
    GameObject::~GameObject();
}
