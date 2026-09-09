#include "SaveManager.hpp"
#include "UserProfile.hpp"
#include "EconomyManager.hpp"
#include "FacilityManager.hpp"
#include "TaskManager.hpp"
#include "MarketCatalog.hpp"
#include "Warehouse.hpp"
#include "MiningRig.hpp"
#include "GPU.hpp"
#include "PowerGrid.hpp"
#include "CoolingManager.hpp"
#include "ThermalModel.hpp"
#include "ResearchManager.hpp"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace Core {

namespace {

std::string Trim(const std::string& str) {
    auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string GetCurrentTimestampString() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M");
    return ss.str();
}

std::unordered_map<std::string, std::string> ParseKeyValueFile(const std::string& filepath) {
    std::unordered_map<std::string, std::string> kvMap;
    std::ifstream file(filepath);
    if (!file.is_open()) return kvMap;

    std::string currentSection = "";
    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = Trim(line);
        if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == ';') continue;

        if (trimmed.front() == '[' && trimmed.back() == ']') {
            currentSection = trimmed.substr(1, trimmed.length() - 2);
            continue;
        }

        auto eqPos = trimmed.find('=');
        if (eqPos != std::string::npos) {
            std::string key = Trim(trimmed.substr(0, eqPos));
            std::string val = Trim(trimmed.substr(eqPos + 1));
            std::string fullKey = currentSection.empty() ? key : (currentSection + "." + key);
            kvMap[fullKey] = val;
        }
    }
    return kvMap;
}

std::string GetVal(const std::unordered_map<std::string, std::string>& m, const std::string& key, const std::string& defVal = "") {
    auto it = m.find(key);
    return (it != m.end()) ? it->second : defVal;
}

double GetValDouble(const std::unordered_map<std::string, std::string>& m, const std::string& key, double defVal = 0.0) {
    auto it = m.find(key);
    if (it != m.end()) {
        try { return std::stod(it->second); } catch (...) {}
    }
    return defVal;
}

int GetValInt(const std::unordered_map<std::string, std::string>& m, const std::string& key, int defVal = 0) {
    auto it = m.find(key);
    if (it != m.end()) {
        try { return std::stoi(it->second); } catch (...) {}
    }
    return defVal;
}

bool GetValBool(const std::unordered_map<std::string, std::string>& m, const std::string& key, bool defVal = false) {
    auto it = m.find(key);
    if (it != m.end()) {
        return (it->second == "1" || it->second == "true" || it->second == "TRUE");
    }
    return defVal;
}

} // anonymous namespace

bool SaveManager::HasSaveFile(const std::string& filepath) {
    return std::filesystem::exists(filepath);
}

bool SaveManager::ReadSaveMetadata(SaveMetadata& outMetadata, const std::string& filepath) {
    if (!HasSaveFile(filepath)) return false;

    auto kv = ParseKeyValueFile(filepath);
    if (kv.empty()) return false;

    if (GetVal(kv, "HEADER.magic") != "GAMEOFTEX_SAVE") {
        return false;
    }

    outMetadata.companyName = GetVal(kv, "HEADER.companyName", "Eren Mining Co.");
    outMetadata.avatarIndex = GetValInt(kv, "HEADER.avatarIndex", 0);
    outMetadata.timestamp = GetVal(kv, "HEADER.timestamp", "");
    outMetadata.fiatBalance = GetValDouble(kv, "ECONOMY.fiat", 0.0);
    outMetadata.cryptoBalance = GetValDouble(kv, "ECONOMY.crypto", 0.0);
    outMetadata.activeFacilityName = GetVal(kv, "HEADER.activeFacilityName", "Teksas");
    outMetadata.totalRigs = GetValInt(kv, "HEADER.totalRigs", 1);
    outMetadata.totalGPUs = GetValInt(kv, "HEADER.totalGPUs", 0);
    outMetadata.isValid = true;
    return true;
}

