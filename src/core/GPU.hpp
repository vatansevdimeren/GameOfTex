#pragma once

#include <string>

namespace Core {

/**
 * @class GPU
 * @brief Represents a single physical graphics processing unit.
 * 
 * SRP: This class is solely responsible for holding hardware specifications,
 * tracking overclock parameters, and computing power/hashrate values based
 * on silicon quality. It does NOT compute temperatures or money.
 */
class GPU {
public:
    /**
     * @brief Constructs a new GPU with hardware specs and silicon lottery quality.
     * @param name Name of the GPU model (e.g. "RTX 3080").
     * @param baseHashrate Factory standard hashrate in MH/s.
     * @param basePowerWatts Factory standard power draw in Watts.
     * @param siliconQuality Silicon lottery multiplier (typically 0.90 to 1.15).
     */
    GPU(const std::string& name, double baseHashrate, double basePowerWatts, double siliconQuality = 1.0);

    // Getters for immutable hardware specs
    [[nodiscard]] const std::string& GetName() const;
    [[nodiscard]] double GetBaseHashrate() const;
    [[nodiscard]] double GetBasePowerWatts() const;
    [[nodiscard]] double GetSiliconQuality() const;

    // Overclocking parameters
    [[nodiscard]] double GetOverclockMultiplier() const;
    void SetOverclockMultiplier(double multiplier);

    // Performance calculations
    [[nodiscard]] double GetEffectiveHashrate() const;
    [[nodiscard]] double GetEffectivePowerWatts() const;

    // Thermal throttle state
    [[nodiscard]] bool IsThrottled() const;
    void SetThrottled(bool throttled);

private:
    std::string m_name;
    double m_baseHashrate;       // Base hashrate in MH/s
    double m_basePowerWatts;      // Base power draw in Watts
    double m_siliconQuality;      // Silicon quality multiplier
    double m_overclockMultiplier; // Multiplier (1.0 = stock, 1.2 = 20% OC)
    bool m_isThrottled;           // Flagged true when overheating
};

} // namespace Core
