#pragma once

namespace Core {

/**
 * @class PowerGrid
 * @brief Manages electricity consumption, green power production, and utility bills.
 * 
 * SRP: This class is solely responsible for balancing electrical load,
 * checking circuit breaker limits, and computing power costs/revenue.
 */
class PowerGrid {
public:
    /**
     * @brief Constructs a power grid manager.
     * @param maxBreakerCapacityWatts Maximum wattage before the circuit breaker trips (e.g. 3600W for a home circuit).
     * @param gridPricePerKwh Cost of electricity from the city grid in $ per kWh (e.g. 0.12).
     */
    PowerGrid(double maxBreakerCapacityWatts, double gridPricePerKwh = 0.12);

    // Grid configuration
    [[nodiscard]] double GetMaxCapacityWatts() const;
    void SetMaxCapacityWatts(double capacityWatts);

    [[nodiscard]] double GetPricePerKwh() const;
    void SetPricePerKwh(double price);

    // Real-time balance
    void AddConsumerWatts(double watts);
    void AddProducerWatts(double watts);
    void ResetStep();

    [[nodiscard]] double GetTotalConsumptionWatts() const;
    [[nodiscard]] double GetTotalProductionWatts() const;
    [[nodiscard]] double GetNetGridDrawWatts() const;

    /**
     * @brief Computes power cost for an elapsed duration.
     * @param durationSeconds Elapsed simulation time in seconds.
     * @return Cost in currency (positive = paying the utility, negative = selling power to grid).
     */
    [[nodiscard]] double CalculateCostForDuration(double durationSeconds) const;

    /**
     * @brief Checks if current consumption exceeded the breaker limit.
     */
    [[nodiscard]] bool IsBreakerTripped() const;

    /**
     * @brief Manually reset the breaker switch.
     */
    void ResetBreaker();
    void SetBreakerTripped(bool tripped);

    // Dynamic Network Spikes & Grid Events
    void Update(double dt);
    [[nodiscard]] bool IsNetworkSpikeActive() const;
    [[nodiscard]] double GetNetworkSpikeRemainingSeconds() const;
    [[nodiscard]] double GetPowerSurgeMultiplier() const;
    [[nodiscard]] double GetHashrateSurgeMultiplier() const;
    [[nodiscard]] bool IsGridStrained() const;

private:
    double m_maxCapacityWatts;
    double m_gridPricePerKwh;

    double m_currentConsumerWatts;
    double m_currentProducerWatts;
    bool m_breakerTripped;

    // Network Power Spike simulation
    double m_spikeCooldownTimer{35.0};
    double m_spikeDurationTimer{0.0};
    bool m_spikeActive{false};
};

} // namespace Core
