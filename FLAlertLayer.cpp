#include "FLAlertLayer.hpp"
#include "GameManager.hpp"  // For getGameVariable("0079")

using namespace cocos2d;

// ============================================================================
// Constructor / Destructor
// ============================================================================

FLAlertLayer::FLAlertLayer()
    : m_mainLayer(nullptr)
    , m_delegate(nullptr)
    , m_scene(nullptr)
    , m_noElastic(false)
    , m_alertNode(nullptr)
    , m_zOrder(-1)
    , m_isShowing(false)
    , m_touchEnabledNode(nullptr)
    , m_touchID1(-1)
    , m_touchID2(-1)
    , m_forcePrioRegistered(false)
{
    // Cocos2d-x requires explicit touch/keyboard enablement
    setTouchEnabled(true);
    setKeypadEnabled(true);
}

FLAlertLayer::~FLAlertLayer() {
    if (m_forcePrioRegistered) {
        CCTouchDispatcher::sharedDispatcher()->unregisterForcePrio(this);
    }
    
    CC_SAFE_RELEASE(m_alertNode);
}

// ============================================================================
// Factory Method
// ============================================================================

FLAlertLayer* FLAlertLayer::create(FLAlertLayerProtocol* delegate,
                                   const char* title,
                                   const char* btn1,
                                   const char* btn2,
                                   float width,
                                   bool noElastic,
                                   float height,
                                   float zOrder) {
    FLAlertLayer* pRet = new FLAlertLayer();
    if (pRet && pRet->init(delegate, title, btn1, btn2, width, noElastic, height, zOrder)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

// ============================================================================
// Initialization
// ============================================================================

bool FLAlertLayer::init(int width) {
    // Initialize semi-transparent black background
    if (!CCLayerColor::initWithColor(ccc4(0, 0, 0, 128))) {
        return false;
    }

    // Check "no-pause-menu" game variable (0079)
    auto gm = GameManager::sharedState();
    if (gm && gm->getGameVariable("0079")) {
        m_noElastic = true;
    }

    incrementForcePrio();

    // Create container for alert content
    m_alertNode = CCLayer::create();
    m_alertNode->retain();
    addChild(m_alertNode);

    // Initialize touch tracking
    m_touchID1 = -1;
    m_touchID2 = -1;

    return true;
}

bool FLAlertLayer::init(FLAlertLayerProtocol* delegate,
                        const char* title,
                        const char* btn1,
                        const char* btn2,
                        float width,
                        bool noElastic,
                        float height,
                        float zOrder) {
    // Geometry Dash passes width as integer cast to float
    if (!init(static_cast<int>(width))) {
        return false;
    }

    m_delegate = delegate;
    m_noElastic = noElastic;
    m_zOrder = static_cast<int>(zOrder);
    
    // Note: Full UI construction (labels/buttons) omitted for brevity
    // Actual implementation would create title label and buttons here
    
    return true;
}

// ============================================================================
// Display Management
// ============================================================================

void FLAlertLayer::show() {
    if (m_isShowing) return;
    m_isShowing = true;

    // Get current running scene
    CCDirector* director = CCDirector::sharedDirector();
    m_scene = director->getRunningScene();
    
    // Determine Z-order if not specified
    if (m_zOrder == -1) {
        m_zOrder = m_scene->getHighestChildZ() + 1;
        if (m_zOrder <= 104) {  // 0x68
            m_zOrder = 105;     // Default minimum Z-order
        }
    }

    // Add to scene at calculated Z-order
    m_scene->addChild(this, m_zOrder);

    // Show animation: elastic entrance + fade-in
    if (!m_noElastic) {
        // Scale animation: 0.6 -> 1.1 with elastic easing
        CCScaleTo* scaleTo = CCScaleTo::create(0.15f, 1.1f);
        CCEaseElasticOut* elastic = CCEaseElasticOut::create(scaleTo, 0.35f);
        m_alertNode->setScale(0.6f);
        m_alertNode->runAction(elastic);
    }

    // Fade in background from transparent to semi-transparent
    CCFadeTo* fadeIn = CCFadeTo::create(0.15f, 128);
    runAction(fadeIn);

    // Register for touch events
    registerWithTouchDispatcher();
}

void FLAlertLayer::keyBackClicked() {
    // Back button behavior: dismiss via button 2 if noElastic, else button 1
    if (m_noElastic) {
        onBtn2(nullptr);
    } else {
        onBtn1(nullptr);
    }
}

// ============================================================================
// Button Callbacks
// ============================================================================

void FLAlertLayer::onBtn1(CCObject* sender) {
    // Unregister from touch dispatcher before dismissal
    CCTouchDispatcher::sharedDispatcher()->unregisterForcePrio(this);
    
    // Notify delegate (btn1 = true means "Cancel/No" button)
    if (m_delegate) {
        m_delegate->FLAlert_Clicked(this, true);
    }
    
    // Remove from parent with cleanup
    removeFromParentAndCleanup(true);
}

void FLAlertLayer::onBtn2(CCObject* sender) {
    // Unregister from touch dispatcher before dismissal
    CCTouchDispatcher::sharedDispatcher()->unregisterForcePrio(this);
    
    // Notify delegate (btn2 = false means "OK/Yes" button)
    if (m_delegate) {
        m_delegate->FLAlert_Clicked(this, false);
    }
    
    // Remove from parent with cleanup
    removeFromParentAndCleanup(true);
}

// ============================================================================
// Touch Handling
// ============================================================================

void FLAlertLayer::registerWithTouchDispatcher() {
    // Register with high priority (-501) to capture touches over other UI
    CCTouchDispatcher::sharedDispatcher()->addPrioTargetedDelegate(
        this,
        -501,  // 0xFFFFFE0B sign-extended
        true   // Swallows touches
    );
}

void FLAlertLayer::incrementForcePrio() {
    if (m_forcePrioRegistered) return;
    
    m_forcePrioRegistered = true;
    CCTouchDispatcher::sharedDispatcher()->registerForcePrio(this, 2);
}

bool FLAlertLayer::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    int touchID = touch->getID();
    
    // Primary touch: background dismissal area
    if (m_touchID1 == -1) {
        // If no main content layer OR touch outside main content area
        if (!m_mainLayer || !m_mainLayer->boundingBox().containsPoint(touch->getLocation())) {
            m_touchID1 = touchID;
            return true;  // Claim touch for background dismissal
        }
    }
    
    // Secondary touch: passthrough area (e.g., text input fields)
    if (m_touchEnabledNode && m_touchID2 == -1) {
        if (m_touchEnabledNode->boundingBox().containsPoint(touch->getLocation())) {
            m_touchID2 = touchID;
            return true;  // Claim touch for passthrough node
        }
    }
    
    return false;  // Don't claim this touch
}

void FLAlertLayer::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    int touchID = touch->getID();
    
    if (touchID == m_touchID1) {
        // Background touch ended - dismiss alert via "Cancel" button
        m_touchID1 = -1;
        onBtn1(nullptr);
    } 
    else if (touchID == m_touchID2 && m_touchEnabledNode) {
        // Passthrough node touch ended - forward event (simplified)
        m_touchID2 = -1;
        // In full implementation: m_touchEnabledNode->ccTouchEnded(touch, event);
    }
}

void FLAlertLayer::ccTouchCancelled(CCTouch* touch, CCEvent* event) {
    int touchID = touch->getID();
    
    if (touchID == m_touchID1) m_touchID1 = -1;
    else if (touchID == m_touchID2) m_touchID2 = -1;
}

void FLAlertLayer::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    // No-op in original implementation - just track touch state
}
