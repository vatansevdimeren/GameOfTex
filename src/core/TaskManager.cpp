#include "TaskManager.hpp"
#include "Warehouse.hpp"
#include "EconomyManager.hpp"
#include "CoolingManager.hpp"
#include "PowerGrid.hpp"
#include "FacilityManager.hpp"
#include <algorithm>

namespace Core {

TaskManager::TaskManager() {
    // 20 Dengeli & Katmanlı Görev (Balanced Multi-Tier Quests)
    m_tasks = {
        // --- SEVİYE 1: BAŞLANGIÇ & DONANIM ---
        {
            "TASK_INSPECT",
            "TASK_TITLE_INSPECT",
            "TASK_DESC_INSPECT",
            1.0, 0.0, 200.0, 0.0, false, false
        },
        {
            "TASK_OVERCLOCK",
            "TASK_TITLE_OVERCLOCK",
            "TASK_DESC_OVERCLOCK",
            1.0, 0.0, 350.0, 0.0, false, false
        },
        {
            "TASK_BUY_GPU",
            "TASK_TITLE_BUY_GPU",
            "TASK_DESC_BUY_GPU",
            1.0, 0.0, 400.0, 0.0, false, false
        },
        {
            "TASK_FIRST_TRADE",
            "TASK_TITLE_FIRST_TRADE",
            "TASK_DESC_FIRST_TRADE",
            1.0, 0.0, 300.0, 0.0, false, false
        },
        {
            "TASK_READ_NEWS",
            "TASK_TITLE_READ_NEWS",
            "TASK_DESC_READ_NEWS",
            3.0, 0.0, 250.0, 0.0, false, false
        },
        {
            "TASK_HASHRATE_50",
            "TASK_TITLE_HASHRATE_50",
            "TASK_DESC_HASHRATE_50",
            50.0, 0.0, 350.0, 0.0, false, false
        },
        {
            "TASK_MINE_TEX_100",
            "TASK_TITLE_MINE_TEX_100",
            "TASK_DESC_MINE_TEX_100",
            100.0, 0.0, 500.0, 0.0, false, false
        },
        {
            "TASK_MINE_RVN_1000",
            "TASK_TITLE_MINE_RVN_1000",
            "TASK_DESC_MINE_RVN_1000",
            1000.0, 0.0, 650.0, 0.0, false, false
        },

        // --- SEVİYE 2: TİCARET & GELİŞİM ---
        {
            "TASK_SELL_CRYPTO",
            "TASK_TITLE_SELL_CRYPTO",
            "TASK_DESC_SELL_CRYPTO",
            500.0, 0.0, 450.0, 0.0, false, false
        },
        {
            "TASK_MULTI_RIG",
            "TASK_TITLE_MULTI_RIG",
            "TASK_DESC_MULTI_RIG",
            2.0, 0.0, 1200.0, 0.0, false, false
        },
        {
            "TASK_MINE_ETC",
            "TASK_TITLE_MINE_ETC",
            "TASK_DESC_MINE_ETC",
            1.0, 0.0, 600.0, 0.0, false, false
        },
        {
            "TASK_DIVERSIFY",
            "TASK_TITLE_DIVERSIFY",
            "TASK_DESC_DIVERSIFY",
            3.0, 0.0, 1000.0, 0.0, false, false
        },
        {
            "TASK_HASHRATE",
            "TASK_TITLE_HASHRATE",
            "TASK_DESC_HASHRATE",
            150.0, 0.0, 800.0, 0.0, false, false
        },

        // --- SEVİYE 3: ALTYAPI & ENDÜSTRİYEL GÜÇ ---
        {
            "TASK_COOLING",
            "TASK_TITLE_COOLING",
            "TASK_DESC_COOLING",
            3000.0, 0.0, 800.0, 0.0, false, false
        },
        {
            "TASK_POWER",
            "TASK_TITLE_POWER",
            "TASK_DESC_POWER",
            7500.0, 0.0, 1000.0, 0.0, false, false
        },
        {
            "TASK_SOLAR",
            "TASK_TITLE_SOLAR",
            "TASK_DESC_SOLAR",
            1000.0, 0.0, 1500.0, 0.0, false, false
        },
        {
            "TASK_MINE_ETHW",
            "TASK_TITLE_MINE_ETHW",
            "TASK_DESC_MINE_ETHW",
            1.0, 0.0, 2500.0, 0.0, false, false
        },
        {
            "TASK_PORTFOLIO_10K",
            "TASK_TITLE_PORTFOLIO_10K",
            "TASK_DESC_PORTFOLIO_10K",
            10000.0, 0.0, 2000.0, 0.0, false, false
        },
        {
            "TASK_MAX_FACILITY",
            "TASK_TITLE_MAX_FACILITY",
            "TASK_DESC_MAX_FACILITY",
            2.0, 0.0, 3500.0, 0.0, false, false
        },
        {
            "TASK_ASIC_KING",
            "TASK_TITLE_ASIC_KING",
            "TASK_DESC_ASIC_KING",
            500.0, 0.0, 5000.0, 0.0, false, false
        }
    };
}

void TaskManager::NotifyCardInspected() {
    m_cardInspected = true;
}

void TaskManager::NotifyCardOverclocked() {
    m_cardOverclocked = true;
}

void TaskManager::NotifyCryptoSold(double usdAmount) {
    m_totalCryptoSoldUsd += usdAmount;
}

void TaskManager::NotifyGpuPurchased() {
    m_gpusPurchasedCount++;
}

void TaskManager::NotifyTradeExecuted() {
    m_tradesExecutedCount++;
}

void TaskManager::NotifyNewsRead() {
    m_newsReadCount++;
}

void TaskManager::UpdateProgress(const Warehouse& warehouse,
                                const EconomyManager& economy,
                                const CoolingManager& cooling,
                                const PowerGrid& powerGrid,
                                const FacilityManager* facilityManager) {
    for (auto& task : m_tasks) {
        if (task.isCompleted) continue;

        if (task.id == "TASK_INSPECT") {
            task.currentProgress = m_cardInspected ? 1.0 : 0.0;
        } else if (task.id == "TASK_OVERCLOCK") {
            task.currentProgress = m_cardOverclocked ? 1.0 : 0.0;
        } else if (task.id == "TASK_BUY_GPU") {
            task.currentProgress = static_cast<double>(m_gpusPurchasedCount);
        } else if (task.id == "TASK_FIRST_TRADE") {
            task.currentProgress = static_cast<double>(m_tradesExecutedCount);
        } else if (task.id == "TASK_READ_NEWS") {
            task.currentProgress = static_cast<double>(m_newsReadCount);
        } else if (task.id == "TASK_HASHRATE_50") {
            task.currentProgress = warehouse.CalculateTotalHashrate();
        } else if (task.id == "TASK_MINE_TEX_100") {
            const auto* texCoin = economy.GetCoinById("TEX");
            task.currentProgress = texCoin ? texCoin->balance : 0.0;
        } else if (task.id == "TASK_MINE_RVN_1000") {
            const auto* rvnCoin = economy.GetCoinById("RVN");
            task.currentProgress = rvnCoin ? rvnCoin->balance : 0.0;
        } else if (task.id == "TASK_SELL_CRYPTO") {
            task.currentProgress = m_totalCryptoSoldUsd;
        } else if (task.id == "TASK_MULTI_RIG") {
            task.currentProgress = static_cast<double>(warehouse.GetRigCount());
        } else if (task.id == "TASK_MINE_ETC") {
            const auto* etcCoin = economy.GetCoinById("ETC");
            task.currentProgress = etcCoin ? etcCoin->balance : 0.0;
        } else if (task.id == "TASK_DIVERSIFY") {
            double heldCount = 0.0;
            for (const auto& c : economy.GetCoins()) {
                if (c.balance > 0.0001) heldCount += 1.0;
            }
            task.currentProgress = heldCount;
        } else if (task.id == "TASK_HASHRATE") {
            task.currentProgress = warehouse.CalculateTotalHashrate();
        } else if (task.id == "TASK_COOLING") {
            task.currentProgress = cooling.CalculateTotalCoolingWatts();
        } else if (task.id == "TASK_POWER") {
            task.currentProgress = powerGrid.GetMaxCapacityWatts();
        } else if (task.id == "TASK_SOLAR") {
            task.currentProgress = powerGrid.GetTotalProductionWatts();
        } else if (task.id == "TASK_MINE_ETHW") {
            const auto* ethwCoin = economy.GetCoinById("ETHW");
            task.currentProgress = ethwCoin ? ethwCoin->balance : 0.0;
        } else if (task.id == "TASK_PORTFOLIO_10K") {
            task.currentProgress = economy.GetTotalPortfolioValueUSD();
        } else if (task.id == "TASK_MAX_FACILITY") {
            if (facilityManager) {
                double purchasedCount = 0.0;
                for (const auto& fac : facilityManager->GetAllFacilities()) {
                    if (fac.isPurchased) purchasedCount += 1.0;
                }
                task.currentProgress = purchasedCount;
            }
        } else if (task.id == "TASK_ASIC_KING") {
            task.currentProgress = warehouse.CalculateTotalHashrate();
        }

        if (task.currentProgress >= task.targetProgress) {
            task.isCompleted = true;
        }
    }
}

const std::vector<GameTask>& TaskManager::GetTasks() const {
    return m_tasks;
}

size_t TaskManager::GetUnclaimedCompletedCount() const {
    size_t count = 0;
    for (const auto& task : m_tasks) {
        if (task.isCompleted && !task.isClaimed) {
            count++;
        }
    }
    return count;
}

bool TaskManager::ClaimReward(const std::string& taskId, EconomyManager& economy) {
    for (auto& task : m_tasks) {
        if (task.id == taskId) {
            if (task.isCompleted && !task.isClaimed) {
                task.isClaimed = true;
                if (task.rewardCash > 0.0) {
                    economy.AddFiat(task.rewardCash);
                }
                return true;
            }
            return false;
        }
    }
    return false;
}

void TaskManager::SetTaskState(const std::string& taskId, bool completed, bool claimed, double progress) {
    for (auto& task : m_tasks) {
        if (task.id == taskId) {
            task.isCompleted = completed;
            task.isClaimed = claimed;
            task.currentProgress = progress;
            return;
        }
    }
}

} // namespace Core
