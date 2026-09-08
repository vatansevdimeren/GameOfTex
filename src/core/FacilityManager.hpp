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

private:
    std::vector<FacilityLocation> m_facilities;
    size_t m_activeFacilityIndex{0};
};

} // namespace Core
