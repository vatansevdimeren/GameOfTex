#pragma once

#include "raylib.h"
#include "UIButton.hpp"
#include "../core/TaskManager.hpp"
#include "../core/EconomyManager.hpp"
#include <vector>

namespace Render {

/**
 * @class TaskModal
 * @brief Modal window displaying quests, milestones, progress bars, and claimable rewards.
 * 
 * SRP: This class is solely responsible for rendering the mission hub and capturing claim clicks.
 */
class TaskModal {
public:
    TaskModal();

    void Open();
    void Close();
    [[nodiscard]] bool IsOpen() const;

    void Update(Core::TaskManager& taskManager, Core::EconomyManager& economy);
    void Draw(const Core::TaskManager& taskManager, const Core::EconomyManager& economy) const;

private:
    bool m_isOpen{false};
    UIButton m_btnClose;
    std::vector<UIButton> m_claimButtons;
    float m_scrollOffset{0.0f};
};

} // namespace Render
