#include "CoolingManager.hpp"

namespace Core {

CoolingManager::CoolingManager() {
    m_tiers.push_back(CoolingTierInfo{"Temel Oda Fanı", 300.0, 0.0, true});
    m_tiers.push_back(CoolingTierInfo{"Sanayi Egzoz Fanları", 800.0, 600.0, false});
    m_tiers.push_back(CoolingTierInfo{"HVAC Soguk Koridor Chiller", 2500.0, 1800.0, false});
    m_tiers.push_back(CoolingTierInfo{"Daldirma Sivi Sogutma Tanki", 6000.0, 6000.0, false});
}

double CoolingManager::CalculateTotalCoolingWatts() const {
    double totalWatts = 0.0;
    for (const auto& tier : m_tiers) {
        if (tier.isInstalled) {
            totalWatts += tier.addedCoolingWatts;
        }
    }
    return totalWatts;
}

const std::vector<CoolingTierInfo>& CoolingManager::GetTiers() const {
    return m_tiers;
}

double CoolingManager::UpgradeTier(size_t tierIndex) {
    if (tierIndex < m_tiers.size() && !m_tiers[tierIndex].isInstalled) {
        m_tiers[tierIndex].isInstalled = true;
        return m_tiers[tierIndex].cost;
    }
    return 0.0;
}

void CoolingManager::SetTierInstalled(size_t tierIndex, bool installed) {
    if (tierIndex < m_tiers.size()) {
        m_tiers[tierIndex].isInstalled = installed;
    }
}

bool CoolingManager::IsImmersionCoolingActive() const {
    // 3. indeks Daldırma Sıvı Soğutmadır
    return (m_tiers.size() > 3 && m_tiers[3].isInstalled);
}

} // namespace Core
