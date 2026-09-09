#include "MarketCatalog.hpp"

namespace Core {

MarketCatalog::MarketCatalog() {
    InitCatalog();
}

void MarketCatalog::InitCatalog() {
    // 1. Ekran Kartı & ASIC Donanım Kataloğu (13 Model - 4 Farklı Tier)
    m_gpuModels = {
        // --- TIER 1: ENTRY / BAŞLANGIÇ (Ev / Garaj) ---
        {
            "rx_580",
            "AMD Radeon RX 580 8GB",
            "TIER_ENTRY",
            28.0,
            95.0,
            180.0,
            28.0 / 95.0,
            Color{80, 200, 120, 255},
            1,
            1
        },
        {
            "gtx_1660s",
            "NVIDIA GTX 1660 Super",
            "TIER_ENTRY",
            31.0,
            75.0,
            240.0,
            31.0 / 75.0,
            Color{60, 220, 160, 255},
            1,
            1
        },
        {
            "rtx_2060s",
            "RTX 2060 Super 8GB",
            "TIER_ENTRY",
            42.0,
            110.0,
            340.0,
            42.0 / 110.0,
            Color{110, 230, 120, 255},
            1,
            1
        },

        // --- TIER 2: MID / PROFESYONEL (Depo / Atölye) ---
        {
            "rx_6800xt",
            "AMD Radeon RX 6800 XT",
            "TIER_MID",
            64.0,
            140.0,
            520.0,
            64.0 / 140.0,
            Color{50, 190, 240, 255},
            2,
            1
        },
        {
            "rtx_3070ti",
            "RTX 3070 Ti 8GB",
            "TIER_MID",
            68.0,
            145.0,
            590.0,
            68.0 / 145.0,
            Color{70, 160, 255, 255},
            2,
            1
        },
        {
            "rtx_3080",
            "RTX 3080 Trinity 10GB",
            "TIER_MID",
            98.0,
            210.0,
            820.0,
            98.0 / 210.0,
            Color{0, 210, 255, 255},
            2,
            2
        },

        // --- TIER 3: HIGH / SUNUCU & VERİ MERKEZİ ---
        {
            "rtx_4070tis",
            "RTX 4070 Ti Super 16GB",
            "TIER_HIGH",
            92.0,
            180.0,
            890.0,
            92.0 / 180.0,
            Color{160, 100, 245, 255},
            3,
            2
        },
        {
            "rtx_4080s",
            "RTX 4080 Super OC",
            "TIER_HIGH",
            115.0,
            220.0,
            1150.0,
            115.0 / 220.0,
            Color{190, 90, 255, 255},
            3,
            2
        },
        {
            "rtx_4090m",
            "RTX 4090 Monster 24GB",
            "TIER_ULTRA",
            180.0,
            360.0,
            2100.0,
            180.0 / 360.0,
            Color{255, 135, 40, 255},
            3,
            3
        },

        // --- TIER 4: INDUSTRIAL & ASIC ÇİPLERİ ---
        {
            "rtx_6000ada",
            "RTX 6000 Ada DataCenter",
            "TIER_ASIC",
            240.0,
            300.0,
            3600.0,
            240.0 / 300.0,
            Color{255, 205, 50, 255},
            4,
            3
        },
        {
            "antminer_s19",
            "Antminer S19 Pro+ Hydro",
            "TIER_ASIC",
            280.0,
            460.0,
            4100.0,
            280.0 / 460.0,
            Color{255, 80, 80, 255},
            4,
            3
        },
        {
            "asic_titan",
            "Cryptonex Titan ASIC 400",
            "TIER_ASIC",
            350.0,
            540.0,
            4800.0,
            350.0 / 540.0,
            Color{255, 45, 95, 255},
            4,
            4
        },
        {
            "quantum_matrix",
            "Quantum Hash Matrix X1",
            "TIER_ASIC",
            520.0,
            720.0,
            7500.0,
            520.0 / 720.0,
            Color{255, 245, 120, 255},
            4,
            4
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

void MarketCatalog::SetPowerUpgradeInstalled(size_t index, bool installed) {
    if (index < m_powerUpgrades.size()) {
        m_powerUpgrades[index].isInstalled = installed;
    }
}

void MarketCatalog::SetFacilityUpgradeInstalled(size_t index, bool installed) {
    if (index < m_facilityUpgrades.size()) {
        m_facilityUpgrades[index].isInstalled = installed;
    }
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
