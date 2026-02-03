#include "main.hpp"
#include "GJBaseGameLayer.hpp"
#include "cocos2d.h"
#include "EnterEffectInstance.h"
#include "GameManager.h"
#include "GameObject.h"
#include "PlayerObject.h"

class GJBaseGameLayer {
public:
    void createPlayer() {
        // Stack protection
        long stackGuard = __stack_chk_guard;

        // ===== CREATE FIRST PLAYER (MAIN PLAYER) =====

        // Get player icon IDs from GameManager
        GameManager* gm = GameManager::sharedState();

        // Calculate player cube icon ID
        int cubeID = gm->playerCube - gm->playerCubeUnlocked;  // 0x264 - 0x268
        int shipID = gm->playerShip - gm->playerShipUnlocked;  // 0x270 - 0x274

        // Create player object
        // Parameters: cubeID, shipID, this, gameLayer, isPlayer2?
        PlayerObject* player1 = PlayerObject::create(cubeID, shipID, this,
                                                    m_gameLayer, !m_isDualMode);

        m_player1 = player1;  // Store at +0xdb8

        // ===== SET PLAYER 1 COLORS =====

        // Get primary color (cube color)
        int primaryColorID = gm->primaryColor - gm->primaryColorUnlocked;  // 0x2c4 - 0x2c8
        ccColor3B primaryColor = GameManager::colorForIdx(primaryColorID);

        // Set player color
        player1->setColor(primaryColor);

        // Get secondary color (ship trail)
        int secondaryColorID = gm->secondaryColor - gm->secondaryColorUnlocked;  // 0x2d0 - 0x2d4
        ccColor3B secondaryColor = GameManager::colorForIdx(secondaryColorID);

        // Set secondary color
        player1->setSecondColor(secondaryColor);

        // Get glow color if enabled
        int glowColorID = gm->glowColor - gm->glowColorUnlocked;  // 0x2dc - 0x2e0
        if (glowColorID >= 0) {
            ccColor3B glowColor = GameManager::colorForIdx(glowColorID);
            player1->enableCustomGlowColor(glowColor);
        }

        // Update glow effect
        player1->updateGlowColor();

        // Add player to game layer (z-order 59 = 0x3b)
        m_gameLayer->addChild(player1, 59);

        // ===== SETUP PLAYER 1 PROPERTIES =====

        player1->m_unknown384 = 1;  // Some flag

        // If not dual mode, add particles
        if (!m_isDualMode) {
            player1->addAllParticles();
        }

        // Check if checkpoint mode is enabled
        bool checkpointMode = GameManager::getGameVariable("0068");
        player1->updateCheckpointMode(checkpointMode);

        // Toggle player mode (cube/ship/ball/etc)
        player1->toggleMode(!m_isDualMode);

        // Set some player property based on dual mode
        char playerProp = m_isDualMode ? m_dualPlayerProperty : 0;  // +0x8a6
        player1->m_unknownC34 = playerProp;

        // ===== CREATE SECOND PLAYER (DUAL MODE) =====

        // Only create player2 if in dual mode
        if (m_isDualMode) {
            // Calculate player icon IDs again (may have changed)
            cubeID = gm->playerCube - gm->playerCubeUnlocked;
            shipID = gm->playerShip - gm->playerShipUnlocked;

            // Create player2
            PlayerObject* player2 = PlayerObject::create(cubeID, shipID, this,
                                                        m_gameLayer, true);  // isPlayer2 = true

            m_player2 = player2;  // Store at +0xdc0

            // ===== SET PLAYER 2 COLORS =====

            // Player2 uses secondary color as primary
            int player2PrimaryColorID = gm->secondaryColor - gm->secondaryColorUnlocked;
            ccColor3B player2PrimaryColor = GameManager::colorForIdx(player2PrimaryColorID);
            player2->setColor(player2PrimaryColor);

            // Player2 uses primary color as secondary
            int player2SecondaryColorID = gm->primaryColor - gm->primaryColorUnlocked;
            ccColor3B player2SecondaryColor = GameManager::colorForIdx(player2SecondaryColorID);
            player2->setSecondColor(player2SecondaryColor);

            // Glow color (same as player1)
            if (glowColorID >= 0) {
                ccColor3B glowColor = GameManager::colorForIdx(glowColorID);
                player2->enableCustomGlowColor(glowColor);
            }

            player2->updateGlowColor();

            // Retain player2 (prevent deletion)
            player2->retain();

            // ===== SETUP PLAYER 2 PROPERTIES =====

            player2->m_unknown14C = 1;      // Some byte flag
            player2->m_unknown384 = 2;      // Different from player1

            // Toggle player mode
            player2->toggleMode(false);     // Player2 always starts as cube?

            // Set player property
            char player2Prop = m_isDualMode ? m_dualPlayerProperty : 0;
            player2->m_unknownC34 = player2Prop;
        }

        // Stack check
        if (stackGuard != __stack_chk_guard) {
            __stack_chk_fail();
        }
    }

