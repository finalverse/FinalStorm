#pragma once

#include "../Scene/SceneNode.h"
#include "UI3DPanel.h"
#include <memory>

namespace FinalStorm {

class HolographicDisplay : public SceneNode {
public:
    HolographicDisplay(uint32_t width, uint32_t height);

    Panel& getPanel() { return *m_panel; }

protected:
    void onRender(Renderer* renderer) override;

private:
    std::unique_ptr<Panel> m_panel;
};

} // namespace FinalStorm
