#include "GJGameState.h"
#include "GameObject.h"
#include "cocos2d.h"

// This function gets or creates physics data for a GameObject
GameObjectPhysics* GJGameState::getGameObjectPhysics(GameObject* gameObject) {
    // Stack protection
    long stackGuard = __stack_chk_guard;
    
    // ===== STEP 1: CALCULATE HASH TABLE INDEX =====
    int objectID = gameObject->m_physicsID;          // +0x384 in GameObject
    GameObject* targetObject = gameObject;
    
    // Get hash table data
    size_t tableSize = m_physicsTableSize;           // +0x2B8 in GJGameState
    GameObjectPhysics** physicsTable = m_physicsTable; // +0x2B0
    
    // Calculate hash: objectID % tableSize
    size_t index = objectID % tableSize;
    
    // ===== STEP 2: LOOK UP IN HASH TABLE =====
    GameObjectPhysics* physics = physicsTable[index];
    
    // Check if we found the right physics object
    if (physics) {
        GameObjectPhysics* current = physics;
        
        // Traverse collision chain (separate chaining)
        while (current) {
            if (current->m_objectID == objectID) {
                // Found existing physics!
                current->m_gameStateValue = m_someValue;  // +0x220 in GJGameState
                return current;
            }
            
            current = current->m_next;
            if (!current) break;
            
            // Check hash collision
            size_t nextIndex = current->m_objectID % tableSize;
            if (nextIndex != index) {
                break;  // Different hash bucket
            }
        }
    }
    
    // ===== STEP 3: CREATE NEW PHYSICS OBJECT =====
    // Initialize a new physics structure on stack
    CCPoint point1(0, 0);
    CCPoint point2(0, 0);
    
    GameObjectPhysics newPhysics;
    newPhysics.m_next = nullptr;
    newPhysics.m_objectID = objectID;
    newPhysics.m_position = point1;
    newPhysics.m_velocity = point2;
    newPhysics.m_rotation = 0;
    newPhysics.m_angularVelocity = 0;
    newPhysics.m_unknown18 = 0;
    newPhysics.m_unknown1C = 0.0f;
    newPhysics.m_unknown20 = 0.0f;
    
    // ===== STEP 4: ALLOCATE PERMANENT COPY =====
    GameObjectPhysics* allocatedPhysics = new GameObjectPhysics();
    *allocatedPhysics = newPhysics;
    
    try {
        // Insert into hash table
        insertIntoPhysicsTable(allocatedPhysics, index);
        
        // Link to game object
        allocatedPhysics->m_gameObject = targetObject;
        allocatedPhysics->m_gameStateValue = m_someValue;
        
        return allocatedPhysics;
        
    } catch (...) {
        // Cleanup on exception
        delete allocatedPhysics;
        throw;
    }
}

// Helper function to insert into hash table
void GJGameState::insertIntoPhysicsTable(GameObjectPhysics* physics, size_t index) {
    // Hash table uses separate chaining
    if (!m_physicsTable[index]) {
        // Empty bucket
        m_physicsTable[index] = physics;
    } else {
        // Collision - insert at beginning of chain
        physics->m_next = m_physicsTable[index];
        m_physicsTable[index] = physics;
    }
    
    // Update statistics
    m_physicsCount++;
}
