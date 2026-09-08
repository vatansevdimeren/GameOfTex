#pragma once

#include <string>
#include <vector>

namespace Core {

class Warehouse;
class EconomyManager;
class CoolingManager;
class PowerGrid;

/**
 * @struct GameTask
 * @brief Represents a single achievable quest or mission with conditions and rewards.
 */
struct GameTask {
    std::string id;
    std::string titleKey;
    std::string descKey;
    double targetProgress{1.0};
    double currentProgress{0.0};
    double rewardCash{0.0};
    double rewardCrypto{0.0};
    bool isCompleted{false};
    bool isClaimed{false};
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
                        const PowerGrid& powerGrid);

    // Event hooks
    void NotifyCardInspected();
    void NotifyCardOverclocked();
    void NotifyCryptoSold(double usdAmount);
    void NotifyGpuPurchased();

    [[nodiscard]] const std::vector<GameTask>& GetTasks() const;
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
};

} // namespace Core
