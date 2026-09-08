#include "GPU.hpp"
#include <algorithm>

namespace Core {

GPU::GPU(const std::string& name, double baseHashrate, double basePowerWatts, double siliconQuality)
    : m_name(name)
    , m_baseHashrate(baseHashrate)
    , m_basePowerWatts(basePowerWatts)
    , m_siliconQuality(std::clamp(siliconQuality, 0.85, 1.25))
    , m_overclockMultiplier(1.0)
    , m_isThrottled(false)
{
}

const std::string& GPU::GetName() const {
    return m_name;
}

double GPU::GetBaseHashrate() const {
    return m_baseHashrate;
}

double GPU::GetBasePowerWatts() const {
    return m_basePowerWatts;
}

double GPU::GetSiliconQuality() const {
    return m_siliconQuality;
}

double GPU::GetOverclockMultiplier() const {
    return m_overclockMultiplier;
}

void GPU::SetOverclockMultiplier(double multiplier) {
    // Güvenlik sınırları: 0.5 (undervolt/underclock) ile 1.50 (%50 ekstrem OC) arası
    m_overclockMultiplier = std::clamp(multiplier, 0.5, 1.50);
}

double GPU::GetEffectiveHashrate() const {
    // Throttled durumundaysa hashrate yarıya düşer
    const double throttlePenalty = m_isThrottled ? 0.5 : 1.0;
    return m_baseHashrate * m_overclockMultiplier * m_siliconQuality * throttlePenalty;
}

double GPU::GetEffectivePowerWatts() const {
    // Fizik kuralı: Güç tüketimi hız aşırtma/voltaj ile karesel artar (P ~ V^2 * f)
    const double powerScaling = m_overclockMultiplier * m_overclockMultiplier;
    // Kaliteli silikon daha az elektrik çeker (verimlilik avantajı)
    const double efficiencyBonus = 1.0 / m_siliconQuality;
    return m_basePowerWatts * powerScaling * efficiencyBonus;
}

bool GPU::IsThrottled() const {
    return m_isThrottled;
}

void GPU::SetThrottled(bool throttled) {
    m_isThrottled = throttled;
}

} // namespace Core
