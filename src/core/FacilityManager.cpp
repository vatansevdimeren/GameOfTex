#include "FacilityManager.hpp"
#include "EconomyManager.hpp"
#include <random>
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace Core {

FacilityManager::FacilityManager() {
    // 1. Austin, Texas, USA (Starter Facility)
    {
        FacilityLocation fac;
        fac.id = "FAC_TEXAS";
        fac.nameKey = "FAC_TEXAS_NAME";
        fac.countryKey = "FAC_TEXAS_COUNTRY";
        fac.descKey = "FAC_TEXAS_DESC";
        fac.climateTempCelsius = 30.0;
        fac.baseGridWatts = 15000.0;
        fac.gridPricePerKwh = 0.14;
        fac.maxRigCapacity = 5;
        fac.purchaseCostUSD = 0.0;
        fac.isPurchased = true;
        fac.mapNormX = 0.22f;
        fac.mapNormY = 0.44f;

        fac.warehouse = std::make_unique<Warehouse>("Teksas Madencilik Hangari - Sektor 01");
        fac.warehouse->SetMaxRigCapacity(5);
        if (auto* rig = fac.warehouse->GetActiveRig()) {
            rig->InstallGPU(std::make_unique<GPU>("RTX 3080", 100.0, 220.0, 1.05));
            rig->InstallGPU(std::make_unique<GPU>("RTX 3070", 62.0, 130.0, 0.98));
        }
        fac.thermalModel = std::make_unique<ThermalModel>(fac.climateTempCelsius);
        fac.powerGrid = std::make_unique<PowerGrid>(fac.baseGridWatts, fac.gridPricePerKwh);
        fac.coolingManager = std::make_unique<CoolingManager>();
        fac.thermalModel->SetCoolingPowerWatts(fac.coolingManager->CalculateTotalCoolingWatts());

        fac.economist.name = "Dr. Austin Miller";
        fac.economist.title = "Stajyer Finans Analisti";

        m_facilities.push_back(std::move(fac));
    }

    // 2. Reykjavik, Iceland (Geothermal Cold Valley)
    {
        FacilityLocation fac;
        fac.id = "FAC_ICELAND";
        fac.nameKey = "FAC_ICELAND_NAME";
        fac.countryKey = "FAC_ICELAND_COUNTRY";
        fac.descKey = "FAC_ICELAND_DESC";
        fac.climateTempCelsius = -2.0;
        fac.baseGridWatts = 35000.0;
        fac.gridPricePerKwh = 0.05;
        fac.maxRigCapacity = 8;
        fac.purchaseCostUSD = 8500.0;
        fac.isPurchased = false;
        fac.mapNormX = 0.43f;
        fac.mapNormY = 0.22f;

        fac.warehouse = std::make_unique<Warehouse>("Reykjavik Jeotermal Tesisi");
        fac.warehouse->SetMaxRigCapacity(8);
        fac.thermalModel = std::make_unique<ThermalModel>(fac.climateTempCelsius);
        fac.powerGrid = std::make_unique<PowerGrid>(fac.baseGridWatts, fac.gridPricePerKwh);
        fac.coolingManager = std::make_unique<CoolingManager>();
        fac.thermalModel->SetCoolingPowerWatts(fac.coolingManager->CalculateTotalCoolingWatts());

        fac.economist.name = "Gunnar Sigurdsson";
        fac.economist.title = "Stajyer Finans Analisti";

        m_facilities.push_back(std::move(fac));
    }

    // 3. Tromso, Norway (Fjord Underwater Facility)
    {
        FacilityLocation fac;
        fac.id = "FAC_NORWAY";
        fac.nameKey = "FAC_NORWAY_NAME";
        fac.countryKey = "FAC_NORWAY_COUNTRY";
        fac.descKey = "FAC_NORWAY_DESC";
        fac.climateTempCelsius = -6.0;
        fac.baseGridWatts = 65000.0;
        fac.gridPricePerKwh = 0.06;
        fac.maxRigCapacity = 14;
        fac.purchaseCostUSD = 24000.0;
        fac.isPurchased = false;
        fac.mapNormX = 0.54f;
        fac.mapNormY = 0.18f;

        fac.warehouse = std::make_unique<Warehouse>("Tromso Fiyort Sualti Veri Parki");
        fac.warehouse->SetMaxRigCapacity(14);
        fac.thermalModel = std::make_unique<ThermalModel>(fac.climateTempCelsius);
        fac.powerGrid = std::make_unique<PowerGrid>(fac.baseGridWatts, fac.gridPricePerKwh);
        fac.coolingManager = std::make_unique<CoolingManager>();
        fac.thermalModel->SetCoolingPowerWatts(fac.coolingManager->CalculateTotalCoolingWatts());

        fac.economist.name = "Freja Lind";
        fac.economist.title = "Stajyer Finans Analisti";

        m_facilities.push_back(std::move(fac));
    }

    // 4. Frankfurt, Germany (Industrial Data Park)
    {
        FacilityLocation fac;
        fac.id = "FAC_GERMANY";
        fac.nameKey = "FAC_GERMANY_NAME";
        fac.countryKey = "FAC_GERMANY_COUNTRY";
        fac.descKey = "FAC_GERMANY_DESC";
        fac.climateTempCelsius = 16.0;
        fac.baseGridWatts = 120000.0;
        fac.gridPricePerKwh = 0.18;
        fac.maxRigCapacity = 20;
        fac.purchaseCostUSD = 55000.0;
        fac.isPurchased = false;
        fac.mapNormX = 0.51f;
        fac.mapNormY = 0.33f;

        fac.warehouse = std::make_unique<Warehouse>("Frankfurt Endustriyel Veri Parki");
        fac.warehouse->SetMaxRigCapacity(20);
        fac.thermalModel = std::make_unique<ThermalModel>(fac.climateTempCelsius);
        fac.powerGrid = std::make_unique<PowerGrid>(fac.baseGridWatts, fac.gridPricePerKwh);
        fac.coolingManager = std::make_unique<CoolingManager>();
        fac.thermalModel->SetCoolingPowerWatts(fac.coolingManager->CalculateTotalCoolingWatts());

        fac.economist.name = "Hans Becker";
        fac.economist.title = "Stajyer Finans Analisti";

        m_facilities.push_back(std::move(fac));
    }

    // 5. Novosibirsk, Siberia, Russia (Arctic Mining Base)
    {
        FacilityLocation fac;
        fac.id = "FAC_SIBERIA";
        fac.nameKey = "FAC_SIBERIA_NAME";
        fac.countryKey = "FAC_SIBERIA_COUNTRY";
        fac.descKey = "FAC_SIBERIA_DESC";
        fac.climateTempCelsius = -18.0;
        fac.baseGridWatts = 250000.0;
        fac.gridPricePerKwh = 0.04;
        fac.maxRigCapacity = 30;
        fac.purchaseCostUSD = 120000.0;
        fac.isPurchased = false;
        fac.mapNormX = 0.73f;
        fac.mapNormY = 0.27f;

        fac.warehouse = std::make_unique<Warehouse>("Sibirya Kutup Madencilik Ussu");
        fac.warehouse->SetMaxRigCapacity(30);
        fac.thermalModel = std::make_unique<ThermalModel>(fac.climateTempCelsius);
        fac.powerGrid = std::make_unique<PowerGrid>(fac.baseGridWatts, fac.gridPricePerKwh);
        fac.coolingManager = std::make_unique<CoolingManager>();
        fac.thermalModel->SetCoolingPowerWatts(fac.coolingManager->CalculateTotalCoolingWatts());

        fac.economist.name = "Dmitri Volkov";
        fac.economist.title = "Stajyer Finans Analisti";

        m_facilities.push_back(std::move(fac));
    }
}

size_t FacilityManager::GetFacilityCount() const {
    return m_facilities.size();
}

size_t FacilityManager::GetActiveFacilityIndex() const {
    return m_activeFacilityIndex;
}

void FacilityManager::SetActiveFacilityIndex(size_t index) {
    if (index < m_facilities.size()) {
        m_activeFacilityIndex = index;
    }
}

FacilityLocation* FacilityManager::GetActiveFacility() {
    if (m_activeFacilityIndex < m_facilities.size()) {
        return &m_facilities[m_activeFacilityIndex];
    }
    return nullptr;
}

const FacilityLocation* FacilityManager::GetActiveFacility() const {
    if (m_activeFacilityIndex < m_facilities.size()) {
        return &m_facilities[m_activeFacilityIndex];
    }
    return nullptr;
}

FacilityLocation* FacilityManager::GetFacility(size_t index) {
    if (index < m_facilities.size()) {
        return &m_facilities[index];
    }
    return nullptr;
}

const FacilityLocation* FacilityManager::GetFacility(size_t index) const {
    if (index < m_facilities.size()) {
        return &m_facilities[index];
    }
    return nullptr;
}

const std::vector<FacilityLocation>& FacilityManager::GetAllFacilities() const {
    return m_facilities;
}

bool FacilityManager::PurchaseFacility(size_t index, EconomyManager& economy) {
    if (index >= m_facilities.size()) return false;
    auto& fac = m_facilities[index];
    if (fac.isPurchased) return true;

    if (economy.DeductFiat(fac.purchaseCostUSD)) {
        fac.isPurchased = true;
        m_activeFacilityIndex = index;
        return true;
    }
    return false;
}

bool FacilityManager::SwitchFacility(size_t index) {
    if (index < m_facilities.size() && m_facilities[index].isPurchased) {
        m_activeFacilityIndex = index;
        return true;
    }
    return false;
}

double FacilityManager::GetEconomistUpgradeCost(size_t facilityIndex) const {
    if (facilityIndex >= m_facilities.size()) return 0.0;
    const auto& fac = m_facilities[facilityIndex];
    int curLevel = fac.economist.level;
    if (curLevel >= 5) return 0.0;

    double baseCosts[] = {1200.0, 2400.0, 6000.0, 15000.0, 35000.0};
    double base = (facilityIndex < 5) ? baseCosts[facilityIndex] : 2000.0;

    switch (curLevel) {
        case 0: return base;
        case 1: return base * 2.5;
        case 2: return base * 5.0;
        case 3: return base * 12.0;
        case 4: return base * 28.0;
        default: return 0.0;
    }
}

std::string FacilityManager::GetEconomistNextTitle(size_t facilityIndex) const {
    if (facilityIndex >= m_facilities.size()) return "";
    int nextLevel = m_facilities[facilityIndex].economist.level + 1;
    switch (nextLevel) {
        case 1: return "Stajyer Finans Analisti";
        case 2: return "Kidemli Piyasa Analisti";
        case 3: return "Kripto Portfoy Yoneticisi";
        case 4: return "Kantitatif Algoritmik Trader";
        case 5: return "Yapay Zeka Destekli Bas Ekonomist";
        default: return "Maksimum Seviye";
    }
}

bool FacilityManager::HireOrUpgradeEconomist(size_t facilityIndex, EconomyManager& economy) {
    if (facilityIndex >= m_facilities.size()) return false;
    auto& fac = m_facilities[facilityIndex];
    if (fac.economist.level >= 5) return false;

    double cost = GetEconomistUpgradeCost(facilityIndex);
    if (cost <= 0.0 || economy.GetFiatBalance() < cost) return false;

    if (economy.DeductFiat(cost)) {
        fac.economist.ApplyLevelStats(fac.economist.level + 1);
        return true;
    }
    return false;
}

void FacilityManager::UpdateEconomists(double dt, EconomyManager& economy, std::string& outNotification) {
    outNotification.clear();
    static std::mt19937 rng(4242);

    for (size_t i = 0; i < m_facilities.size(); ++i) {
        auto& fac = m_facilities[i];
        if (!fac.isPurchased || fac.economist.level <= 0) continue;

        fac.economist.tradeTimer += dt;
        if (fac.economist.tradeTimer >= fac.economist.tradeInterval) {
            fac.economist.tradeTimer = 0.0;

            double fiat = economy.GetFiatBalance();
            if (fiat < 150.0) continue;

            double baseTrades[] = { 180.0, 360.0, 850.0, 2200.0, 5500.0 };
            double baseAmount = (i < 5 ? baseTrades[i] : 250.0) * std::pow(1.8, fac.economist.level - 1);

            std::uniform_real_distribution<double> rollDist(0.0, 1.0);
            std::uniform_real_distribution<double> varDist(0.85, 1.25);
            double roll = rollDist(rng);

            if (roll < fac.economist.winRate) {
                // KÂRLI İŞLEM
                double profit = baseAmount * varDist(rng);
                economy.AddFiat(profit);
                fac.economist.lastTradeProfit = profit;
                fac.economist.totalProfitLifetime += profit;
                fac.economist.successfulTrades++;
                fac.economist.lastTradeLog = "+" + economy.FormatFiat(profit) + " (Kar)";

                if (outNotification.empty()) {
                    outNotification = fac.economist.name + ": Basarili Arbitraj (+" + economy.FormatFiat(profit) + ")";
                }
            } else {
                // ZARARLI İŞLEM (Piyasa Düzeltmesi Kaybı - En fazla mevcut nakdin %12'si)
                double lossRaw = baseAmount * 0.55 * varDist(rng);
                double maxLoss = fiat * 0.12;
                double actualLoss = std::clamp(lossRaw, 10.0, maxLoss);

                economy.DeductFiat(actualLoss);
                fac.economist.lastTradeProfit = -actualLoss;
                fac.economist.totalProfitLifetime -= actualLoss;
                fac.economist.failedTrades++;
                fac.economist.lastTradeLog = "-" + economy.FormatFiat(actualLoss) + " (Zarar)";

                if (outNotification.empty()) {
                    outNotification = fac.economist.name + ": Piyasa Kaybi (-" + economy.FormatFiat(actualLoss) + ")";
                }
            }
        }
    }
}

} // namespace Core
