#include "cocos2d.h"
#include "GameManager.h"
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
    
private:
    PlayerObject* m_player1;       // +0xdb8
    PlayerObject* m_player2;       // +0xdc0
    CCLayer* m_gameLayer;          // +0xff0
    bool m_isDualMode;             // +0x30cc (byte)
    char m_dualPlayerProperty;     // +0x8a6 (byte)
    
    // Color offsets in GameManager:
    // 0x264 - playerCube
    // 0x268 - playerCubeUnlocked
    // 0x270 - playerShip  
    // 0x274 - playerShipUnlocked
    // 0x2c4 - primaryColor
    // 0x2c8 - primaryColorUnlocked
    // 0x2d0 - secondaryColor
    // 0x2d4 - secondaryColorUnlocked
    // 0x2dc - glowColor
    // 0x2e0 - glowColorUnlocked
};

// PlayerObject methods called:
class PlayerObject : public GameObject {
public:
    static PlayerObject* create(int cubeID, int shipID, 
                               GJBaseGameLayer* gameLayer, 
                               CCLayer* parentLayer, 
                               bool isPlayer2);
    
    void setColor(const ccColor3B& color);           // Virtual +0x3b0
    void setSecondColor(const ccColor3B& color);     // +0x???
    void enableCustomGlowColor(const ccColor3B& color);
    void updateGlowColor();
    void addAllParticles();
    void updateCheckpointMode(bool enabled);
    void toggleMode(bool mode);
    
    // Unknown properties:
    int m_unknown384;    // Some flag (1 for player1, 2 for player2)
    char m_unknown14C;   // Byte flag
    char m_unknownC34;   // Some property from dual mode
};
