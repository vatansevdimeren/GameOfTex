#pragma once

#include <string>
#include <vector>
#include <raylib.h>

namespace Core {

/**
 * @struct GPUModelItem
 * @brief Represents a graphics card catalog product with its performance metrics and pricing.
 */
struct GPUModelItem {
    std::string id;
    std::string name;
    std::string tierKey;
    double hashrate;          // Base MH/s
    double powerWatts;        // Base Watts
    double priceUSD;          // Purchase cost in USD
    double efficiency;        // hashrate / powerWatts
    Color accentColor;        // Visual tier highlight color
    int tier{1};              // 1: Entry, 2: Mid, 3: High, 4: ASIC/Industrial
    int minFacilityTier{1};   // Minimum required facility tier to install
};

/**
 * @struct CPUModelItem
 * @brief Represents a processor product for CPU mining (RandomX / GhostRider).
 */
struct CPUModelItem {
    std::string id;
    std::string name;
    std::string cores;        // e.g. "6C / 12T"
    double hashrateKH;        // Base KH/s
    double powerWatts;        // Base Watts
    double priceUSD;          // Purchase cost in USD
    Color accentColor;        // Visual highlight color
    int tier{1};              // 1: Entry, 2: Mid, 3: High, 4: Server/HEDT
};

/**
 * @struct PowerUpgradeItem
 * @brief Represents an electrical substation or breaker panel upgrade.
 */
struct PowerUpgradeItem {
    std::string id;
    std::string nameKey;
    double capacityWatts;     // New maximum breaker wattage
    double priceUSD;
    bool isInstalled;
};

/**
 * @struct FacilityUpgradeItem
 * @brief Represents infrastructure expansions like solar panels or fire suppression.
 */
struct FacilityUpgradeItem {
    std::string id;
    std::string nameKey;
    std::string descKey;
    double priceUSD;
    bool isInstalled;
    double addedGreenWatts;
    bool isFireSuppression;
};

/**
 * @class MarketCatalog
 * @brief Manages the repository of purchasable hardware, power grids, and facility systems.
 * 
 * SRP: This class is solely responsible for maintaining catalog specifications,
 * tracking purchased permanent upgrades, and providing catalog items to the UI and gameplay.
 */
class MarketCatalog {
public:
    MarketCatalog();

    [[nodiscard]] const std::vector<GPUModelItem>& GetGPUModels() const;
    [[nodiscard]] const GPUModelItem* GetGPUModel(size_t index) const;

    [[nodiscard]] const std::vector<CPUModelItem>& GetCPUModels() const;
    [[nodiscard]] const CPUModelItem* GetCPUModel(size_t index) const;

    [[nodiscard]] std::vector<PowerUpgradeItem>& GetPowerUpgrades();
    [[nodiscard]] const std::vector<PowerUpgradeItem>& GetPowerUpgrades() const;

    [[nodiscard]] std::vector<FacilityUpgradeItem>& GetFacilityUpgrades();
    [[nodiscard]] const std::vector<FacilityUpgradeItem>& GetFacilityUpgrades() const;

    bool PurchasePowerUpgrade(size_t index);
    bool PurchaseFacilityUpgrade(size_t index);
    void SetPowerUpgradeInstalled(size_t index, bool installed);
    void SetFacilityUpgradeInstalled(size_t index, bool installed);

    [[nodiscard]] bool HasAutoFireSuppression() const;
    [[nodiscard]] double GetTotalGreenWatts() const;

private:
    void InitCatalog();

    std::vector<GPUModelItem> m_gpuModels;
    std::vector<CPUModelItem> m_cpuModels;
    std::vector<PowerUpgradeItem> m_powerUpgrades;
    std::vector<FacilityUpgradeItem> m_facilityUpgrades;
};

} // namespace Core
