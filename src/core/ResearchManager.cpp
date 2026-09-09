#include "ResearchManager.hpp"
#include "EconomyManager.hpp"
#include "Warehouse.hpp"
#include <cmath>
#include <algorithm>

namespace Core {

ResearchManager::ResearchManager() {
    m_techs = {
        TechItem{
            TechType::CUSTOM_BIOS,
            "tech_bios",
            "Ozel Madenci BIOS & Silikon Kalitesi",
            "Custom Miner BIOS & Silicon Lottery",
            "Kartlarin silikon zamanlamalarini optimize ederek her seviyede +%10 kalici hashrate saglar.",
            "Optimizes silicon timings to grant +10% permanent hashrate per level.",
            0, 5, 1200.0, 2.3
        },
        TechItem{
            TechType::AI_POOL_ROUTER,
            "tech_ai_pool",
            "Yapay Zeka Havuz Yonlendiricisi",
            "AI Pool Hash Router",
            "Yapay zeka ile en karli bloklari tespit eder, her seviyede +%12 madencilik geliri katar.",
            "Routes hashing to optimal blocks, boosting mining revenue by +12% per level.",
            0, 5, 1800.0, 2.4
        },
        TechItem{
            TechType::IMMERSION_NANO,
            "tech_immersion",
            "Nano Termal Iletkenlik",
            "Nano Thermal Conductivity",
            "Grafen ve nano bilesenlerle kartlarin isi artisini her seviyede %8 azaltir.",
            "Uses graphene and nano-composites to reduce heat generation by 8% per level.",
            0, 5, 1500.0, 2.2
        },
        TechItem{
            TechType::GREEN_POWER,
            "tech_green_power",
            "Yenilenebilir Yesil Enerji Santrali",
            "Renewable Green Energy Farm",
            "Tesis catisina gunes ve ruzgar panelleri kurarak elektrik faturasini her seviyede %10 dusurur.",
            "Installs solar and wind turbines to reduce electricity bills by 10% per level.",
            0, 5, 2200.0, 2.3
        },
        TechItem{
            TechType::SMART_AUTOMATION,
            "tech_automation",
            "Otonom Tesis Muhendisi (AI)",
            "Autonomous Facility Engineer",
            "Salter attiginda 2 saniyede otomatik acar ve kartlar 85C uzerine ciktiginda fanlari %100'e kilitler.",
            "Automatically resets tripped breaker within 2s and forces 100% fans when cards exceed 85C.",
            0, 1, 6500.0, 1.0
        }
    };
}

const std::vector<TechItem>& ResearchManager::GetTechnologies() const {
    return m_techs;
}

const TechItem* ResearchManager::GetTechnology(TechType type) const {
    for (const auto& t : m_techs) {
        if (t.type == type) return &t;
    }
    return nullptr;
}

double ResearchManager::GetTechCost(TechType type) const {
    const auto* t = GetTechnology(type);
    if (!t) return 0.0;
    if (t->currentLevel >= t->maxLevel) return 0.0;
    return t->baseCost * std::pow(t->costMultiplier, t->currentLevel);
}

bool ResearchManager::UpgradeTech(TechType type, EconomyManager& economy) {
    for (auto& t : m_techs) {
        if (t.type == type) {
            if (t.currentLevel >= t.maxLevel) return false;
            double cost = t.baseCost * std::pow(t.costMultiplier, t.currentLevel);
            if (economy.GetFiatBalance() >= cost && economy.DeductFiat(cost)) {
                t.currentLevel++;
                return true;
            }
            return false;
        }
    }
    return false;
}

double ResearchManager::GetHashrateMultiplier() const {
    int biosLvl = 0;
    if (const auto* t = GetTechnology(TechType::CUSTOM_BIOS)) biosLvl = t->currentLevel;

    double mult = 1.0 + (biosLvl * 0.10);
    mult *= GetPrestigeMultiplier();
    return mult;
}

double ResearchManager::GetProfitMultiplier() const {
    int aiLvl = 0;
    if (const auto* t = GetTechnology(TechType::AI_POOL_ROUTER)) aiLvl = t->currentLevel;

    double mult = 1.0 + (aiLvl * 0.12);
    mult *= GetPrestigeMultiplier();
    return mult;
}

double ResearchManager::GetHeatReductionMultiplier() const {
    int immLvl = 0;
    if (const auto* t = GetTechnology(TechType::IMMERSION_NANO)) immLvl = t->currentLevel;
    double reduction = immLvl * 0.08;
    return std::max(0.60, 1.0 - reduction);
}

double ResearchManager::GetPowerCostReductionMultiplier() const {
    int greenLvl = 0;
    if (const auto* t = GetTechnology(TechType::GREEN_POWER)) greenLvl = t->currentLevel;
    double reduction = greenLvl * 0.10;
    return std::max(0.50, 1.0 - reduction);
}

bool ResearchManager::HasSmartAutomation() const {
    if (const auto* t = GetTechnology(TechType::SMART_AUTOMATION)) {
        return t->currentLevel > 0;
    }
    return false;
}

int ResearchManager::GetVentureShares() const {
    return m_ventureShares;
}

int ResearchManager::GetPrestigeCount() const {
    return m_prestigeCount;
}

double ResearchManager::GetPrestigeMultiplier() const {
    return 1.0 + (m_ventureShares * 0.05); // Her hisse kalıcı +%5 çarpan
}

bool ResearchManager::CanExecuteIPO(double companyNetWorth) const {
    return companyNetWorth >= 50000.0;
}

int ResearchManager::CalculateClaimableShares(double companyNetWorth) const {
    if (companyNetWorth < 50000.0) return 0;
    return std::max(1, static_cast<int>(std::floor(std::sqrt(companyNetWorth / 5000.0))));
}

bool ResearchManager::ExecuteIPO(double companyNetWorth, EconomyManager& economy, Warehouse& warehouse) {
    int newShares = CalculateClaimableShares(companyNetWorth);
    if (newShares <= 0) return false;

    m_ventureShares += newShares;
    m_prestigeCount++;

    // Şirket operasyonlarını sıfırla, ama Ar-Ge ve Venture Hisselerini KORU!
    economy.SetFiatBalance(2500.0); // Başlangıç teşvik sermayesi
    economy.SetCryptoBalance(0.0);

    // Kasaları başlangıç durumuna döndür (1 adet boş rig)
    while (warehouse.GetRigCount() > 1) {
        warehouse.RemoveRig(warehouse.GetRigCount() - 1);
    }
    if (auto* firstRig = warehouse.GetRig(0)) {
        while (firstRig->GetGPUCount() > 0) {
            firstRig->RemoveGPU(firstRig->GetGPUCount() - 1);
        }
        firstRig->SetPoweredOn(true);
    }

    return true;
}

void ResearchManager::SetTechLevel(TechType type, int level) {
    for (auto& t : m_techs) {
        if (t.type == type) {
            t.currentLevel = std::clamp(level, 0, t.maxLevel);
            break;
        }
    }
}

void ResearchManager::SetVentureShares(int shares) {
    m_ventureShares = std::max(0, shares);
}

void ResearchManager::SetPrestigeCount(int count) {
    m_prestigeCount = std::max(0, count);
}

} // namespace Core
