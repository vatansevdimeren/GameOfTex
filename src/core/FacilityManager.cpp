#include "FacilityManager.hpp"
#include "EconomyManager.hpp"

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

} // namespace Core
