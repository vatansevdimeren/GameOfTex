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
    [[nodiscard]] const GPU* GetGPU(size_t slotIndex) const;

    // Rig Power Control
    [[nodiscard]] bool IsPoweredOn() const;
    void SetPoweredOn(bool on);
    void TogglePower();
    void Update(double dt);
    [[nodiscard]] bool IsInStartupSurge() const;

    // Aggregate performance metrics
    [[nodiscard]] double CalculateTotalHashrate() const;
    [[nodiscard]] double CalculateTotalPowerWatts() const;

    // Power Supply (PSU) System
    [[nodiscard]] double GetPSUMaxWatts() const;
    [[nodiscard]] const std::string& GetPSUName() const;
    [[nodiscard]] size_t GetPSUTier() const;
    [[nodiscard]] double GetNextPSUCost() const;
    [[nodiscard]] std::string GetNextPSUName() const;
    [[nodiscard]] bool CanUpgradePSU() const;
    bool UpgradePSU();
    void SetPSUTier(size_t tier);
    [[nodiscard]] bool IsPSUOverloaded() const;

    // Temperature metrics
    [[nodiscard]] double CalculateAverageTemperature(const class ThermalModel& thermalModel) const;

    // Rig Frame Level / Tier System (1: Ahşap 2-slot, 2: Alüminyum 4-slot, 3: Çelik 6-slot, 4: Server Rack 8-slot, 5: Kriyojenik 10-slot)
    [[nodiscard]] int GetRigLevel() const;
    void SetRigLevel(int level);
    [[nodiscard]] std::string GetRigLevelName() const;
    [[nodiscard]] std::string GetNextRigLevelName() const;
    [[nodiscard]] double GetNextRigUpgradeCost() const;
    [[nodiscard]] bool CanUpgradeRigFrame() const;
    bool UpgradeRigFrame();

    // CPU Madenciliği Sistemi
    void InstallCPU(const std::string& name, double hashrateKH, double powerWatts);
    [[nodiscard]] const std::string& GetCPUName() const;
    [[nodiscard]] double GetCPUHashrateKH() const;
    [[nodiscard]] double GetCPUPowerWatts() const;
    [[nodiscard]] double CalculateTotalCPUHashrateKH() const;

private:
    std::string m_name;
    size_t m_maxCapacity{6};
    double m_motherboardBaseWatts;
    bool m_isPoweredOn{true};
    double m_startupSurgeTimer{0.0};
    size_t m_psuTier{0}; // 0: 850W, 1: 1300W, 2: 1800W, 3: 2600W
    int m_rigLevel{1};   // 1 to 5
    std::string m_cpuName{"AMD Ryzen 5 3600"};
    double m_cpuHashrateKH{7.2};
    double m_cpuPowerWatts{65.0};
    std::vector<std::unique_ptr<GPU>> m_gpus;
};

} // namespace Core
