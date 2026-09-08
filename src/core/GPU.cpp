#include "GPU.hpp"
#include <algorithm>

namespace Core {

GPU::GPU(const std::string& name, double baseHashrate, double basePowerWatts, double siliconQuality)
    : m_name(name)
    , m_baseHashrate(baseHashrate)
    , m_basePowerWatts(basePowerWatts)
    , m_siliconQuality(std::clamp(siliconQuality, 0.85, 1.25))
    , m_overclockMultiplier(1.0)
    , m_coreClockOffsetMHz(0.0)
    , m_powerLimitPercent(100.0)
    , m_fanSpeedPercent(80.0)
    , m_healthPercent(100.0)
    , m_isBurnt(false)
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
    m_overclockMultiplier = std::clamp(multiplier, 0.5, 1.60);
}

double GPU::GetCoreClockOffset() const {
    return m_coreClockOffsetMHz;
}

void GPU::SetCoreClockOffset(double offsetMHz) {
    m_coreClockOffsetMHz = std::clamp(offsetMHz, -300.0, 500.0);
}

double GPU::GetPowerLimitPercent() const {
    return m_powerLimitPercent;
}

void GPU::SetPowerLimitPercent(double percent) {
    m_powerLimitPercent = std::clamp(percent, 60.0, 160.0);
}

double GPU::GetFanSpeedPercent() const {
    return m_fanSpeedPercent;
}

void GPU::SetFanSpeedPercent(double percent) {
    m_fanSpeedPercent = std::clamp(percent, 20.0, 100.0);
}

double GPU::GetHealthPercent() const {
    return m_healthPercent;
}

void GPU::SetHealthPercent(double health) {
    m_healthPercent = std::clamp(health, 0.0, 100.0);
}

void GPU::SetSiliconQuality(double quality) {
    m_siliconQuality = std::clamp(quality, 0.5, 2.0);
}

void GPU::TakeDamage(double damage) {
    if (m_isBurnt) return;

    m_healthPercent = std::max(0.0, m_healthPercent - damage);
    if (m_healthPercent <= 0.0) {
        m_isBurnt = true;
    }
}

void GPU::Repair() {
    m_healthPercent = 100.0;
    m_isBurnt = false;
    m_isThrottled = false;
}

bool GPU::IsBurnt() const {
    return m_isBurnt;
}

void GPU::SetBurnt(bool burnt) {
    m_isBurnt = burnt;
    if (burnt) {
        m_healthPercent = 0.0;
    }
}

bool GPU::IsThrottled() const {
    return m_isThrottled;
}

void GPU::SetThrottled(bool throttled) {
    m_isThrottled = throttled;
}

double GPU::GetEffectiveHashrate() const {
    // Yanmış veya iflas etmiş kart kazamaz!
    if (m_isBurnt || m_healthPercent <= 0.0) {
        return 0.0;
    }

    const double throttlePenalty = m_isThrottled ? 0.45 : 1.0;
    const double clockFactor = 1.0 + (m_coreClockOffsetMHz / 1500.0); // 1500MHz baz saat üzerinden ölçekleme
    const double healthFactor = 0.5 + (m_healthPercent / 200.0); // Hasarlı kart daha az kazar

    return m_baseHashrate * m_overclockMultiplier * clockFactor * m_siliconQuality * throttlePenalty * healthFactor;
}

double GPU::GetEffectivePowerWatts() const {
    // Yanmış kart sadece kısa devre/arıza tüketimi çeker
    if (m_isBurnt) {
        return 10.0;
    }

    const double powerTarget = m_powerLimitPercent / 100.0;
    const double powerScaling = m_overclockMultiplier * m_overclockMultiplier;
    const double efficiencyBonus = 1.0 / m_siliconQuality;

    return m_basePowerWatts * powerScaling * powerTarget * efficiencyBonus;
}

} // namespace Core
