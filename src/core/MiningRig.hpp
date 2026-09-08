#pragma once

#include "GPU.hpp"
#include <vector>
#include <memory>
#include <string>

namespace Core {

/**
 * @class MiningRig
 * @brief Represents a physical mining rig containing multiple GPU slots,
 * motherboard, and system overhead.
 * 
 * SRP: This class is solely responsible for managing installed GPUs within
 * a single frame/chassis and aggregating total rig power and hashrate.
 */
class MiningRig {
public:
    /**
     * @brief Constructs a mining rig with a maximum GPU capacity.
     * @param rigName Friendly name for this rig.
     * @param maxGpuCapacity Number of PCIe slots available on the motherboard (e.g. 6).
     * @param motherboardBaseWatts Base power consumed by motherboard, CPU, RAM and SSD (default: 50W).
     */
    MiningRig(const std::string& rigName, size_t maxGpuCapacity = 6, double motherboardBaseWatts = 50.0);

    // Rig management
    [[nodiscard]] const std::string& GetName() const;
    [[nodiscard]] size_t GetMaxCapacity() const;
    [[nodiscard]] size_t GetGPUCount() const;

    /**
     * @brief Installs a GPU into the next available slot.
     * @param gpu Unique pointer to the GPU object.
     * @return True if installed successfully, false if the rig is full.
     */
    bool InstallGPU(std::unique_ptr<GPU> gpu);

    /**
     * @brief Removes and returns the GPU at the specified slot index.
     * @param slotIndex Index of the GPU slot.
     * @return Unique pointer to the removed GPU, or nullptr if invalid.
     */
    std::unique_ptr<GPU> RemoveGPU(size_t slotIndex);

    /**
     * @brief Direct read-only access to installed GPUs.
     */
    [[nodiscard]] const std::vector<std::unique_ptr<GPU>>& GetGPUs() const;

    /**
     * @brief Mutable access to a specific GPU for overclocking.
     */
    [[nodiscard]] GPU* GetGPU(size_t slotIndex);

    // Rig Power Control
    [[nodiscard]] bool IsPoweredOn() const;
    void SetPoweredOn(bool on);
    void TogglePower();

    // Aggregate performance metrics
    [[nodiscard]] double CalculateTotalHashrate() const;
    [[nodiscard]] double CalculateTotalPowerWatts() const;

private:
    std::string m_name;
    size_t m_maxCapacity;
    double m_motherboardBaseWatts;
    bool m_isPoweredOn{true};
    std::vector<std::unique_ptr<GPU>> m_gpus;
};

} // namespace Core
