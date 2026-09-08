#include "ThermalModel.hpp"
#include <algorithm>

namespace Core {

ThermalModel::ThermalModel(double baseAmbientTempCelsius, double thermalResistance)
    : m_baseAmbientTempCelsius(baseAmbientTempCelsius)
    , m_currentAmbientCelsius(baseAmbientTempCelsius)
    , m_coolingPowerWatts(0.0)
    , m_thermalResistance(thermalResistance)
{
}

double ThermalModel::GetAmbientTemperature() const {
    return m_currentAmbientCelsius;
}

void ThermalModel::SetBaseAmbientTemperature(double baseTemp) {
    m_baseAmbientTempCelsius = baseTemp;
}

double ThermalModel::GetCoolingPowerWatts() const {
    return m_coolingPowerWatts;
}

void ThermalModel::SetCoolingPowerWatts(double coolingWatts) {
    m_coolingPowerWatts = std::max(0.0, coolingWatts);
}

void ThermalModel::Update(double totalHeatGeneratedWatts, double deltaTimeSeconds) {
    // Net termal birikim: Üretilen ısı - Tahliye edilen soğutma gücü
    const double netHeatWatts = totalHeatGeneratedWatts - m_coolingPowerWatts;

    // Oda hava kütlesi termal ataleti (örnek: küçük oda için 1200 Joule/°C)
    constexpr double roomThermalCapacity = 15000.0; // Joules per degree C

    // Sıcaklık değişimi: deltaT = (NetWatts * dt) / Capacity
    const double deltaT = (netHeatWatts * deltaTimeSeconds) / roomThermalCapacity;
    m_currentAmbientCelsius += deltaT;

    // Oda sıcaklığı dış ortam baz sıcaklığının altına inemez (termodinamik denge)
    if (m_currentAmbientCelsius < m_baseAmbientTempCelsius) {
        m_currentAmbientCelsius = m_baseAmbientTempCelsius;
    }
}

double ThermalModel::CalculateGPUTemperature(double gpuPowerWatts, double fanSpeedPercent) const {
    const double clampedFan = std::clamp(fanSpeedPercent, 0.1, 1.0);
    // Fan devri arttıkça termal direnç düşer (daha iyi soğutur)
    const double effectiveResistance = m_thermalResistance / clampedFan;
    
    // Çekirdek Sıcaklığı = Ortam Sıcaklığı + (Güç * Etkin Direnç)
    return m_currentAmbientCelsius + (gpuPowerWatts * effectiveResistance);
}

bool ThermalModel::IsOverheating(double tempCelsius) {
    constexpr double throttlingThreshold = 85.0; // 85°C sonrası kart throttle yer
    return tempCelsius >= throttlingThreshold;
}

} // namespace Core
