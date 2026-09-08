#pragma once

#include <string>
#include <vector>

namespace Core {

struct CoolingTierInfo {
    std::string name;
    double addedCoolingWatts;
    double cost;
    bool isInstalled;
};

/**
 * @class CoolingManager
 * @brief Manages industrial cooling infrastructure tiers for the warehouse facility.
 * 
 * SRP: This class is solely responsible for tracking cooling tier upgrades,
 * calculating total facility heat extraction capacity, and managing upgrade costs.
 */
class CoolingManager {
public:
    CoolingManager();

    [[nodiscard]] double CalculateTotalCoolingWatts() const;

    [[nodiscard]] const std::vector<CoolingTierInfo>& GetTiers() const;

    /**
     * @brief Attempts to upgrade to the specified tier index.
     * @param tierIndex Index of the cooling system (1: Wall Fan, 2: HVAC, 3: Immersion).
     * @return Cost of upgrade if successful, 0.0 if already installed or invalid.
     */
    double UpgradeTier(size_t tierIndex);

    [[nodiscard]] bool IsImmersionCoolingActive() const;

private:
    std::vector<CoolingTierInfo> m_tiers;
};

} // namespace Core
