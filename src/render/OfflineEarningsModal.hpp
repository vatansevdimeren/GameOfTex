#pragma once

#include "raylib.h"
#include "UIButton.hpp"
#include "../core/SaveManager.hpp"
#include "../core/EconomyManager.hpp"
#include <string>

namespace Render {

/**
 * @class OfflineEarningsModal
 * @brief Modal dialog presenting the offline idle mining harvest report when returning to the game.
 */
class OfflineEarningsModal {
public:
    OfflineEarningsModal();

    void Show(const Core::OfflineMiningReport& report);
    void Close();
    [[nodiscard]] bool IsOpen() const;

    void Update();
    void Draw(const Core::EconomyManager& economy) const;

private:
    bool m_isOpen{false};
    Core::OfflineMiningReport m_report;
    UIButton m_btnCollect;
};

} // namespace Render
