#include "PowerGrid.hpp"
#include <algorithm>

namespace Core {

PowerGrid::PowerGrid(double maxBreakerCapacityWatts, double gridPricePerKwh)
    : m_maxCapacityWatts(maxBreakerCapacityWatts)
    , m_gridPricePerKwh(gridPricePerKwh)
    , m_currentConsumerWatts(0.0)
    , m_currentProducerWatts(0.0)
    , m_breakerTripped(false)
{
}

double PowerGrid::GetMaxCapacityWatts() const {
    return m_maxCapacityWatts;
}

void PowerGrid::SetMaxCapacityWatts(double capacityWatts) {
    m_maxCapacityWatts = std::max(0.0, capacityWatts);
}

double PowerGrid::GetPricePerKwh() const {
    return m_gridPricePerKwh;
}

void PowerGrid::SetPricePerKwh(double price) {
    m_gridPricePerKwh = std::max(0.0, price);
}

void PowerGrid::AddConsumerWatts(double watts) {
    m_currentConsumerWatts += std::max(0.0, watts);
    if (m_currentConsumerWatts > m_maxCapacityWatts) {
        m_breakerTripped = true;
    }
}

void PowerGrid::AddProducerWatts(double watts) {
    m_currentProducerWatts += std::max(0.0, watts);
}

void PowerGrid::ResetStep() {
    m_currentConsumerWatts = 0.0;
    m_currentProducerWatts = 0.0;
}

double PowerGrid::GetTotalConsumptionWatts() const {
    return m_currentConsumerWatts;
}

double PowerGrid::GetTotalProductionWatts() const {
    return m_currentProducerWatts;
}

double PowerGrid::GetNetGridDrawWatts() const {
    // Şebekeden çekilen net güç = Tüketim - Yeşil Üretim
    return m_currentConsumerWatts - m_currentProducerWatts;
}

double PowerGrid::CalculateCostForDuration(double durationSeconds) const {
    // Net Watt -> Kilowatt saat (kWh) dönüşümü:
    // (NetWatts / 1000.0) * (durationSeconds / 3600.0)
    const double netWatts = GetNetGridDrawWatts();
    const double hours = durationSeconds / 3600.0;
    const double kwh = (netWatts / 1000.0) * hours;
    return kwh * m_gridPricePerKwh;
}

bool PowerGrid::IsBreakerTripped() const {
    return m_breakerTripped;
}

void PowerGrid::ResetBreaker() {
    m_breakerTripped = false;
}

} // namespace Core
