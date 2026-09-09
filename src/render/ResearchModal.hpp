#pragma once

#include "raylib.h"
#include "UIButton.hpp"
#include "../core/ResearchManager.hpp"
#include "../core/EconomyManager.hpp"
#include "../core/Warehouse.hpp"
#include <vector>

namespace Render {

/**
 * @class ResearchModal
 * @brief Modal dialog for upgrading Research & Development technologies and executing Venture IPO (Prestige).
 */
class ResearchModal {
public:
    ResearchModal();

    void Open();
    void Close();
    [[nodiscard]] bool IsOpen() const;

    void Update(Core::ResearchManager& research, Core::EconomyManager& economy, Core::Warehouse& warehouse);
    void Draw(const Core::ResearchManager& research, const Core::EconomyManager& economy) const;

private:
    bool m_isOpen{false};
    UIButton m_btnClose;
    UIButton m_btnTabTech;
    UIButton m_btnTabIPO;
    UIButton m_btnExecuteIPO;
    std::vector<UIButton> m_upgradeButtons;

    int m_activeTab{0}; // 0 = Tech Tree, 1 = Venture IPO
    float m_scrollOffset{0.0f};
};

} // namespace Render