    void processAreaMoveGroupAction(cocos2d::CCArray* objects, 
                                   EnterEffectInstance* effect,
                                   const cocos2d::CCPoint& target,
                                   int groupID,
                                   int param_5,
                                   int param_6,
                                   int param_7,
                                   int param_8,
                                   bool param_9,
                                   bool param_10) {
        
        // Get effect data
        auto effectData = effect->getEffectData();  // param_3 + 0xc0
        float effectRadius = effectData->radius;    // +0x768
        
        // Early exit if no radius
        if (effectRadius <= 0.0f) return;
        
        // Update processed count
        int processed = cocos2d::CCArray::count(objects);
        m_processedCount += processed;  // +0x3604
        
        if (!objects) return;
        
        // Get object array data
        uint32_t objectCount = objects->getCount();  // **(uint **)(objects + 0x38)
        if (objectCount == 0) return;
        
        GameObject** objectArray = objects->getObjects();  // *(... + 4)
        GameObject** end = objectArray + objectCount - 1;
        
        int successCount = 0;
        int totalAttempted = 0;
        uint32_t effectIndex = param_9 ? 4 : 0;  // For tracking effect state
        
        // Process each object
        for (GameObject** ptr = objectArray; ptr <= end; ptr++) {
            GameObject* obj = *ptr;
            if (!obj) break;
            
            totalAttempted++;
            
            // Check if object is active
            if (!obj->isActive()) {
                // Check if object is within bounds
                if (obj->isWithinBounds(param_5, param_6, param_7, param_8)) {
                    goto process_object;
                }
                continue;
            }
            
process_object:
            // Check object group
            int currentGroup = m_currentGroup;  // +0x3c8
            int objGroup = obj->getGroup();     // +0x4c8
            
            if (objGroup < currentGroup) {
                // Update object to current group
                updateObjectToCurrentGroup(obj, currentGroup);
            }
            
            // Get object position
            cocos2d::CCPoint objPos = obj->getPosition();  // Virtual call +0x540
            
            // Calculate movement based on effect mode
            float distance = 0.0f;
            float ratio = 0.0f;
            
            // Effect has 3 modes: 0, 1, or 2
            int effectMode = effectData->mode;  // +0x79c
            
            if (effectMode == 1) {
                // Mode 1: Horizontal movement
                float horizontalForce = effect->horizontalForce;  // +0x3c
                float targetX = target.x;
                float objX = objPos.x;
                
                float appliedForce = 0.0f;
                if (horizontalForce != 0.0f) {
                    appliedForce = horizontalForce * getForceMultiplier(obj, 1);
                }
                
                float diff = objX - targetX + appliedForce;
                float movementScale = (diff < 0.0f) ? effect->negativeScale : effect->positiveScale;
                distance = diff * movementScale;
                
                bool shouldMove = (effect->checkXDirection) ? 
                                  (objX <= target.x) : (objX <= objPos.y);
                
            } else if (effectMode == 2) {
                // Mode 2: Vertical movement (similar to mode 1 but vertical)
                // ... similar logic with y coordinates
            } else {
                // Mode 0: Radial movement from center
                float horizontalForce = effect->horizontalForce;
                float verticalForce = effect->verticalForce;  // +0x44
                
                float xOffset = 0.0f;
                if (horizontalForce != 0.0f) {
                    xOffset = horizontalForce * getForceMultiplier(obj, 1);
                }
                
                float yOffset = 0.0f;
                if (verticalForce != 0.0f) {
                    yOffset = verticalForce * getForceMultiplier(obj, 2);
                }
                
                cocos2d::CCPoint adjustedTarget(target.x + xOffset, target.y + yOffset);
                distance = cocos2d::ccpDistance(objPos, adjustedTarget);
            }
            
            // Calculate movement ratio
            float radiusScale = effect->radiusScale;  // +0x34
            float baseRadius = effect->baseRadius;    // +0x30
            
            float appliedRadiusScale = 0.0f;
            if (radiusScale != 0.0f) {
                appliedRadiusScale = radiusScale * getForceMultiplier(obj, 0);
            }
            
            float effectiveRadius = baseRadius + appliedRadiusScale;
            
            // Check if effect has custom radius formula
            if (effectData->hasCustomRadius) {  // +0x50 != 0.0
                // Special formula: (distance/radius - custom) / (1 - custom)
                ratio = (distance / effectiveRadius - effectData->customRadius) / 
                       (1.0f - effectData->customRadius);
            } else {
                // Simple ratio
                ratio = distance / effectiveRadius;
            }
            
            // Clamp ratio
            if (ratio >= 1.0f) {
                ratio = 0.0f;
                if (effectData->invertAtMax) {  // +0x7c8
                    ratio = 1.0f;
                }
                
                if (ratio >= 1.0f) {
                    // Clear effect tracking
                    if (effectData->trackEffectState) {  // +0x7a1
                        effect->clearState(effectIndex);
                    }
                    continue;
                }
            }
            
            if (ratio <= 0.0f) {
                if (effectData->invertAtMax) {
                    ratio = 1.0f - ratio;
                } else {
                    ratio = 0.0f;
                }
            }
            
            // Apply easing
            float easedRatio = ratio;
            
            if (effectData->useAdvancedEasing) {  // +0x7a0
                // Get easing forces
                float forceA = effect->forceA;  // +0x70
                float forceB = effect->forceB;  // +0x6c
                float forceC = effect->forceC;  // +0x68
                float forceD = effect->forceD;  // +0x64
                
                float combinedForceX = forceA + getForceMultiplier(obj, 0, 0x1108) * forceB;
                float combinedForceY = forceC + getForceMultiplier(obj, 0, 0x1104) * forceD;
                
                if (combinedForceX == 0.0f && combinedForceY == 0.0f) {
                    // No forces to apply
                    continue;
                }
                
                if (effectData->trackEffectState) {
                    // Check if we should toggle effect state
                    if (ratio > 0.01f && ratio < 0.99f) {
                        int currentState = effect->getState(effectIndex);
                        int toggleMode = effect->toggleMode;  // +0xd8
                        
                        if (currentState == toggleMode - 2) {
                            effect->setState(effectIndex, 0);
                        } else if (currentState == toggleMode - 1) {
                            effect->setState(effectIndex, 1);
                        } else {
                            effect->setState(effectIndex, toggleMode + (currentState ? 1 : 0));
                        }
                    }
                    
                    // Use different easing based on state
                    if (effect->getState(effectIndex) != 0) {
                        easedRatio = getEnterEasingValue(ratio, 
                                                         effectData->easeType,    // +0x76c
                                                         effectData->easeRate,    // +0x770  
                                                         effectData->easeStrength // +0x774
                                                        );
                    } else {
                        easedRatio = getEnterEasingValue(ratio,
                                                         effectData->altEaseType,    // +0x778
                                                         effectData->altEaseRate,    // +0x77c
                                                         effectData->altEaseStrength // +0x780
                                                        );
                    }
                } else {
                    easedRatio = getEnterEasingValue(ratio,
                                                     effectData->easeType,
                                                     effectData->easeRate,
                                                     effectData->easeStrength);
                }
                
                // Apply forces with eased ratio
                float finalForceX = (1.0f - easedRatio) * combinedForceX;
                float finalForceY = (1.0f - easedRatio) * combinedForceY;
                
                // Apply movement
                if (finalForceX != 0.0f || finalForceY != 0.0f) {
                    if (objGroup < currentGroup) {
                        updateObjectToCurrentGroup(obj, currentGroup);
                    }
                    
                    // Apply vertical force
                    if (finalForceY != 0.0f) {
                        obj->addYPosition(finalForceY);
                    }
                    
                    // Mark object as dirty
                    obj->dirtifyObjectPos();
                    obj->dirtifyObjectRect();
                    
                    // Apply horizontal force (if object allows it)
                    if (finalForceX != 0.0f && !obj->isXMovementLocked()) {
                        obj->addXPosition(finalForceX);
                    }
                    
                    // Trigger object callback
                    triggerObjectAction(obj);
                }
                
            } else {
                // Simple easing with directional forces
                float directionForce = effect->directionForce;  // +0x58
                float baseDirection = effect->baseDirection;    // +0x54
                
                float appliedDirection = 0.0f;
                if (directionForce != 0.0f) {
                    appliedDirection = directionForce * getForceMultiplier(obj, 10);
                }
                
                float finalDirection = baseDirection + appliedDirection;
                
                if (finalDirection != 0.0f) {
                    // Complex directional movement with rotation
                    cocos2d::CCPoint movementDirection(0, 0);
                    
                    if (effectData->useObjectDirection) {  // +0x764
                        // Calculate direction from object to target
                        cocos2d::CCPoint dir = objPos - target;
                        float length = dir.length();
                        
                        if (length > 0.0f) {
                            dir.x /= length;
                            dir.y /= length;
                            movementDirection = dir;
                        }
                        
                        if (length < effectRadius) {
                            finalDirection *= (length / effectRadius);
                        }
                        
                    } else if (effectData->useFixedDirection) {  // +0x758
                        // Fixed angle direction
                        float angle = (effect->angle + getForceMultiplier(obj, 11) * effect->angleScale - 90.0f) * 0.017453292f;
                        movementDirection = cocos2d::ccpForAngle(angle);
                    } else {
                        // Predefined direction
                        movementDirection = effect->predefinedDirection;  // +0x75c
                    }
                    
                    easedRatio = getEnterEasingValue(ratio,
                                                     effectData->easeType,
                                                     effectData->easeRate,
                                                     effectData->easeStrength);
                    
                    finalDirection *= (1.0f - easedRatio);
                    
                    // Apply directional movement
                    float finalForceX = finalDirection * movementDirection.x;
                    float finalForceY = finalDirection * movementDirection.y;
                    
                    // ... similar application logic
                }
            }
            
            successCount++;
            effectIndex += 4;  // Move to next effect slot
        }
        
        // Update statistics
        m_processedCount += successCount;      // +0x3604
        m_totalProcessed += totalAttempted;    // +0x3614
    }
    
private:
    PlayerObject* m_player1;       // +0xdb8
    PlayerObject* m_player2;       // +0xdc0
    CCLayer* m_gameLayer;          // +0xff0
    bool m_isDualMode;             // +0x30cc (byte)
    char m_dualPlayerProperty;     // +0x8a6 (byte)
    float getForceMultiplier(GameObject* obj, int multiplierIndex) {
        int groupIndex = obj->getEffectGroup();  // +0x3dc
        return m_forceMultipliers[groupIndex + multiplierIndex];  // +0x10e4
    }
    
    void updateObjectToCurrentGroup(GameObject* obj, int currentGroup) {
        // Update object group and save previous state
        if (!obj->isGroupFrozen()) {  // +0x4fa
            // Save previous group position
            if (obj->getPreviousGroup() != currentGroup) {
                obj->savePreviousPosition();
                obj->dirtifyObjectRect();
            }
            
            // Save previous rotation
            if (obj->getPreviousRotationGroup() != currentGroup) {
                obj->savePreviousRotation();
                obj->dirtifyObjectRect();
            }
        }
        
        // Reset accumulated forces
        obj->resetForces();
        
        // Add to group update list
        addToUpdateList(obj);  // Uses vector at +0xe40
        
        // Update group ID
        obj->setGroup(currentGroup);
    }
    
    void addToUpdateList(GameObject* obj) {
        // Adds object to vector for later processing
        // Vector at +0xe40, size at +0xef4, capacity at +0xf08
        // Standard vector push_back logic
    }
    
    int m_processedCount;          // +0x3604
    int m_totalProcessed;          // +0x3614
    int m_currentGroup;            // +0x3c8
    float m_forceMultipliers[32];  // +0x10e4 - array of force multipliers
};
