#include "main.hpp"
#include "PlayerObject.hpp"

class PlayerObject : public GameObject {
public:
    virtual ~PlayerObject() {
        // Stack canary protection
        long stackGuard = __stack_chk_guard;
        
        // ===== STEP 1: VTABLE SETUP =====
        // Set vtable pointers (inherited from GameObject)
        this->vtable = &PlayerObject::vtable_start;           // +0x0
        this->vtable_offset_0x140 = &PlayerObject::setColor;  // +0x140
        this->vtable_offset_0x158 = &PlayerObject::setBlendFunc; // +0x158
        this->vtable_offset_0x538 = &PlayerObject::data_0x538; // +0x538
        
        // ===== STEP 2: CLEAN UP PARTICLE ARRAYS =====
        // Release all particles in m_particleArray (+0x678)
        CCArray* particleArray = m_particleArray;
        if (particleArray) {
            // Call release() on each particle
            for (int i = 0; i < particleArray->count(); i++) {
                CCObject* particle = particleArray->objectAtIndex(i);
                if (particle) {
                    particle->release();
                }
            }
            particleArray->release();  // Release the array itself
        }
        
        // ===== STEP 3: RELEASE CCOBJECT POINTERS =====
        // Release all CCObject references
        if (m_pointer_0x598) m_pointer_0x598->release();
        if (m_pointer_0x5a0) m_pointer_0x5a0->release();
        if (m_pointer_0x5a8) m_pointer_0x5a8->release();
        if (m_pointer_0x5b0) m_pointer_0x5b0->release();
        if (m_pointer_0x590) m_pointer_0x590->release();
        if (m_pointer_0xa08) m_pointer_0xa08->release();
        if (m_pointer_0xc20) m_pointer_0xc20->release();
        if (m_pointer_0xbe8) m_pointer_0xbe8->release();
        if (m_pointer_0xbf0) m_pointer_0xbf0->release();
        
        // ===== STEP 4: UNLOAD PLAYER ICONS =====
        GameManager* gm = GameManager::sharedState();
        gm->unloadIcons(m_iconID);  // +0xbe4
        
        // ===== STEP 5: CLEAN UP CONTAINERS =====
        // Release some ref-counted object at +0xbd8
        // (Has reference counting with atomic operations)
        
        // Delete red-black tree at +0xbb0
        deleteRedBlackTree(m_rbTree_0xbb0);
        
        // Delete another red-black tree at +0xb70
        deleteRedBlackTree(m_rbTree_0xb70);
        
        // Delete hash table at +0xa88
        deleteHashTable(m_hashTable_0xa88);
        
        // ===== STEP 6: DESTROY HASHTABLES =====
        // Destroy various hashtables
        m_hashtable_0xa10.~Hashtable();
        m_hashtable_0x820.~Hashtable();
        m_hashtable_0x6c0.~Hashtable();
        m_hashtable_0x688.~Hashtable();
        
        // ===== STEP 7: CALL PARENT DESTRUCTOR =====
        GameObject::~GameObject();
        
        // ===== STEP 8: STACK CHECK =====
        if (stackGuard != __stack_chk_guard) {
            __stack_chk_fail();
        }
    }
    
    // Destructor for delete operator
    virtual ~PlayerObject() {
        // Calls the real destructor then operator delete
        this->~PlayerObject();
        operator delete(this);
    }

private:
    // Member offsets from assembly:
    CCArray* m_particleArray;      // +0x678
    CCObject* m_pointer_0x598;     // +0x598
    CCObject* m_pointer_0x5a0;     // +0x5a0
    CCObject* m_pointer_0x5a8;     // +0x5a8
    CCObject* m_pointer_0x5b0;     // +0x5b0
    CCObject* m_pointer_0x590;     // +0x590
    CCObject* m_pointer_0xa08;     // +0xa08
    CCObject* m_pointer_0xc20;     // +0xc20
    CCObject* m_pointer_0xbe8;     // +0xbe8
    CCObject* m_pointer_0xbf0;     // +0xbf0
    
    int m_iconID;                  // +0xbe4
    CCObject* m_refCountedObject;  // +0xbd8 (with atomic ref counting)
    
    // Containers
    std::_Rb_tree<> m_rbTree_0xbb0;  // Red-black tree
    std::_Rb_tree<> m_rbTree_0xb70;  // Another red-black tree
    void* m_hashTable_0xa88;         // Hash table pointer
    
    // More hashtables (in-place)
    std::_Hashtable<> m_hashtable_0xa10;
    std::_Hashtable<> m_hashtable_0x820;
    std::_Hashtable<> m_hashtable_0x6c0;
    std::_Hashtable<> m_hashtable_0x688;
    
    // Vtable offsets
    void* vtable_offset_0x140;      // setColor function
    void* vtable_offset_0x158;      // setBlendFunc function
    void* vtable_offset_0x538;      // Some data pointer
};

// Helper functions
void deleteRedBlackTree(std::_Rb_tree<>& tree) {
    // Recursively delete nodes
    auto deleteNode = [&](auto node, auto deleteFunc) {
        if (!node) return;
        deleteNode(node->left, deleteFunc);
        deleteNode(node->right, deleteFunc);
        deleteFunc(node);
    };
    
    deleteNode(tree.root(), [](auto node) { delete node; });
}

void deleteHashTable(void* table) {
    if (table) {
        // Hash table cleanup logic
        operator delete(table);
    }
}