bool SaveManager::SaveGame(const std::string& filepath,
                           const UserProfile& profile,
                           const EconomyManager& economy,
                           const FacilityManager& facilityManager,
                           const TaskManager& taskManager,
                           const MarketCatalog& marketCatalog,
                           const ResearchManager* researchManager) {
    std::ofstream file(filepath, std::ios::trunc);
    if (!file.is_open()) return false;

    int totalRigsCount = 0;
    int totalGPUsCount = 0;
    const auto& facilities = facilityManager.GetAllFacilities();
    for (const auto& fac : facilities) {
        if (fac.isPurchased && fac.warehouse) {
            totalRigsCount += static_cast<int>(fac.warehouse->GetRigCount());
            for (const auto& r : fac.warehouse->GetAllRigs()) {
                if (r) totalGPUsCount += static_cast<int>(r->GetGPUCount());
            }
        }
    }

    std::string activeFacName = "Teksas";
    if (auto* af = facilityManager.GetActiveFacility()) {
        activeFacName = af->warehouse ? af->warehouse->GetFacilityName() : "Teksas";
    }

    // 1. HEADER
    file << "[HEADER]\n";
    file << "magic=GAMEOFTEX_SAVE\n";
    file << "version=1\n";
    file << "timestamp=" << GetCurrentTimestampString() << "\n";
    file << "companyName=" << profile.GetCompanyName() << "\n";
    file << "avatarIndex=" << profile.GetAvatarIndex() << "\n";
    file << "claimedBonus=" << (profile.HasClaimedBonus() ? "1" : "0") << "\n";
    file << "activeFacilityName=" << activeFacName << "\n";
    file << "totalRigs=" << totalRigsCount << "\n";
    file << "totalGPUs=" << totalGPUsCount << "\n\n";

    // 2. ECONOMY
    file << "[ECONOMY]\n";
    file << "fiat=" << std::fixed << std::setprecision(2) << economy.GetFiatBalance() << "\n";
    file << "crypto=" << std::fixed << std::setprecision(6) << economy.GetCryptoBalance() << "\n";
    file << "price=" << std::fixed << std::setprecision(2) << economy.GetCryptoPrice() << "\n";
    file << "currency=" << static_cast<int>(economy.GetCurrency()) << "\n";
    file << "activeCoinIndex=" << economy.GetActiveCoinIndex() << "\n";
    file << "coinCount=" << economy.GetCoins().size() << "\n";
    for (size_t i = 0; i < economy.GetCoins().size(); ++i) {
        const auto& c = economy.GetCoins()[i];
        file << "coin_" << i << "_id=" << c.id << "\n";
        file << "coin_" << i << "_balance=" << std::fixed << std::setprecision(8) << c.balance << "\n";
        file << "coin_" << i << "_price=" << std::fixed << std::setprecision(4) << c.priceUSD << "\n";
    }
    file << "\n";

    // 3. FACILITIES
    file << "[FACILITIES]\n";
    file << "activeFacilityIndex=" << facilityManager.GetActiveFacilityIndex() << "\n";
    file << "facilityCount=" << facilities.size() << "\n\n";

    for (size_t fIdx = 0; fIdx < facilities.size(); ++fIdx) {
        const auto& fac = facilities[fIdx];
        std::string fSec = "FACILITY_" + std::to_string(fIdx);
        file << "[" << fSec << "]\n";
        file << "id=" << fac.id << "\n";
        file << "isPurchased=" << (fac.isPurchased ? "1" : "0") << "\n";
        
        if (fac.powerGrid) {
            file << "maxGridWatts=" << fac.powerGrid->GetMaxCapacityWatts() << "\n";
            file << "breakerTripped=" << (fac.powerGrid->IsBreakerTripped() ? "1" : "0") << "\n";
        }
        if (fac.thermalModel) {
            file << "roomTemp=" << fac.thermalModel->GetAmbientTemperature() << "\n";
        }
        if (fac.coolingManager) {
            const auto& tiers = fac.coolingManager->GetTiers();
            file << "coolingCount=" << tiers.size() << "\n";
            for (size_t cIdx = 0; cIdx < tiers.size(); ++cIdx) {
                file << "cooling_" << cIdx << "=" << (tiers[cIdx].isInstalled ? "1" : "0") << "\n";
            }
        }

        if (fac.warehouse) {
            file << "activeRigIndex=" << fac.warehouse->GetActiveRigIndex() << "\n";
            const auto& rigs = fac.warehouse->GetAllRigs();
            file << "rigCount=" << rigs.size() << "\n";
            for (size_t rIdx = 0; rIdx < rigs.size(); ++rIdx) {
                const auto* rig = rigs[rIdx].get();
                if (!rig) continue;
                std::string rPrefix = "rig_" + std::to_string(rIdx) + "_";
                file << rPrefix << "name=" << rig->GetName() << "\n";
                file << rPrefix << "maxCapacity=" << rig->GetMaxCapacity() << "\n";
                file << rPrefix << "psuTier=" << rig->GetPSUTier() << "\n";
                file << rPrefix << "isPowered=" << (rig->IsPoweredOn() ? "1" : "0") << "\n";
                file << rPrefix << "gpuCount=" << rig->GetGPUCount() << "\n";

                for (size_t gIdx = 0; gIdx < rig->GetGPUCount(); ++gIdx) {
                    const auto* gpu = rig->GetGPU(gIdx);
                    if (!gpu) continue;
                    std::string gPrefix = rPrefix + "gpu_" + std::to_string(gIdx) + "_";
                    file << gPrefix << "name=" << gpu->GetName() << "\n";
                    file << gPrefix << "baseHash=" << gpu->GetBaseHashrate() << "\n";
                    file << gPrefix << "basePower=" << gpu->GetBasePowerWatts() << "\n";
                    file << gPrefix << "silicon=" << gpu->GetSiliconQuality() << "\n";
                    file << gPrefix << "oc=" << gpu->GetOverclockMultiplier() << "\n";
                    file << gPrefix << "clockOffset=" << gpu->GetCoreClockOffset() << "\n";
                    file << gPrefix << "powerLimit=" << gpu->GetPowerLimitPercent() << "\n";
                    file << gPrefix << "fanSpeed=" << gpu->GetFanSpeedPercent() << "\n";
                    file << gPrefix << "health=" << gpu->GetHealthPercent() << "\n";
                    file << gPrefix << "isBurnt=" << (gpu->IsBurnt() ? "1" : "0") << "\n";
                }
            }
        }
        file << "\n";
    }

    // 4. MARKET UPGRADES
    file << "[MARKET]\n";
    const auto& powerUpgrades = marketCatalog.GetPowerUpgrades();
    file << "powerUpgradeCount=" << powerUpgrades.size() << "\n";
    for (size_t i = 0; i < powerUpgrades.size(); ++i) {
        file << "power_" << i << "=" << (powerUpgrades[i].isInstalled ? "1" : "0") << "\n";
    }

    const auto& facUpgrades = marketCatalog.GetFacilityUpgrades();
    file << "facilityUpgradeCount=" << facUpgrades.size() << "\n";
    for (size_t i = 0; i < facUpgrades.size(); ++i) {
        file << "facility_" << i << "=" << (facUpgrades[i].isInstalled ? "1" : "0") << "\n";
    }
    file << "\n";

    // 5. TASKS
    file << "[TASKS]\n";
    const auto& tasks = taskManager.GetTasks();
    file << "taskCount=" << tasks.size() << "\n";
    for (size_t i = 0; i < tasks.size(); ++i) {
        std::string tPrefix = "task_" + std::to_string(i) + "_";
        file << tPrefix << "id=" << tasks[i].id << "\n";
        file << tPrefix << "completed=" << (tasks[i].isCompleted ? "1" : "0") << "\n";
        file << tPrefix << "claimed=" << (tasks[i].isClaimed ? "1" : "0") << "\n";
        file << tPrefix << "progress=" << tasks[i].currentProgress << "\n";
    }
    file << "\n";

    // 6. RESEARCH & PRESTIGE
    if (researchManager) {
        file << "[RESEARCH]\n";
        file << "ventureShares=" << researchManager->GetVentureShares() << "\n";
        file << "prestigeCount=" << researchManager->GetPrestigeCount() << "\n";
        const auto& techs = researchManager->GetTechnologies();
        file << "techCount=" << techs.size() << "\n";
        for (size_t i = 0; i < techs.size(); ++i) {
            file << "tech_" << i << "_id=" << techs[i].id << "\n";
            file << "tech_" << i << "_level=" << techs[i].currentLevel << "\n";
        }
        file << "\n";
    }

    return true;
}

