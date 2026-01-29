#include "GJEffectManager.h"
#include "GroupCommandObject2.h"
#include <unordered_map>
#include <map>
#include <vector>
#include <set>

// ==============================================
// FULL GJEffectManager STRUCTURE (FROM DESTRUCTOR)
// ==============================================

GJEffectManager::~GJEffectManager() {
    // Step 1: Release basic CCObject references
    if (m_effectContainer) {
        m_effectContainer->release();
    }
    if (m_particleContainer) {
        m_particleContainer->release();
    }
    if (m_soundContainer) {
        m_soundContainer->release();
    }
    if (m_colorContainer) {
        m_colorContainer->release();
    }
    if (m_followContainer) {
        m_followContainer->release();
    }
    
    // Step 2: Clean up ActiveEffectContainers vector (0x530-0x538)
    // This is a vector of ActiveEffectContainer pointers
    for (auto* container : m_activeEffectContainers) {
        if (container) {
            // Each ActiveEffectContainer has internal data structures
            // that need cleanup
            
            // Clean dynamic array at +0x1E8
            if (container->m_dynamicArray) {
                delete[] container->m_dynamicArray;
            }
            
            // Clean 13 internal vector-like structures (0x1C0 bytes each)
            for (auto& innerStruct : container->m_internalVectors) {
                // Each inner struct has 11 dynamic arrays
                if (innerStruct.m_array1) delete[] innerStruct.m_array1;
                if (innerStruct.m_array2) delete[] innerStruct.m_array2;
                if (innerStruct.m_array3) delete[] innerStruct.m_array3;
                if (innerStruct.m_array4) delete[] innerStruct.m_array4;
                if (innerStruct.m_array5) delete[] innerStruct.m_array5;
                if (innerStruct.m_array6) delete[] innerStruct.m_array6;
                if (innerStruct.m_array7) delete[] innerStruct.m_array7;
                if (innerStruct.m_array8) delete[] innerStruct.m_array8;
                if (innerStruct.m_array9) delete[] innerStruct.m_array9;
                if (innerStruct.m_array10) delete[] innerStruct.m_array10;
                if (innerStruct.m_array11) delete[] innerStruct.m_array11;
            }
            
            // Clean the vector storage itself
            if (container->m_internalVectors.data()) {
                delete[] container->m_internalVectors.data();
            }
            
            delete container;
        }
    }
    
    // Step 3: Clean up PendingEffectContainers vector (0x518-0x520)
    // Same structure as active containers
    for (auto* container : m_pendingEffectContainers) {
        if (container) {
            // Same cleanup pattern as above
            if (container->m_dynamicArray) {
                delete[] container->m_dynamicArray;
            }
            
            for (auto& innerStruct : container->m_internalVectors) {
                if (innerStruct.m_array1) delete[] innerStruct.m_array1;
                // ... all 11 arrays
            }
            
            if (container->m_internalVectors.data()) {
                delete[] container->m_internalVectors.data();
            }
            
            delete container;
        }
    }
    
    // Step 4: Clean up GameObject vectors (0x548-0x550)
    // Vector of GameObject pointers
    for (auto* obj : m_affectedObjects) {
        if (obj) {
            obj->release();
        }
    }
    if (m_affectedObjects.data()) {
        delete[] m_affectedObjects.data();
    }
    
    // Step 5: Clean up linked lists of EffectNodes
    // Three linked lists at offsets 0x628, 0x660, 0x698
    
    // First linked list (probably running effects)
    auto* node = m_runningEffectsHead;
    while (node) {
        auto* next = node->next;
        if (node->effectData) {
            node->effectData->release();
        }
        delete node;
        node = next;
    }
    
    // Second linked list (probably pending effects)
    node = m_pendingEffectsHead;
    while (node) {
        auto* next = node->next;
        if (node->effectData) {
            node->effectData->release();
        }
        delete node;
        node = next;
    }
    
    // Third linked list (probably delayed effects)
    node = m_delayedEffectsHead;
    while (node) {
        auto* next = node->next;
        if (node->effectData) {
            node->effectData->release();
        }
        delete node;
        node = next;
    }
    
    // Step 6: Clean up 3 std::unordered_maps (hashtables)
    // These are at offsets 0x618, 0x650, 0x688
    
    // Effect ID to GameObject map
    m_effectIDToObjectMap.clear();
    m_effectIDToObjectMap.~unordered_map();
    
    // Group ID to Effects map
    m_groupIDToEffectsMap.clear();
    m_groupIDToEffectsMap.~unordered_map();
    
    // Object ID to Effects map
    m_objectIDToEffectsMap.clear();
    m_objectIDToEffectsMap.~unordered_map();
    
    // Step 7: Clean up various dynamic arrays
    if (m_effectTimelineData) delete[] m_effectTimelineData;
    if (m_colorTransitionData) delete[] m_colorTransitionData;
    if (m_moveTransitionData) delete[] m_moveTransitionData;
    if (m_scaleTransitionData) delete[] m_scaleTransitionData;
    if (m_rotateTransitionData) delete[] m_effectTimelineData; // Note: duplicate offset in disassembly
    
    // Step 8: Clean up std::map (red-black tree) at 0x720
    // This is probably for sorted effects by time
    m_sortedEffectsByTime.clear();
    m_sortedEffectsByTime.~map();
    
    if (m_sortedEffectsStorage) delete[] m_sortedEffectsStorage;
    
    // Step 9: Clean up more hashtables (total 11+ hashtables in the class!)
    // Offsets: 0x5c8, 0x578, 0x460, 0x3f8, 0x3c0, 0x388, 0x350, 0x318, 0x288, 0x1f8, 0x1a8, 0x170
    
    m_triggerMap.clear();
    m_delayMap.clear();
    m_repeatMap.clear();
    m_easingMap.clear();
    m_blendMap.clear();
    m_persistMap.clear();
    m_lockMap.clear();
    m_targetMap.clear();
    m_channelMap.clear();
    m_layerMap.clear();
    m_orderMap.clear();
    m_priorityMap.clear();
    
    // Step 10: Clean up command queue vectors
    // This is our GroupCommandObject2 vector at offset 0x560-0x568
    for (auto& cmd : m_commandQueue) {
        // GroupCommandObject2 destructor handles its own cleanup
        cmd.~GroupCommandObject2();
    }
    if (m_commandQueue.data()) {
        delete[] m_commandQueue.data();
    }
    
    // Step 11: Clean up more internal structures
    // Arrays of structs at various offsets
    
    // At offset 0x500-0x508 (48-byte structs)
    for (auto& effectTrack : m_effectTracks) {
        if (effectTrack.timelineData) {
            delete[] effectTrack.timelineData;
        }
    }
    if (m_effectTracks.data()) {
        delete[] m_effectTracks.data();
    }
    
    // At offset 0x4c8 (hashtable)
    m_trackMap.clear();
    m_trackMap.~unordered_map();
    
    // At offset 0x498 (red-black tree)
    m_trackTree.clear();
    m_trackTree.~map();
    
    // Step 12: Many more cleanup operations...
    // The destructor shows at least 20+ different data structures!
    
    // Call parent destructor (CCNode)
    CCNode::~CCNode();
}
