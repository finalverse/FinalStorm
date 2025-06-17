#pragma once

#include "../SceneGraph/SceneNode.h"
#include <functional>

namespace FinalStorm {

class InteractiveOrb : public SceneNode {
public:
    using ActivateCallback = std::function<void()>;

    InteractiveOrb();

    void setActivateCallback(ActivateCallback cb) { m_onActivate = cb; }
    void activate();

protected:
    void onRender(Renderer* renderer) override;

private:
    ActivateCallback m_onActivate;
};

} // namespace FinalStorm
