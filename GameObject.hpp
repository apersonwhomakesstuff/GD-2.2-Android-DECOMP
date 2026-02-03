// GameObject.hpp
#pragma once

#include "main.hpp"
#include <string>

// Forward declarations
class GJSpriteColor;
class PlayerObject;

class GameObject : public cocos2d::CCNode {
protected:
    // --- Verified Offsets (from disassembly) ---

    // Sprites
    cocos2d::CCSprite* m_secondarySprite;   // 0x2e0
    cocos2d::CCSprite* m_mainSprite;        // 0x2f0
    void*              m_physicsObject;     // 0x310
    cocos2d::CCSprite* m_colorSprite;       // 0x368

    // Core IDs & state
    int                m_objectID;          // 0x34
    int                m_uniqueID;          // 0x384
    bool               m_hasGlow;           // 0x289
    bool               m_highDetail;        // 0x420
    bool               m_dontFade;          // 0x4fe
    int                m_objectType;        // 0x3f4

    // Particle system data
    char*              m_particleString;    // 0x438
    char*              m_particleString2;   // 0x440
    int*               m_particleData;      // 0x478
    int*               m_particleData2;     // 0x488
    int*               m_particleData3;     // 0x498

public:
    // --- Constructors / Destructors ---
    GameObject();
    virtual ~GameObject();

    // --- Static Utilities ---
    static void resetMID();
    void assignUniqueID();

    // --- Glow System ---
    void createGlow(const std::string& frameName);
    void addGlow(const std::string& frameName);
    void addEmptyGlow();
    void removeGlow();
    void removeColorSprite();

    // --- Frame Helpers ---
    static cocos2d::CCSpriteFrame* getColorFrame(const std::string& baseFrame);
    static cocos2d::CCSpriteFrame* getGlowFrame(const std::string& baseFrame);

    // --- Logic & Queries ---
    bool shouldHaveGlow() const;
    bool canChangeMainColor() const;

    // --- Color & Mode ---
    GJSpriteColor* getMainColor();
    GJSpriteColor* getSecondaryColor();
    int getMainColorMode();
    int getSecondaryColorMode();
    void setDefaultMainColorMode(int mode);
    void setDefaultSecondaryColorMode(int mode);

    // --- Position & Orientation ---
    void updateStartPos();
    void updateStartValues();
    void updateUnmodifiedPositions();
    void quickUpdatePosition2();
    void addToTempOffset(double dx, double dy);
    void dirtifyObjectRect();
    void updateIsOriented();
    void updateOrientedBox();
    void calculateOrientedBox();
    cocos2d::CCRect* getObjectRectPointer();

    // --- Rendering & Visibility ---
    void makeVisible();
    void makeInvisible();
    void setOpacity(unsigned char opacity);
    void setGlowOpacity(unsigned char opacity);
    void updateBlendMode();
    void resetObject();

    // --- Parenting & Layers ---
    int getParentMode();
    cocos2d::CCNode* parentForZLayer(int zLayer, bool foreground, int groupID);
    void addMainSpriteToParent(bool force = false);
    void addColorSpriteToParent(bool force = false);

    // --- Particles & Effects ---
    void claimParticle();
    void createAndAddParticle(const std::string& key, cocos2d::CCPoint pos, float unk, bool unk2);

    // --- Slopes & Direction ---
    bool isFacingDown();
    int getObjectDirection();
    float slopeFloorTop();
    float slopeWallLeft();
    float slopeYPos(GameObject* other);

    // --- Groups & ID ---
    int getGroupID();
    void copyGroups(GameObject* other);
    void createGroupContainer(int groupID);

    // --- Editor & Customization ---
    void updateObjectEditorColor();
    int editorColorForMode(int mode);
    int editorColorForCustomMode(int mode);
    void updateHSVState();
    void addCustomChild(const std::string& frame, cocos2d::CCPoint offset);
    void addCustomBlackChild(const std::string& frame, float scale, bool rotate);

    // --- Scale & Rotation ---
    void updateCustomScaleX(float scale);
    void updateCustomScaleY(float scale);
    void addRotation(float degrees);
    void resetRScaleForced();
    bool didScaleXChange();

    // --- Flags & State ---
    bool ignoreEnter();
};

// EnhancedGameObject.hpp
class EnhancedGameObject : public GameObject {
public:
    EnhancedGameObject();
    virtual ~EnhancedGameObject();

    bool hasBeenActivatedByPlayer(PlayerObject* player);
    void resetObject() override;
};

// TextGameObject.hpp
class TextGameObject : public GameObject {
private:
    char* m_textContent; // 0x538

public:
    TextGameObject();
    virtual ~TextGameObject();
};
