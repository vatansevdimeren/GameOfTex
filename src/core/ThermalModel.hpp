#pragma once

namespace Core {

/**
 * @class ThermalModel
 * @brief Simulates thermodynamic heat generation and cooling dynamics.
 * 
 * SRP: This class is solely responsible for room/ambient temperature
 * differential equations, heat dissipation, and GPU core temperatures.
 */
class ThermalModel {
public:
    /**
     * @brief Constructs thermal model with base ambient temperature.
     * @param baseAmbientTempCelsius Ambient room temperature (e.g. 21.0°C).
     * @param thermalResistance GPU heatsink thermal resistance in °C/Watt (default 0.18).
     */
    explicit ThermalModel(double baseAmbientTempCelsius = 21.0, double thermalResistance = 0.18);

    // Getters and Setters
    [[nodiscard]] double GetAmbientTemperature() const;
    void SetBaseAmbientTemperature(double baseTemp);

    [[nodiscard]] double GetCoolingPowerWatts() const;
    void SetCoolingPowerWatts(double coolingWatts);

    /**
     * @brief Updates ambient room temperature based on total dissipated heat vs active cooling.
     * @param totalHeatGeneratedWatts Total Watts drawn by all rigs and systems.
     * @param deltaTimeSeconds Elapsed simulation time in seconds.
     */
    void Update(double totalHeatGeneratedWatts, double deltaTimeSeconds);

    /**
     * @brief Calculates a GPU core temperature given its current power draw and fan cooling.
     * @param gpuPowerWatts Power drawn by the GPU.
     * @param fanSpeedPercent Fan speed ratio from 0.0 to 1.0 (higher = better cooling).
     * @return Core temperature in degrees Celsius.
     */
    [[nodiscard]] double CalculateGPUTemperature(double gpuPowerWatts, double fanSpeedPercent = 0.80) const;

    /**
     * @brief Checks if a given temperature has crossed the safe threshold.
     * @param tempCelsius Temperature to test.
     * @return True if above 85°C (thermal throttling limit).
     */
    [[nodiscard]] static bool IsOverheating(double tempCelsius);

private:
    double m_baseAmbientTempCelsius;  // External weather / base room temperature
    double m_currentAmbientCelsius;   // Current room temperature after heat build-up
    double m_coolingPowerWatts;       // Total active heat extraction capability
    double m_thermalResistance;       // Heat transfer factor (°C per Watt)
};

} // namespace Core
