#pragma once

#include <raylib.h>
#include <vector>
#include <memory>
#include "UIButton.hpp"
#include "../core/EconomyManager.hpp"
#include "../core/Warehouse.hpp"
#include "../core/CoolingManager.hpp"
#include "../core/MarketCatalog.hpp"
#include "../core/PowerGrid.hpp"

namespace Render {

enum class MarketCategory {
    GPUS,
    POWER,
    COOLING,
    FACILITIES
};

struct MarketPurchaseAction {
    enum class ActionType {
        NONE,
        BUY_GPU,
        BUY_POWER,
        BUY_COOLING,
        BUY_FACILITY
    };
    ActionType type = ActionType::NONE;
    size_t itemIndex = 0;
};

/**
 * @class MarketModal
 * @brief Modal window for the hardware and facility marketplace.
 * 
 * SRP: This class is solely responsible for rendering the market tabs,
 * drawing visual performance bars for hardware specifications, and detecting purchase clicks.
 */
class MarketModal {
public:
    MarketModal();

    void Open();
    void Close();
    [[nodiscard]] bool IsOpen() const;

    [[nodiscard]] MarketPurchaseAction Update(
        Core::EconomyManager& economy,
        const Core::Warehouse& warehouse,
        const Core::CoolingManager& coolingManager,
        const Core::MarketCatalog& catalog,
        const Core::PowerGrid& powerGrid
    );

    void Draw(
        const Core::EconomyManager& economy,
        const Core::Warehouse& warehouse,
        const Core::CoolingManager& coolingManager,
        const Core::MarketCatalog& catalog,
        const Core::PowerGrid& powerGrid
    );

private:
    void DrawHeader(float modalX, float modalY, float modalW);
    void DrawTabs(float modalX, float modalY, float modalW);
    
    void DrawGPUsCategory(
        float startX, float startY, float width, float height,
        const Core::EconomyManager& economy,
        const Core::Warehouse& warehouse,
        const Core::MarketCatalog& catalog
    );

    void DrawPowerCategory(
        float startX, float startY, float width, float height,
        const Core::EconomyManager& economy,
        const Core::MarketCatalog& catalog,
        const Core::PowerGrid& powerGrid
    );

    void DrawCoolingCategory(
        float startX, float startY, float width, float height,
        const Core::EconomyManager& economy,
        const Core::CoolingManager& coolingManager
    );

    void DrawFacilitiesCategory(
        float startX, float startY, float width, float height,
        const Core::EconomyManager& economy,
        const Core::MarketCatalog& catalog
    );

    void DrawStatBar(
        float x, float y, float w, float h,
        float ratio, Color barColor,
        const char* labelLeft, const char* labelRight
    );

    bool m_isOpen;
    MarketCategory m_currentCategory;

    UIButton m_btnTabGPUs;
    UIButton m_btnTabPower;
    UIButton m_btnTabCooling;
    UIButton m_btnTabFacilities;
    UIButton m_btnClose;

    std::vector<UIButton> m_gpuBuyButtons;
    std::vector<UIButton> m_powerBuyButtons;
    std::vector<UIButton> m_coolingBuyButtons;
    std::vector<UIButton> m_facilityBuyButtons;
};

} // namespace Render
