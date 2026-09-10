#pragma once

#include "raylib.h"
#include "UIButton.hpp"
#include "../core/FacilityManager.hpp"
#include "../core/EconomyManager.hpp"
#include <vector>

namespace Render {

/**
 * @class WorldMapModal
 * @brief Interactive tactical world map showing global facility locations, climate, and purchase options.
 * 
 * SRP: Solely responsible for rendering the world map UI and handling facility selection/purchases.
 */
class WorldMapModal {
public:
    WorldMapModal();

    void Open();
    void Close();
    [[nodiscard]] bool IsOpen() const;

    void Update(Core::FacilityManager& facilityManager, Core::EconomyManager& economy);
    void Draw(const Core::FacilityManager& facilityManager, const Core::EconomyManager& economy) const;

private:
    bool m_isOpen{false};
    size_t m_selectedFacilityIndex{0};

    UIButton m_btnClose;
    UIButton m_btnAction;
    UIButton m_btnUpgradeEconomist;
    std::vector<UIButton> m_facilityListButtons;

    void DrawWorldMapTacticalGrid(Rectangle mapArea) const;
};

} // namespace Render
