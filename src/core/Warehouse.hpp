#pragma once

#include "MiningRig.hpp"
#include <vector>
#include <memory>
#include <string>

namespace Core {

/**
 * @class Warehouse
 * @brief Manages multiple mining rigs inside a single physical warehouse facility.
 * 
 * SRP: This class is solely responsible for managing the collection of rigs,
 * handling rig expansions/purchases, and providing aggregated metrics.
 */
class Warehouse {
public:
    explicit Warehouse(std::string facilityName = "Ana Madencilik Deposu");

    [[nodiscard]] const std::string& GetFacilityName() const;
    void SetFacilityName(std::string name);

    [[nodiscard]] size_t GetRigCount() const;
    [[nodiscard]] size_t GetActiveRigIndex() const;
    void SetActiveRigIndex(size_t index);

    void NextRig();
    void PreviousRig();

    /**
     * @brief Gets pointer to the currently inspected rig in the UI viewport.
     */
    [[nodiscard]] MiningRig* GetActiveRig();
    [[nodiscard]] const MiningRig* GetActiveRig() const;

    /**
     * @brief Gets pointer to a specific rig by index.
     */
    [[nodiscard]] MiningRig* GetRig(size_t index);
    [[nodiscard]] const MiningRig* GetRig(size_t index) const;

    /**
     * @brief Purchases and builds a new mining rig inside the warehouse.
     * @param rigName Name for the new rig.
     * @param gpuCapacity Slot capacity (default 6).
     * @return True if added successfully.
     */
    bool AddNewRig(const std::string& rigName, size_t gpuCapacity = 6);
    void AddRig(std::unique_ptr<MiningRig> rig);
    void ClearRigs();

    /**
     * @brief Decommissions and sells an existing rig, removing it from warehouse.
     * @param index Rig index to remove.
     * @return True if removed successfully, false if only 1 rig remains or index invalid.
     */
    bool RemoveRig(size_t index);

    // Facility-wide aggregated calculations
    [[nodiscard]] double CalculateTotalHashrate() const;
    [[nodiscard]] double CalculateTotalPowerWatts() const;

    [[nodiscard]] const std::vector<std::unique_ptr<MiningRig>>& GetAllRigs() const;

    [[nodiscard]] size_t GetMaxRigCapacity() const;
    void SetMaxRigCapacity(size_t cap);

private:
    std::string m_facilityName;
    std::vector<std::unique_ptr<MiningRig>> m_rigs;
    size_t m_activeRigIndex;
    size_t m_maxRigCapacity{5};
};

} // namespace Core
