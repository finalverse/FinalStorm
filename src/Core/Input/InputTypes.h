// src/Core/Input/InputTypes.h
// Input type definitions
// Common types for input handling

#pragma once
#include "Core/Math/MathTypes.h"

namespace FinalStorm {

enum class InputEventType {
    None,
    MouseDown,
    MouseUp,
    MouseMove,
    MouseScroll,
    KeyDown,
    KeyUp,
    TouchBegin,
    TouchMove,
    TouchEnd,
    Gesture
};

enum class MouseButton {
    None,
    Left,
    Right,
    Middle
};

enum class GestureType {
    None,
    Tap,
    DoubleTap,
    Swipe,
    Pinch,
    Rotate
};

struct InputEvent {
    InputEventType type = InputEventType::None;
    
    // Mouse/Touch
    float2 position;
    float2 delta;
    MouseButton mouseButton = MouseButton::None;
    
    // Keyboard
    uint16_t keyCode = 0;
    bool shift = false;
    bool control = false;
    bool alt = false;
    bool command = false;
    
    // Gesture
    GestureType gesture = GestureType::None;
    float gestureValue = 0.0f;
};

} // namespace FinalStorm