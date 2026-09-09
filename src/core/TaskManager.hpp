#pragma once

#include <string>
#include <vector>

namespace Core {

class Warehouse;
class EconomyManager;
class CoolingManager;
class PowerGrid;
class FacilityManager;

enum class TaskCategory {
    ALL,
    CASH,       // Gelir & Nakit Servet Kademeleri
    CRYPTO,     // Kripto Madencilik & Çapraz Coin Ödülleri
    FACILITY,   // Fabrika & Tesis Operasyonları
    HARDWARE,   // Donanım, Rig Şasisi, CPU ve GPU
    TRADING     // Borsa Ticareti, Haberler ve Şirket Portföyü
};

/**
 * @struct GameTask
 * @brief Represents a single achievable quest or mission with conditions and rewards.
 */
struct GameTask {
    std::string id;
    std::string titleKey;
    std::string descKey;
    TaskCategory category{TaskCategory::CASH};
    std::string requiredFacilityId; // e.g. "FAC_ICELAND" (opens when facility is purchased)
    double targetProgress{1.0};
    double currentProgress{0.0};
    double rewardCash{0.0};
    std::string rewardCryptoSymbol; // e.g. "SOL", "BTC", "ETHW", "XMR", "TEX", "RVN", "ETC"
    double rewardCryptoAmount{0.0};
    bool isCompleted{false};
    bool isClaimed{false};
    bool isUnlocked{true};          // True if requirements (like purchased facility) are met
};

/**
 * @class TaskManager
 * @brief Manages quests, achievements, progression tracking, and rewards.
 * 
 * SRP: Solely responsible for tracking player milestones and issuing economic rewards.
 */
class TaskManager {
public:
    TaskManager();

    /**
     * @brief Evaluates active game state against all task goals.
     */
    void UpdateProgress(const Warehouse& warehouse,
                        const EconomyManager& economy,
                        const CoolingManager& cooling,
                        const PowerGrid& powerGrid,
                        const FacilityManager* facilityManager = nullptr);

    // Event hooks
    void NotifyCardInspected();
    void NotifyCardOverclocked();
    void NotifyCryptoSold(double usdAmount);
    void NotifyGpuPurchased();
    void NotifyTradeExecuted();
    void NotifyNewsRead();
    void NotifyRigFrameUpgraded();
    void NotifyCPUPurchased();

    [[nodiscard]] const std::vector<GameTask>& GetTasks() const;
    [[nodiscard]] std::vector<size_t> GetTaskIndicesByCategory(TaskCategory cat) const;
    [[nodiscard]] size_t GetUnclaimedCompletedCount() const;

    /**
     * @brief Claims the reward for a completed task and credits the player's wallet.
     * @return True if claimed successfully, false if already claimed or incomplete.
     */
    bool ClaimReward(const std::string& taskId, EconomyManager& economy);
    void SetTaskState(const std::string& taskId, bool completed, bool claimed, double progress);

private:
    std::vector<GameTask> m_tasks;
    bool m_cardInspected{false};
    bool m_cardOverclocked{false};
    double m_totalCryptoSoldUsd{0.0};
    int m_gpusPurchasedCount{0};
    int m_tradesExecutedCount{0};
    int m_newsReadCount{0};
    int m_rigFrameUpgradesCount{0};
    int m_cpusPurchasedCount{0};
};

} // namespace Core
