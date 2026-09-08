#include "MiningRig.hpp"

namespace Core {

MiningRig::MiningRig(const std::string& rigName, size_t maxGpuCapacity, double motherboardBaseWatts)
    : m_name(rigName)
    , m_maxCapacity(maxGpuCapacity)
    , m_motherboardBaseWatts(motherboardBaseWatts)
{
    m_gpus.reserve(maxGpuCapacity);
}

const std::string& MiningRig::GetName() const {
    return m_name;
}

size_t MiningRig::GetMaxCapacity() const {
    return m_maxCapacity;
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
    // Anakart ve sistemin taban tüketimi
    double total = m_motherboardBaseWatts;
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

} // namespace Core
