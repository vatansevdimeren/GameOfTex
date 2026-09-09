#pragma once

#include <string>
#include <vector>

namespace Core {
class EconomyManager;
class Warehouse;

enum class TechType {
    CUSTOM_BIOS,      // +10% Hashrate per lvl (Max 5)
    AI_POOL_ROUTER,   // +12% Mining Profit per lvl (Max 5)
    IMMERSION_NANO,   // -8% Heat Generation per lvl (Max 5)
    GREEN_POWER,      // -10% Electricity Cost per lvl (Max 5)
    SMART_AUTOMATION  // Auto-resets tripped breaker & maxes fans during overheat (Max 1)
};

struct TechItem {
    TechType type;
    std::string id;
    std::string nameTr;
    std::string nameEn;
    std::string descTr;
    std::string descEn;
    int currentLevel{0};
    int maxLevel{5};
    double baseCost{1000.0};
    double costMultiplier{2.2};
};

/**
 * @class ResearchManager
 * @brief Manages permanent research technology upgrades, passive multipliers, and IPO / Prestige progression.
 */
class ResearchManager {
public:
    ResearchManager();

    [[nodiscard]] const std::vector<TechItem>& GetTechnologies() const;
    [[nodiscard]] const TechItem* GetTechnology(TechType type) const;

    [[nodiscard]] double GetTechCost(TechType type) const;
    bool UpgradeTech(TechType type, EconomyManager& economy);

    // Multipliers
    [[nodiscard]] double GetHashrateMultiplier() const;
    [[nodiscard]] double GetProfitMultiplier() const;
    [[nodiscard]] double GetHeatReductionMultiplier() const;
    [[nodiscard]] double GetPowerCostReductionMultiplier() const;
    [[nodiscard]] bool HasSmartAutomation() const;

    // Prestige / IPO (Halka Arz)
    [[nodiscard]] int GetVentureShares() const;
    [[nodiscard]] int GetPrestigeCount() const;
    [[nodiscard]] double GetPrestigeMultiplier() const;
    [[nodiscard]] bool CanExecuteIPO(double companyNetWorth) const;
    [[nodiscard]] int CalculateClaimableShares(double companyNetWorth) const;

    /**
     * @brief Executes IPO / Prestige: Resets operational warehouse/fiat to fresh state,
     * but adds permanent Venture Shares and preserves all unlocked Technologies!
     */
    bool ExecuteIPO(double companyNetWorth, EconomyManager& economy, Warehouse& warehouse);

    // Serialization getters/setters for SaveManager
    void SetTechLevel(TechType type, int level);
    void SetVentureShares(int shares);
    void SetPrestigeCount(int count);

private:
    std::vector<TechItem> m_techs;
    int m_ventureShares{0};
    int m_prestigeCount{0};
};

} // namespace Core
