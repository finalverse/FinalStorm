// Core/Input/InputManager.h
#pragma once

#include "Core/Math/MathTypes.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <set>

namespace FinalStorm {

// This handles low-level input events and feeds them to InteractionManager

enum class KeyCode {
    Unknown = -1,
    
    // Letters
    A = 0, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    
    // Numbers
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    
    // Function keys
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    
    // Control keys
    Escape, Tab, CapsLock, Shift, Control, Alt, Command,
    Space, Enter, Backspace, Delete,
    
    // Arrow keys
    Left, Right, Up, Down,
    
    Count
};

class InputManager {
public:
    using KeyCallback = std::function<void(KeyCode, bool)>;
    using MouseCallback = std::function<void(const vec2&, const vec2&)>;
    using ScrollCallback = std::function<void(const vec2&)>;
    
    static InputManager& getInstance() {
        static InputManager instance;
        return instance;
    }
    
    // Low-level event handlers - called by platform layer
    void handleKeyEvent(KeyCode key, bool pressed);
    void handleMouseMove(float x, float y);
    void handleMouseButton(int button, bool pressed);
    void handleMouseScroll(float deltaX, float deltaY);
    
    // Query current state
    bool isKeyPressed(KeyCode key) const;
    bool isMouseButtonPressed(int button) const;
    vec2 getMousePosition() const { return m_mousePosition; }
    
    // Callbacks for high-level handlers (like InteractionManager)
    void setKeyCallback(KeyCallback callback) { m_keyCallback = callback; }
    void setMouseCallback(MouseCallback callback) { m_mouseCallback = callback; }
    void setScrollCallback(ScrollCallback callback) { m_scrollCallback = callback; }
    
private:
    InputManager() = default;
    
    // Current state
    std::set<KeyCode> m_pressedKeys;
    std::set<int> m_pressedMouseButtons;
    vec2 m_mousePosition;
    vec2 m_lastMousePosition;
    
    // Callbacks
    KeyCallback m_keyCallback;
    MouseCallback m_mouseCallback;
    ScrollCallback m_scrollCallback;
};

} // namespace FinalStorm