bool SaveManager::LoadGame(const std::string& filepath,
                           UserProfile& profile,
                           EconomyManager& economy,
                           FacilityManager& facilityManager,
                           TaskManager& taskManager,
                           MarketCatalog& marketCatalog,
                           ResearchManager* researchManager) {
    if (!HasSaveFile(filepath)) return false;

    auto kv = ParseKeyValueFile(filepath);
    if (kv.empty() || GetVal(kv, "HEADER.magic") != "GAMEOFTEX_SAVE") {
        return false;
    }

    // 1. PROFILE
    profile.SetCompanyName(GetVal(kv, "HEADER.companyName", "Eren Mining Co."));
    profile.SetAvatarIndex(GetValInt(kv, "HEADER.avatarIndex", 0));
    profile.SetClaimedBonus(GetValBool(kv, "HEADER.claimedBonus", true));

    // 2. ECONOMY
    economy.SetFiatBalance(GetValDouble(kv, "ECONOMY.fiat", 1500.0));
    economy.SetCryptoBalance(GetValDouble(kv, "ECONOMY.crypto", 0.0));
    economy.SetCryptoPrice(GetValDouble(kv, "ECONOMY.price", 2.40));
    int currVal = GetValInt(kv, "ECONOMY.currency", 0);
    if (currVal >= 0 && currVal <= 3) {
        economy.SetCurrency(static_cast<CurrencyType>(currVal));
    }
    size_t activeCoin = static_cast<size_t>(GetValInt(kv, "ECONOMY.activeCoinIndex", 0));
    economy.SetActiveCoinIndex(activeCoin);

    size_t coinCount = static_cast<size_t>(GetValInt(kv, "ECONOMY.coinCount", 0));
    for (size_t i = 0; i < coinCount; ++i) {
        std::string cId = GetVal(kv, "ECONOMY.coin_" + std::to_string(i) + "_id", "");
        if (!cId.empty()) {
            double bal = GetValDouble(kv, "ECONOMY.coin_" + std::to_string(i) + "_balance", 0.0);
            double prc = GetValDouble(kv, "ECONOMY.coin_" + std::to_string(i) + "_price", 0.0);
            economy.SetCoinBalance(cId, bal);
            if (prc > 0.0) {
                economy.SetCoinPrice(cId, prc);
            }
        }
    }

    // 3. FACILITIES
    size_t activeFacIdx = static_cast<size_t>(GetValInt(kv, "FACILITIES.activeFacilityIndex", 0));
    facilityManager.SetActiveFacilityIndex(activeFacIdx);

    size_t facCount = static_cast<size_t>(GetValInt(kv, "FACILITIES.facilityCount", 0));
    for (size_t fIdx = 0; fIdx < facCount && fIdx < facilityManager.GetFacilityCount(); ++fIdx) {
        auto* fac = facilityManager.GetFacility(fIdx);
        if (!fac) continue;

        std::string fSec = "FACILITY_" + std::to_string(fIdx) + ".";
        fac->isPurchased = GetValBool(kv, fSec + "isPurchased", fIdx == 0);

        if (fac->powerGrid) {
            double maxWatts = GetValDouble(kv, fSec + "maxGridWatts", fac->baseGridWatts);
            fac->powerGrid->SetMaxCapacityWatts(maxWatts);
            fac->powerGrid->SetBreakerTripped(GetValBool(kv, fSec + "breakerTripped", false));
        }

        if (fac->coolingManager) {
            size_t cCount = static_cast<size_t>(GetValInt(kv, fSec + "coolingCount", 0));
            for (size_t cIdx = 0; cIdx < cCount; ++cIdx) {
                bool inst = GetValBool(kv, fSec + "cooling_" + std::to_string(cIdx), cIdx == 0);
                fac->coolingManager->SetTierInstalled(cIdx, inst);
            }
            if (fac->thermalModel) {
                fac->thermalModel->SetCoolingPowerWatts(fac->coolingManager->CalculateTotalCoolingWatts());
            }
        }

        if (fac->warehouse) {
            size_t rigCount = static_cast<size_t>(GetValInt(kv, fSec + "rigCount", 0));
            if (rigCount > 0) {
                fac->warehouse->ClearRigs();
                for (size_t rIdx = 0; rIdx < rigCount; ++rIdx) {
                    std::string rPrefix = fSec + "rig_" + std::to_string(rIdx) + "_";
                    std::string rName = GetVal(kv, rPrefix + "name", "Rig " + std::to_string(rIdx + 1));
                    size_t maxCap = static_cast<size_t>(GetValInt(kv, rPrefix + "maxCapacity", 6));
                    size_t psuTier = static_cast<size_t>(GetValInt(kv, rPrefix + "psuTier", 0));
                    bool isPowered = GetValBool(kv, rPrefix + "isPowered", true);
                    size_t gpuCount = static_cast<size_t>(GetValInt(kv, rPrefix + "gpuCount", 0));

                    auto newRig = std::make_unique<MiningRig>(rName, maxCap);
                    newRig->SetPSUTier(psuTier);
                    newRig->SetPoweredOn(isPowered);

                    for (size_t gIdx = 0; gIdx < gpuCount; ++gIdx) {
                        std::string gPrefix = rPrefix + "gpu_" + std::to_string(gIdx) + "_";
                        std::string gName = GetVal(kv, gPrefix + "name", "RTX 3080");
                        double bHash = GetValDouble(kv, gPrefix + "baseHash", 100.0);
                        double bPower = GetValDouble(kv, gPrefix + "basePower", 220.0);
                        double sil = GetValDouble(kv, gPrefix + "silicon", 1.0);
                        double oc = GetValDouble(kv, gPrefix + "oc", 1.0);
                        double clkOff = GetValDouble(kv, gPrefix + "clockOffset", 0.0);
                        double pwrLim = GetValDouble(kv, gPrefix + "powerLimit", 100.0);
                        double fan = GetValDouble(kv, gPrefix + "fanSpeed", 75.0);
                        double hp = GetValDouble(kv, gPrefix + "health", 100.0);
                        bool isBurnt = GetValBool(kv, gPrefix + "isBurnt", false);

                        auto gpu = std::make_unique<GPU>(gName, bHash, bPower, sil);
                        gpu->SetOverclockMultiplier(oc);
                        gpu->SetCoreClockOffset(clkOff);
                        gpu->SetPowerLimitPercent(pwrLim);
                        gpu->SetFanSpeedPercent(fan);
                        gpu->SetHealthPercent(hp);
                        gpu->SetBurnt(isBurnt);

                        newRig->InstallGPU(std::move(gpu));
                    }
                    fac->warehouse->AddRig(std::move(newRig));
                }

                size_t activeRig = static_cast<size_t>(GetValInt(kv, fSec + "activeRigIndex", 0));
                fac->warehouse->SetActiveRigIndex(activeRig);
            }
        }
    }

    // 4. MARKET UPGRADES
    size_t powerUpgCount = static_cast<size_t>(GetValInt(kv, "MARKET.powerUpgradeCount", 0));
    for (size_t i = 0; i < powerUpgCount; ++i) {
        bool inst = GetValBool(kv, "MARKET.power_" + std::to_string(i), false);
        marketCatalog.SetPowerUpgradeInstalled(i, inst);
    }
    size_t facUpgCount = static_cast<size_t>(GetValInt(kv, "MARKET.facilityUpgradeCount", 0));
    for (size_t i = 0; i < facUpgCount; ++i) {
        bool inst = GetValBool(kv, "MARKET.facility_" + std::to_string(i), false);
        marketCatalog.SetFacilityUpgradeInstalled(i, inst);
    }

    // 5. TASKS
    size_t taskCount = static_cast<size_t>(GetValInt(kv, "TASKS.taskCount", 0));
    for (size_t i = 0; i < taskCount; ++i) {
        std::string tPrefix = "TASKS.task_" + std::to_string(i) + "_";
        std::string tId = GetVal(kv, tPrefix + "id");
        if (!tId.empty()) {
            bool comp = GetValBool(kv, tPrefix + "completed", false);
            bool claimed = GetValBool(kv, tPrefix + "claimed", false);
            double prog = GetValDouble(kv, tPrefix + "progress", 0.0);
            taskManager.SetTaskState(tId, comp, claimed, prog);
        }
    }

    // 6. RESEARCH & PRESTIGE
    if (researchManager) {
        int shares = GetValInt(kv, "RESEARCH.ventureShares", 0);
        int prestige = GetValInt(kv, "RESEARCH.prestigeCount", 0);
        researchManager->SetVentureShares(shares);
        researchManager->SetPrestigeCount(prestige);

        size_t techCount = static_cast<size_t>(GetValInt(kv, "RESEARCH.techCount", 0));
        for (size_t i = 0; i < techCount; ++i) {
            std::string tId = GetVal(kv, "RESEARCH.tech_" + std::to_string(i) + "_id");
            int lvl = GetValInt(kv, "RESEARCH.tech_" + std::to_string(i) + "_level", 0);
            if (tId == "tech_bios") researchManager->SetTechLevel(TechType::CUSTOM_BIOS, lvl);
            else if (tId == "tech_ai_pool") researchManager->SetTechLevel(TechType::AI_POOL_ROUTER, lvl);
            else if (tId == "tech_immersion") researchManager->SetTechLevel(TechType::IMMERSION_NANO, lvl);
            else if (tId == "tech_green_power") researchManager->SetTechLevel(TechType::GREEN_POWER, lvl);
            else if (tId == "tech_automation") researchManager->SetTechLevel(TechType::SMART_AUTOMATION, lvl);
        }
    }

    return true;
}

bool SaveManager::DeleteSaveFile(const std::string& filepath) {
    if (HasSaveFile(filepath)) {
        return std::filesystem::remove(filepath);
    }
    return false;
}

} // namespace Core
