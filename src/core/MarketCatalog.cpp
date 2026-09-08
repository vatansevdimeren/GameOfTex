#include "MarketCatalog.hpp"

namespace Core {

MarketCatalog::MarketCatalog() {
    InitCatalog();
}

void MarketCatalog::InitCatalog() {
    // 1. Ekran Kartı Modelleri
    m_gpuModels = {
        {
            "gtx_1660s",
            "GTX 1660 Super",
            "TIER_ENTRY",
            30.0,
            75.0,
            250.0,
            30.0 / 75.0,
            Color{90, 200, 130, 255}
        },
        {
            "rtx_3070ti",
            "RTX 3070 Ti",
            "TIER_MID",
            65.0,
            145.0,
            550.0,
            65.0 / 145.0,
            Color{70, 170, 240, 255}
        },
        {
            "rtx_4080s",
            "RTX 4080 Super",
            "TIER_HIGH",
            110.0,
            220.0,
            1100.0,
            110.0 / 220.0,
            Color{170, 110, 240, 255}
        },
        {
            "rtx_4090m",
            "RTX 4090 Monster",
            "TIER_ULTRA",
            175.0,
            360.0,
            2000.0,
            175.0 / 360.0,
            Color{255, 145, 40, 255}
        },
        {
            "asic_titan",
            "Cryptonex Titan ASIC",
            "TIER_ASIC",
            320.0,
            550.0,
            4500.0,
            320.0 / 550.0,
            Color{255, 55, 90, 255}
        }
    };

    // 2. Trafo & Elektrik Şebekesi Yükseltmeleri
    m_powerUpgrades = {
        {"psu_base", "POWER_TIER_1", 3600.0, 0.0, true},
        {"psu_industrial", "POWER_TIER_2", 7500.0, 1500.0, false},
        {"psu_substation", "POWER_TIER_3", 15000.0, 3500.0, false},
        {"psu_grid_line", "POWER_TIER_4", 30000.0, 8000.0, false}
    };

    // 3. Tesis & Depo Geliştirmeleri
    m_facilityUpgrades = {
        {"solar_1", "FACILITY_SOLAR_1", "FACILITY_SOLAR_1_DESC", 1800.0, false, 1000.0, false},
        {"solar_2", "FACILITY_SOLAR_2", "FACILITY_SOLAR_2_DESC", 3600.0, false, 2500.0, false},
        {"fire_foam", "FACILITY_FIRE_EXT", "FACILITY_FIRE_EXT_DESC", 2800.0, false, 0.0, true}
    };
}

const std::vector<GPUModelItem>& MarketCatalog::GetGPUModels() const {
    return m_gpuModels;
}

const GPUModelItem* MarketCatalog::GetGPUModel(size_t index) const {
    if (index < m_gpuModels.size()) {
        return &m_gpuModels[index];
    }
    return nullptr;
}

std::vector<PowerUpgradeItem>& MarketCatalog::GetPowerUpgrades() {
    return m_powerUpgrades;
}

const std::vector<PowerUpgradeItem>& MarketCatalog::GetPowerUpgrades() const {
    return m_powerUpgrades;
}

std::vector<FacilityUpgradeItem>& MarketCatalog::GetFacilityUpgrades() {
    return m_facilityUpgrades;
}

const std::vector<FacilityUpgradeItem>& MarketCatalog::GetFacilityUpgrades() const {
    return m_facilityUpgrades;
}

bool MarketCatalog::PurchasePowerUpgrade(size_t index) {
    if (index < m_powerUpgrades.size() && !m_powerUpgrades[index].isInstalled) {
        m_powerUpgrades[index].isInstalled = true;
        return true;
    }
    return false;
}

bool MarketCatalog::PurchaseFacilityUpgrade(size_t index) {
    if (index < m_facilityUpgrades.size() && !m_facilityUpgrades[index].isInstalled) {
        m_facilityUpgrades[index].isInstalled = true;
        return true;
    }
    return false;
}

bool MarketCatalog::HasAutoFireSuppression() const {
    for (const auto& item : m_facilityUpgrades) {
        if (item.isFireSuppression && item.isInstalled) {
            return true;
        }
    }
    return false;
}

double MarketCatalog::GetTotalGreenWatts() const {
    double total = 0.0;
    for (const auto& item : m_facilityUpgrades) {
        if (item.isInstalled) {
            total += item.addedGreenWatts;
        }
    }
    return total;
}

} // namespace Core
