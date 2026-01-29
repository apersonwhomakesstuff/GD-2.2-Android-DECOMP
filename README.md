# GEOMETRY DASH REVERSE ENGINEERING - GJBaseGameLayer & GameObject 🚀

![Geometry Dash](https://img.shields.io/badge/Geometry-Dash-blue)
![Reverse Engineering](https://img.shields.io/badge/Reverse-Engineering-red)
![C++](https://img.shields.io/badge/C++-17-blue)
![ARM64](https://img.shields.io/badge/ARM64-Assembly-green)

Complete C++ recreation of Geometry Dash's GameObject and GJBaseGameLayer classes from ARM64 disassembly.

## 📁 Project Structure


## 🎯 Features

### GameObject Class
- ✅ Pure virtual methods from vtable
- ✅ Binary-accurate member offsets (`0x350`, `0x351`, `0x388`)
- ✅ ARM64 assembly translation
- ✅ Virtual function calls at `+0xC0` in vtable
- ✅ Object flags and state management

### GJBaseGameLayer Class  
- ✅ Massive destructor with proper cleanup
- ✅ Custom enter effects system
- ✅ Area move group action processing
- ✅ Event linking/unlinking
- ✅ Song trigger management

### Area Effects System
```cpp
// Three movement modes:
// 0 - Radial movement from center
// 1 - Horizontal movement only  
// 2 - Vertical movement only

// Complex easing with toggling
// Force multipliers per object group
// Directional movement calculations
```

## 📊 Memory Layout

### GameObject Offsets
```
0x350 - bool m_objectRectDirty
0x351 - bool m_orientedRectDirty  
0x388 - int m_type (GameObjectType)
0x3dc - short m_effectGroup
0x4c8 - int m_currentGroup
```

### GJBaseGameLayer Key Offsets
```
0x3604 - int m_processedCount
0x3614 - int m_totalProcessed
0x3c8  - int m_currentGroup
0x10e4 - float m_forceMultipliers[32]
0xe40  - std::vector<GameObject*> m_updateList
```

## 🧪 Testing

```cpp
// Unit tests for binary accuracy
TEST(GameObject, Offsets) {
    GameObject obj;
    ASSERT_EQ(offsetof(GameObject, m_objectRectDirty), 0x350);
    ASSERT_EQ(offsetof(GameObject, m_type), 0x388);
}

// Test area effects
TEST(GJBaseGameLayer, AreaMove) {
    GJBaseGameLayer layer;
    CCArray* objects = CCArray::create();
    EnterEffectInstance* effect = new EnterEffectInstance();
    
    layer.processAreaMoveGroupAction(objects, effect, CCPoint(100, 100));
    // Verify object movement
}
```

## 🛠️ Development Tools

- **IDA Pro 7.7+** - Disassembly analysis
- **Ghidra** - Alternative decompiler  
- **Binary Ninja** - For ARM64 analysis
- **Frida** - Runtime instrumentation
- **Custom ARM64 decoder** - For instruction analysis

## 📈 Progress

### 🧱 Tier 1: Layout & Lifetime (stable for modding)
- [x] GameObject class structure (binary-accurate offsets)
- [x] GameObject destructor (verified ARM64 behavior)
- [x] PlayerObject destructor and resource cleanup
- [x] Physics table (lookup + insertion logic)
- [x] GJEffectManager structure and destructor (full container map)
- [x] Core containers (vectors, maps, trees, linked lists)
- [x] Update list and group state tracking

---

### ⚙️ Tier 2: Behavior (implemented or verified)
- [x] Player creation logic (single + dual mode)
- [x] Virtual method analysis (vtable call sites mapped)
- [x] Area move group action (all modes, easing, force application)
- [x] Enter effect state tracking and group linking
- [x] Force multiplier system (per-group)
- [x] Event link/unlink logic
- [x] Song trigger management
- [x] Object flag and state propagation

---

### 🚧 Tier 3: Mapped but incomplete
- [ ] Complete vtable reconstruction (layout known, bodies WIP)
- [ ] All EnterEffect types (some mapped, some stubbed)
- [ ] Effect timeline scheduling
- [ ] Trigger delay / repeat chains
- [ ] Track system runtime behavior
- [ ] Directional force submodes (edge cases)

---

### 🏗️ Tier 4: Not started (but mapped in disassembly)
- [ ] PlayLayer runtime loop and tick processing
- [ ] EditorLayer behavior and UI hooks
- [ ] Level reset and checkpoint rebuild logic
- [ ] Full trigger execution pipeline
- [ ] Multiplayer / practice mode divergences

## 🤝 Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📚 Learning Resources

- [ARM64 Instruction Set Reference](https://developer.arm.com/documentation/ddi0596/)
- [Geometry Dash Modding Documentation](https://github.com/HJfod/GD-Docs)
- [Cocos2d-x Game Engine](https://docs.cocos2d-x.org/)
- [Reverse Engineering for Beginners](https://beginners.re/)

## ⚠️ Disclaimer

This project is for **educational purposes only**. All game assets and code belong to RobTop Games. This is a clean-room reimplementation based on public reverse engineering.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🌟 Acknowledgments

- RobTop Games for creating Geometry Dash
- The GD modding community
- IDA Pro and Ghidra developers
- ARM for the ARM64 architecture documentation

---

**Made with ❤️ by the reverse engineering community**

*"The only limit is your imagination... and the ARM64 instruction set"*
