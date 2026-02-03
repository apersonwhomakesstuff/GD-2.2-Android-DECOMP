// GJGameState.cpp
#include "GJGameState.hpp"
#include <cstring>
#include <new>

// Helper: allocate with size prefix (matches FUN_00de9b00 pattern)
void* allocateWithSize(size_t size) {
    if (size <= 0x10) {
        return ::operator new(0x10);
    }
    void* ptr = ::operator new(size + 0x10);
    *static_cast<size_t*>(ptr) = size;
    return static_cast<char*>(ptr) + 0x10;
}

// Helper: deallocate with size prefix
void deallocateWithSize(void* ptr) {
    if (!ptr) return;
    void* base = static_cast<char*>(ptr) - 0x10;
    size_t size = *static_cast<size_t*>(base);
    ::operator delete(base, size + 0x10);
}

GJGameState::GJGameState() {
    // Zero-initialize entire object (matches memset pattern)
    std::memset(this, 0, sizeof(GJGameState));

    // Set default floats to 1.0f where observed
    m_gravityScale = 1.0f;

    // Allocate dynamic members
    m_particleString = static_cast<char*>(allocateWithSize(0x10));
    if (m_particleString) {
        std::strcpy(m_particleString, "");
    }

    m_checkpointData = nullptr;
}

GJGameState::GJGameState(const GJGameState& other) {
    // Deep copy
    std::memcpy(this, &other, sizeof(GJGameState));

    // Re-allocate owned pointers
    if (other.m_particleString) {
        size_t len = std::strlen(other.m_particleString) + 1;
        m_particleString = static_cast<char*>(allocateWithSize(len));
        std::strcpy(m_particleString, other.m_particleString);
    } else {
        m_particleString = nullptr;
    }

    // Note: m_checkpointData and containers are not fully copied here
    // In real GD, this is handled by custom serialization
    m_checkpointData = nullptr;
}

GJGameState::~GJGameState() {
    if (m_particleString) {
        deallocateWithSize(m_particleString);
        m_particleString = nullptr;
    }
    if (m_checkpointData) {
        deallocateWithSize(m_checkpointData);
        m_checkpointData = nullptr;
    }
}

GJGameState& GJGameState::operator=(const GJGameState& other) {
    if (this != &other) {
        this->~GJGameState();
        new (this) GJGameState(other);
    }
    return *this;
}

void GJGameState::reset() {
    this->~GJGameState();
    new (this) GJGameState();
}

void GJGameState::saveCheckpoint(void* outBuffer) {
    // In real GD, this serializes to a compact binary format
    // For now, memcpy (unsafe but matches structure)
    std::memcpy(outBuffer, this, sizeof(GJGameState));
}

void GJGameState::loadFromCheckpoint(void* buffer) {
    // Dangerous: assumes same memory layout
    std::memcpy(this, buffer, sizeof(GJGameState));
}
