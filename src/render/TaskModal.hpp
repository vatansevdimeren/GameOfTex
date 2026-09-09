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
    void Draw(const Core::TaskManager& taskManager, const Core::EconomyManager& economy);

private:
    bool m_isOpen{false};
    UIButton m_btnClose;
    UIButton m_btnTabAll;
    UIButton m_btnTabCash;
    UIButton m_btnTabCrypto;
    UIButton m_btnTabFacility;
    UIButton m_btnTabHardware;
    UIButton m_btnTabTrading;
    Core::TaskCategory m_currentCategory{Core::TaskCategory::ALL};
    std::vector<UIButton> m_claimButtons;
    float m_scrollOffset{0.0f};
};

} // namespace Render
