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

    // Endüstriyel HVAC ve aktif soğutma ortam sıcaklığını 14°C'ye kadar soğutabilir
    const double minAmbient = std::max(14.0, m_baseAmbientTempCelsius - (m_coolingPowerWatts / 600.0));
    if (m_currentAmbientCelsius < minAmbient) {
        m_currentAmbientCelsius = minAmbient;
    }
}

double ThermalModel::CalculateGPUTemperature(double gpuPowerWatts, double fanSpeedPercent) const {
    const double clampedFan = std::clamp(fanSpeedPercent, 0.1, 1.0);
    // Tesis geneli endüstriyel soğutma (HVAC, Duvar Fanları, Daldırma Sıvı)
    // Depodaki tüm ekran kartlarının termal direncini global olarak düşürür
    const double facilityCoolingBonus = 1.0 / (1.0 + (m_coolingPowerWatts / 2500.0));
    const double effectiveResistance = (m_thermalResistance / clampedFan) * facilityCoolingBonus;
    
    // Çekirdek Sıcaklığı = Ortam Sıcaklığı + (Güç * Etkin Direnç)
    return m_currentAmbientCelsius + (gpuPowerWatts * effectiveResistance);
}

bool ThermalModel::IsOverheating(double tempCelsius) {
    constexpr double throttlingThreshold = 85.0; // 85°C sonrası kart throttle yer
    return tempCelsius >= throttlingThreshold;
}

} // namespace Core
