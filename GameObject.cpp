#include "GameObject.h"
#include "CCSpritePlus.h"
#include "cocos2d.h"
#include <vector>
#include <string>
#include <unordered_map>

// ==============================================
// GAMEOBJECT DESTRUCTOR - FULL IMPLEMENTATION
// ==============================================

GameObject::~GameObject() {
    // Stack protector setup
    uint64_t stack_guard = __stack_chk_guard;
    
    // Update vtable pointers (multiple inheritance)
    this->vtable = &GameObject_vtable;                     // +0x0
    this->colorable_vtable = &Colorable_vtable;           // +0x140 offset
    this->blendable_vtable = &Blendable_vtable;           // +0x158 offset
    
    // ===== STEP 1: RELEASE REFERENCE-COUNTED OBJECTS =====
    
    // Physics body (likely Box2D body wrapper)
    if (this->m_physicsBody) {                           // +0x2f0
        this->m_physicsBody->release();
    }
    
    // Custom collision shape
    if (this->m_customCollision) {                       // +0x368
        this->m_customCollision->release();
    }
    
    // Special effect container
    if (this->m_specialEffects) {                        // +0x2e0
        this->m_specialEffects->release();
    }
    
    // ===== STEP 2: CLEAN UP DYNAMIC ARRAYS =====
    
    // Animation timeline data
    if (this->m_animationData) {                         // +0x438
        delete[] this->m_animationData;
    }
    
    // Color transition data
    if (this->m_colorTransitions) {                      // +0x440
        delete[] this->m_colorTransitions;
    }
    
    // ===== STEP 3: CLEAN UP VECTOR-LIKE STRUCTURES =====
    
    // Trigger groups array
    if (this->m_triggerGroups) {                         // +0x478
        delete[] this->m_triggerGroups;
        // Note: There's also m_triggerValues at +0x488
        if (this->m_triggerValues) {                     // +0x488
            delete[] this->m_triggerValues;
        }
    } else {
        // If m_triggerGroups is null, check m_triggerValues directly
        if (this->m_triggerValues) {                     // +0x488
            delete[] this->m_triggerValues;
        }
    }
    
    // Texture coordinate data (UVs)
    if (this->m_textureCoords) {                         // +0x498
        delete[] this->m_textureCoords;
    }
    
    // ===== STEP 4: REFERENCE COUNTED SHARED OBJECT =====
    
    // This is likely a shared texture or shader
    void* sharedObject = this->m_sharedObject;           // +0x310
    sharedObject -= 0x18;  // Adjust to object header
    
    if (sharedObject != global_shared_object_base) {
        // Thread-safe reference counting
        if (pthread_create != nullptr) {
            // Atomic reference count decrement
            uint32_t* refCount = (uint32_t*)((uintptr_t)sharedObject + 0x8);
            uint32_t oldCount;
            bool success;
            
            do {
                oldCount = __atomic_load_n(refCount, __ATOMIC_SEQ_CST);
                uint32_t newCount = oldCount - 1;
                success = __atomic_compare_exchange_n(refCount, &oldCount, 
                                                     newCount, false,
                                                     __ATOMIC_SEQ_CST, __ATOMIC_RELAXED);
            } while (!success);
            
            if (oldCount > 0) {
                // Object still has references
                goto cleanup_complete;
            }
        } else {
            // Non-atomic decrement (single-threaded)
            uint32_t* refCount = (uint32_t*)((uintptr_t)sharedObject - 0x8);
            uint32_t count = *refCount;
            *refCount = count - 1;
            
            if (count > 0) {
                goto cleanup_complete;
            }
        }
        
        // No references left, delete the object
        delete sharedObject;
    }
    
cleanup_complete:
    // ===== STEP 5: CALL PARENT DESTRUCTOR =====
    
    // GameObject inherits from CCSpritePlus
    CCSpritePlus::~CCSpritePlus();
    
    // ===== STEP 6: STACK PROTECTOR CHECK =====
    
    if (stack_guard != __stack_chk_guard) {
        __stack_chk_fail();
    }
}
