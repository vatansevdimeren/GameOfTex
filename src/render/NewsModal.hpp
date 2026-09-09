#pragma once

#include "raylib.h"
#include "UIButton.hpp"
#include "../core/NewsManager.hpp"
#include "../core/TaskManager.hpp"

namespace Render {

/**
 * @class NewsModal
 * @brief Modal window displaying real-time fake analyst news stream and market rumors.
 * 
 * SRP: Solely responsible for rendering the news & analyst terminal UI and handling scroll / read events.
 */
class NewsModal {
public:
    NewsModal();

    void Open();
    void Close();
    [[nodiscard]] bool IsOpen() const;

    void Update(Core::NewsManager& newsManager, Core::TaskManager& taskManager);
    void Draw(const Core::NewsManager& newsManager) const;

private:
    bool m_isOpen{false};
    UIButton m_btnClose;
    UIButton m_btnMarkAllRead;
    float m_scrollOffset{0.0f};
};

} // namespace Render
