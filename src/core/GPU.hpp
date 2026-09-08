#pragma once

#include <string>

namespace Core {

/**
 * @class GPU
 * @brief Represents a single physical graphics processing unit with overclocking,
 * thermal health tracking, and hardware destruction (burn) mechanics.
 * 
 * SRP: This class is solely responsible for holding hardware specifications,
 * tracking clock/voltage/fan parameters, health, and burnt status.
 */
class GPU {
public:
    /**
     * @brief Constructs a new GPU with hardware specs and silicon lottery quality.
     */
    GPU(const std::string& name, double baseHashrate, double basePowerWatts, double siliconQuality = 1.0);

    // Hardware specifications
    [[nodiscard]] const std::string& GetName() const;
    [[nodiscard]] double GetBaseHashrate() const;
    [[nodiscard]] double GetBasePowerWatts() const;
    [[nodiscard]] double GetSiliconQuality() const;

    // Overclocking parameters
    [[nodiscard]] double GetOverclockMultiplier() const;
    void SetOverclockMultiplier(double multiplier);

    [[nodiscard]] double GetCoreClockOffset() const;
    void SetCoreClockOffset(double offsetMHz);

    [[nodiscard]] double GetPowerLimitPercent() const;
    void SetPowerLimitPercent(double percent);

    [[nodiscard]] double GetFanSpeedPercent() const;
    void SetFanSpeedPercent(double percent);

    // Health and Destruction (Burn) mechanics
    [[nodiscard]] double GetHealthPercent() const;
    void SetHealthPercent(double health);
    void SetSiliconQuality(double quality);
    void TakeDamage(double damage);
    void Repair();

    [[nodiscard]] bool IsBurnt() const;
    void SetBurnt(bool burnt);

    [[nodiscard]] bool IsThrottled() const;
    void SetThrottled(bool throttled);

    // Performance calculations
    [[nodiscard]] double GetEffectiveHashrate() const;
    [[nodiscard]] double GetEffectivePowerWatts() const;

private:
    std::string m_name;
    double m_baseHashrate;          // Factory standard MH/s
    double m_basePowerWatts;         // Factory standard Watts
    double m_siliconQuality;         // Silicon lottery factor (0.85 - 1.25)
    double m_overclockMultiplier;    // Simple multiplier

    double m_coreClockOffsetMHz;     // Core clock offset (-200 to +400 MHz)
    double m_powerLimitPercent;      // Power target (70% to 150%)
    double m_fanSpeedPercent;        // Fan duty cycle (30% to 100%)

    double m_healthPercent;          // 0.0% to 100.0%
    bool m_isBurnt;                  // True if burnt from exceeding 140°C
    bool m_isThrottled;              // True if thermal throttling
};

} // namespace Core
