// src/UI/HolographicDisplay.h
// Holographic display UI element
// Provides futuristic data visualization

#pragma once
#include "UI/UI3DPanel.h"

namespace FinalStorm {

class HolographicDisplay : public UI3DPanel {
public:
    HolographicDisplay(float width = 2.0f, float height = 1.5f);
    ~HolographicDisplay() override;
    
    void setTitle(const std::string& title) { this->title = title; }
    void setData(const std::vector<float>& data) { this->data = data; }
    
protected:
    void onUpdate(float deltaTime) override;
    void onRender(RenderContext& context) override;
    
private:
    std::string title;
    std::vector<float> data;
    float animationTime;
};

} // namespace FinalStorm