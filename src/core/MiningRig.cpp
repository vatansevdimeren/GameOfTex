#include "MiningRig.hpp"
#include "ThermalModel.hpp"
#include <algorithm>

namespace Core {

MiningRig::MiningRig(const std::string& rigName, size_t maxGpuCapacity, double motherboardBaseWatts)
    : m_name(rigName)
    , m_maxCapacity(maxGpuCapacity)
    , m_motherboardBaseWatts(motherboardBaseWatts)
{
    if (maxGpuCapacity <= 2) m_rigLevel = 1;
    else if (maxGpuCapacity <= 4) m_rigLevel = 2;
    else if (maxGpuCapacity <= 6) m_rigLevel = 3;
    else if (maxGpuCapacity <= 8) m_rigLevel = 4;
    else m_rigLevel = 5;

    m_gpus.reserve(maxGpuCapacity);
}

const std::string& MiningRig::GetName() const {
    return m_name;
}

size_t MiningRig::GetMaxCapacity() const {
    switch (m_rigLevel) {
        case 1: return 2;
        case 2: return 4;
        case 3: return 6;
        case 4: return 8;
        case 5: return 10;
        default: return m_maxCapacity;
    }
}

size_t MiningRig::GetGPUCount() const {
    return m_gpus.size();
}

bool MiningRig::InstallGPU(std::unique_ptr<GPU> gpu) {
    if (!gpu || m_gpus.size() >= m_maxCapacity) {
        return false;
    }
    m_gpus.push_back(std::move(gpu));
    return true;
}

std::unique_ptr<GPU> MiningRig::RemoveGPU(size_t slotIndex) {
    if (slotIndex >= m_gpus.size()) {
        return nullptr;
    }
    auto it = m_gpus.begin() + slotIndex;
    std::unique_ptr<GPU> removed = std::move(*it);
    m_gpus.erase(it);
    return removed;
}

const std::vector<std::unique_ptr<GPU>>& MiningRig::GetGPUs() const {
    return m_gpus;
}

GPU* MiningRig::GetGPU(size_t slotIndex) {
    if (slotIndex >= m_gpus.size()) {
        return nullptr;
    }
    return m_gpus[slotIndex].get();
}

const GPU* MiningRig::GetGPU(size_t slotIndex) const {
    if (slotIndex >= m_gpus.size()) {
        return nullptr;
    }
    return m_gpus[slotIndex].get();
}

bool MiningRig::IsPoweredOn() const {
    return m_isPoweredOn;
}

void MiningRig::SetPoweredOn(bool on) {
    if (!m_isPoweredOn && on) {
        m_startupSurgeTimer = 3.0; // 3 saniye kalkış / demeraj akımı
    }
    m_isPoweredOn = on;
}

void MiningRig::TogglePower() {
    SetPoweredOn(!m_isPoweredOn);
}

void MiningRig::Update(double dt) {
    if (m_startupSurgeTimer > 0.0) {
        m_startupSurgeTimer -= dt;
        if (m_startupSurgeTimer < 0.0) {
            m_startupSurgeTimer = 0.0;
        }
    }
}

bool MiningRig::IsInStartupSurge() const {
    return m_isPoweredOn && (m_startupSurgeTimer > 0.0);
}

double MiningRig::CalculateTotalHashrate() const {
    if (!m_isPoweredOn) {
        return 0.0;
    }
    double total = 0.0;
    for (const auto& gpu : m_gpus) {
        if (gpu) {
            total += gpu->GetEffectiveHashrate();
        }
    }
    return total;
}

double MiningRig::CalculateTotalPowerWatts() const {
    if (!m_isPoweredOn) {
        return 2.0; // Bekleme modu (Standby / LEDs)
    }
    // Anakart, CPU ve sistemin taban tüketimi
    double total = m_motherboardBaseWatts + m_cpuPowerWatts;
    for (const auto& gpu : m_gpus) {
        if (gpu) {
            total += gpu->GetEffectivePowerWatts();
        }
    }
    // Demeraj kalkış güç sıçraması: Fanlar %100 hızla döner ve güç kaynakları +%30 ani akım çeker!
    if (m_startupSurgeTimer > 0.0) {
        total *= 1.30;
    }
    return total;
}

double MiningRig::GetPSUMaxWatts() const {
    static const double capacities[] = { 850.0, 1300.0, 1800.0, 2600.0 };
    if (m_psuTier < 4) return capacities[m_psuTier];
    return 2600.0;
}

const std::string& MiningRig::GetPSUName() const {
    static const std::string names[] = {
        "850W Gold (ATX 3.0)",
        "1300W Platinum Modular",
        "1800W Titanium High-Load",
        "2600W Server Dual PSU"
    };
    if (m_psuTier < 4) return names[m_psuTier];
    return names[3];
}

size_t MiningRig::GetPSUTier() const {
    return m_psuTier;
}

double MiningRig::GetNextPSUCost() const {
    static const double costs[] = { 350.0, 650.0, 1100.0, 0.0 };
    if (m_psuTier < 3) return costs[m_psuTier];
    return 0.0;
}

std::string MiningRig::GetNextPSUName() const {
    if (m_psuTier == 0) return "1300W Platinum";
    if (m_psuTier == 1) return "1800W Titanium";
    if (m_psuTier == 2) return "2600W Server Dual";
    return "MAKSIMUM PSU";
}

bool MiningRig::CanUpgradePSU() const {
    return m_psuTier < 3;
}

bool MiningRig::UpgradePSU() {
    if (m_psuTier < 3) {
        m_psuTier++;
        return true;
    }
    return false;
}

void MiningRig::SetPSUTier(size_t tier) {
    m_psuTier = std::min(tier, size_t{3});
}

bool MiningRig::IsPSUOverloaded() const {
    if (!m_isPoweredOn) return false;
    return CalculateTotalPowerWatts() > GetPSUMaxWatts();
}

double MiningRig::CalculateAverageTemperature(const ThermalModel& thermalModel) const {
    if (m_gpus.empty()) return thermalModel.GetAmbientTemperature();
    double sum = 0.0;
    size_t count = 0;
    for (const auto& gpu : m_gpus) {
        if (gpu) {
            double temp = m_isPoweredOn ? thermalModel.CalculateGPUTemperature(gpu->GetEffectivePowerWatts(), gpu->GetFanSpeedPercent() / 100.0)
                                        : thermalModel.GetAmbientTemperature();
            sum += temp;
            count++;
        }
    }
    return (count > 0) ? (sum / static_cast<double>(count)) : thermalModel.GetAmbientTemperature();
}

int MiningRig::GetRigLevel() const {
    return m_rigLevel;
}

void MiningRig::SetRigLevel(int level) {
    m_rigLevel = std::clamp(level, 1, 5);
    m_maxCapacity = GetMaxCapacity();
}

std::string MiningRig::GetRigLevelName() const {
    switch (m_rigLevel) {
        case 1: return "Ahsap Garaj Kasasi (2 Slot)";
        case 2: return "Aluminyum Acik Kasa (4 Slot)";
        case 3: return "Celik Pro Sasi (6 Slot)";
        case 4: return "4U Sunucu Kabini (8 Slot)";
        case 5: return "Kriyojenik Daldırma Tanki (10 Slot)";
        default: return "Standart Kasa";
    }
}

std::string MiningRig::GetNextRigLevelName() const {
    switch (m_rigLevel) {
        case 1: return "Aluminyum Acik Kasa (4 Slot)";
        case 2: return "Celik Pro Sasi (6 Slot)";
        case 3: return "4U Sunucu Kabini (8 Slot)";
        case 4: return "Kriyojenik Daldırma Tanki (10 Slot)";
        default: return "MAKSIMUM SEVIYE";
    }
}

double MiningRig::GetNextRigUpgradeCost() const {
    switch (m_rigLevel) {
        case 1: return 180.0;
        case 2: return 450.0;
        case 3: return 950.0;
        case 4: return 1800.0;
        default: return 0.0;
    }
}

bool MiningRig::CanUpgradeRigFrame() const {
    return m_rigLevel < 5;
}

bool MiningRig::UpgradeRigFrame() {
    if (m_rigLevel < 5) {
        m_rigLevel++;
        m_maxCapacity = GetMaxCapacity();
        return true;
    }
    return false;
}

void MiningRig::InstallCPU(const std::string& name, double hashrateKH, double powerWatts) {
    m_cpuName = name;
    m_cpuHashrateKH = hashrateKH;
    m_cpuPowerWatts = powerWatts;
}

const std::string& MiningRig::GetCPUName() const {
    return m_cpuName;
}

double MiningRig::GetCPUHashrateKH() const {
    return m_cpuHashrateKH;
}

double MiningRig::GetCPUPowerWatts() const {
    return m_cpuPowerWatts;
}

double MiningRig::CalculateTotalCPUHashrateKH() const {
    if (!m_isPoweredOn) return 0.0;
    return m_cpuHashrateKH;
}

} // namespace Core
