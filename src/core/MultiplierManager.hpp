#pragma once

#include "raylib.h"
#include <string>
#include <vector>

namespace Core {
class MiningRig;
class Warehouse;
class ThermalModel;
class EconomyManager;
class MarketCatalog;
}

namespace Render {

enum class FrenzyType {
    NONE,
    TURBO_HASH,      // x5.0 Hashrate for 30s
    BULL_RALLY,      // +80% Coin Price Spike for 25s
    SUPERCONDUCTOR,  // Instant cool & 50% power for 40s
    AIRDROP          // Instant fiat windfall
};

struct SynergyInfo {
    bool isFullSet = false;
    bool isCryoCold = false;
    bool isEcoPower = false;
    double totalMultiplier = 1.0;
    std::string synergyLabel;
};

struct GoldenParticle {
    Vector2 pos;
    Vector2 vel;
    float alpha;
    float size;
    Color color;
};

/**
 * @class MultiplierManager
 * @brief Manages idle multiplier mechanics: Lucky Golden Nonce spawns, Turbo Frenzies, and Rig Synergies.
 */
class MultiplierManager {
public:
    MultiplierManager();

    /**
     * @brief Updates timers, spawns golden nonce, and handles frenzy countdowns.
     */
    void Update(double dt, Core::EconomyManager& economy, Core::Warehouse& warehouse,
                const Core::ThermalModel& thermalModel);

    /**
     * @brief Checks if mouse clicked the active golden nonce.
     * @return true if golden nonce was clicked and frenzy triggered.
     */
    bool CheckClick(Vector2 mousePos, Core::EconomyManager& economy, Core::Warehouse& warehouse,
                    const Core::MarketCatalog& catalog);

    /**
     * @brief Draws floating Golden Nonce if currently active, plus burst particles.
     */
    void Draw(float animTime) const;

    /**
     * @brief Draws the active Frenzy banner in the HUD if any frenzy is currently running.
     */
    void DrawHUDNotification(float screenW, float headerH) const;

    /**
     * @brief Calculates total synergy multiplier for a given rig.
     */
    [[nodiscard]] SynergyInfo CalculateRigSynergy(const Core::MiningRig& rig, const Core::ThermalModel& thermalModel) const;

    /**
     * @brief Returns global hashrate multiplier (e.g. 5.0 during Turbo Frenzy).
     */
    [[nodiscard]] double GetGlobalHashMultiplier() const;

    /**
     * @brief Returns crypto price multiplier (e.g. 1.8 during Bull Rally).
     */
    [[nodiscard]] double GetPriceMultiplier() const;

    /**
     * @brief Checks if superconductor pulse is active (drastically reduces power and temp).
     */
    [[nodiscard]] bool IsSuperconductorActive() const;

    [[nodiscard]] bool IsFrenzyActive() const;
    [[nodiscard]] FrenzyType GetActiveFrenzyType() const;
    [[nodiscard]] float GetFrenzyRemainingSeconds() const;
    [[nodiscard]] const std::string& GetFrenzyBannerText() const;

    /**
     * @brief Sets allowed spawning area for the golden nonce (usually within warehouse viewport).
     */
    void SetSpawnArea(Rectangle area);

private:
    void TriggerFrenzy(FrenzyType type, Core::EconomyManager& economy, Core::Warehouse& warehouse,
                       const Core::MarketCatalog& catalog);
    void SpawnGoldenNonce();
    void SpawnParticles(Vector2 center, int count);

    Rectangle m_spawnArea{100, 150, 600, 350};
    bool m_nonceActive = false;
    Vector2 m_noncePos{0, 0};
    float m_nonceRadius = 24.0f;
    float m_nonceTimer = 0.0f;
    float m_spawnCooldown = 55.0f; // Seconds between golden nonces

    FrenzyType m_activeFrenzy = FrenzyType::NONE;
    float m_frenzyRemaining = 0.0f;
    float m_frenzyDuration = 0.0f;
    std::string m_frenzyBanner;
    std::string m_frenzyToast;
    float m_toastTimer = 0.0f;

    mutable std::vector<GoldenParticle> m_particles;
};

} // namespace Render
