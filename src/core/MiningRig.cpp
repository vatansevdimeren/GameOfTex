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

double MiningRig::CalculateTotalHashrate() const {
    double total = 0.0;
    for (const auto& gpu : m_gpus) {
        if (gpu) {
            total += gpu->GetEffectiveHashrate();
        }
    }
    return total;
}

double MiningRig::CalculateTotalPowerWatts() const {
    // Anakart ve sistemin taban tüketimi
    double total = m_motherboardBaseWatts;
    for (const auto& gpu : m_gpus) {
        if (gpu) {
            total += gpu->GetEffectivePowerWatts();
        }
    }
    return total;
}

} // namespace Core
