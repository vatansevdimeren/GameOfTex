#pragma once

#include "Warehouse.hpp"
#include "ThermalModel.hpp"
#include "PowerGrid.hpp"
#include "CoolingManager.hpp"
#include <string>
#include <vector>
#include <memory>

namespace Core {

class EconomyManager;

/**
 * @struct FacilityEconomist
 * @brief Represents a financial trader / economist hired for a specific facility.
 * Performs automated crypto arbitrage, risk/reward market trading, and electricity hedging discounts.
 */
struct FacilityEconomist {
    int level{0}; // 0: Henüz İşe Alınmadı, 1-5: Seviyeler
    std::string name;
    std::string title;
    double winRate{0.55}; // %55 (Lv1) -> %95 (Lv5)
    double hedgeDiscountPercent{0.0}; // %5 (Lv1) -> %35 (Lv5)
    double tradeTimer{0.0};
    double tradeInterval{28.0}; // Her 28 saniyede bir işlem fırsatı
    double lastTradeProfit{0.0}; // Son işlem sonucu (+ veya -)
    double totalProfitLifetime{0.0};
    int successfulTrades{0};
    int failedTrades{0};
    std::string lastTradeLog;

    void ApplyLevelStats(int lvl) {
        level = lvl;
        switch (lvl) {
            case 1:
                title = "Stajyer Finans Analisti";
                winRate = 0.55;
                hedgeDiscountPercent = 0.05;
                tradeInterval = 28.0;
                break;
            case 2:
                title = "Kidemli Piyasa Analisti";
                winRate = 0.65;
                hedgeDiscountPercent = 0.10;
                tradeInterval = 25.0;
                break;
            case 3:
                title = "Kripto Portfoy Yoneticisi";
                winRate = 0.75;
                hedgeDiscountPercent = 0.18;
                tradeInterval = 22.0;
                break;
            case 4:
                title = "Kantitatif Algoritmik Trader";
                winRate = 0.85;
                hedgeDiscountPercent = 0.25;
                tradeInterval = 18.0;
                break;
            case 5:
                title = "Yapay Zeka Destekli Bas Ekonomist";
                winRate = 0.95;
                hedgeDiscountPercent = 0.35;
                tradeInterval = 15.0;
                break;
            default:
                level = 0;
                title = "Ise Alinmadi";
                winRate = 0.55;
                hedgeDiscountPercent = 0.0;
                tradeInterval = 28.0;
                break;
        }
    }
};

/**
 * @struct FacilityLocation
 * @brief Represents a physical real-world data center / warehouse location on the world map.
 */
struct FacilityLocation {
    std::string id;
    std::string nameKey;
    std::string countryKey;
    std::string descKey;
    double climateTempCelsius{22.0};
    double baseGridWatts{15000.0};
    double gridPricePerKwh{0.14};
    size_t maxRigCapacity{5};
    double purchaseCostUSD{0.0};
    bool isPurchased{false};
    float mapNormX{0.5f};
    float mapNormY{0.5f};

    // Dedicated physical simulation units for this facility
    std::unique_ptr<Warehouse> warehouse;
    std::unique_ptr<ThermalModel> thermalModel;
    std::unique_ptr<PowerGrid> powerGrid;
    std::unique_ptr<CoolingManager> coolingManager;

    // Assigned facility economist
    FacilityEconomist economist;
};

/**
 * @class FacilityManager
 * @brief Manages worldwide mining warehouses, purchases, switching, and regional parameters.
 * 
 * SRP: Solely responsible for tracking facilities, unlocking regions, and active facility delegation.
 */
class FacilityManager {
public:
    FacilityManager();

    [[nodiscard]] size_t GetFacilityCount() const;
    [[nodiscard]] size_t GetActiveFacilityIndex() const;
    void SetActiveFacilityIndex(size_t index);

    [[nodiscard]] FacilityLocation* GetActiveFacility();
    [[nodiscard]] const FacilityLocation* GetActiveFacility() const;

    [[nodiscard]] FacilityLocation* GetFacility(size_t index);
    [[nodiscard]] const FacilityLocation* GetFacility(size_t index) const;

    [[nodiscard]] const std::vector<FacilityLocation>& GetAllFacilities() const;

    bool PurchaseFacility(size_t index, EconomyManager& economy);
    bool SwitchFacility(size_t index);

    // Economist Management API
    bool HireOrUpgradeEconomist(size_t facilityIndex, EconomyManager& economy);
    [[nodiscard]] double GetEconomistUpgradeCost(size_t facilityIndex) const;
    [[nodiscard]] std::string GetEconomistNextTitle(size_t facilityIndex) const;
    void UpdateEconomists(double dt, EconomyManager& economy, std::string& outNotification);

private:
    std::vector<FacilityLocation> m_facilities;
    size_t m_activeFacilityIndex{0};
};

} // namespace Core
