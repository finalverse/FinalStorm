// src/Core/Input/InteractionManager.cpp
// Input and interaction handling implementation
// Manages user input and object selection

#include "Core/Input/InteractionManager.h"
#include "Core/Math/Math.h"

namespace FinalStorm {

InteractionManager::InteractionManager()
    : mousePosition(0, 0)
    , lastMousePosition(0, 0)
    , isDragging(false) {
}

InteractionManager::~InteractionManager() = default;

void InteractionManager::update(float deltaTime) {
    // Update interaction state
    if (hoveredObject) {
        // Update hover effects
    }
}

void InteractionManager::handleEvent(const InputEvent& event) {
    switch (event.type) {
        case InputEventType::MouseDown:
            handleMouseDown(event);
            break;
        case InputEventType::MouseUp:
            handleMouseUp(event);
            break;
        case InputEventType::MouseMove:
            handleMouseMove(event);
            break;
        case InputEventType::KeyDown:
            handleKeyDown(event);
            break;
        case InputEventType::KeyUp:
            handleKeyUp(event);
            break;
        default:
            break;
    }
}

void InteractionManager::handleMouseDown(const InputEvent& event) {
    mousePosition = event.position;
    lastMousePosition = mousePosition;
    isDragging = true;
    
    // TODO: Implement ray casting for object selection
}

void InteractionManager::handleMouseUp(const InputEvent& event) {
    isDragging = false;
    
    if (selectedObject) {
        // Handle object click
    }
}

void InteractionManager::handleMouseMove(const InputEvent& event) {
    lastMousePosition = mousePosition;
    mousePosition = event.position;
    
    if (isDragging) {
        // Handle camera rotation or object manipulation
        float2 delta = mousePosition - lastMousePosition;
        // TODO: Apply delta to camera or selected object
    }
}

void InteractionManager::handleKeyDown(const InputEvent& event) {
    // Handle keyboard shortcuts
    switch (event.keyCode) {
        case 53: // ESC key
            // Deselect object
            selectedObject.reset();
            break;
        default:
            break;
    }
}

void InteractionManager::handleKeyUp(const InputEvent& event) {
    // Handle key release
}

} // namespace FinalStorm