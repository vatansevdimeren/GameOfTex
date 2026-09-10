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
    const double netWatts = std::max(0.0, GetNetGridDrawWatts());
    const double hours = durationSeconds / 3600.0;
    const double kwh = (netWatts / 1000.0) * hours;
    // Ticari Sanayi Şebeke Altyapı, Trafo Kapasitesi & PUE Dağıtım Çarpanı:
    // Madencilik gelirinin ~%12-18'i bandında dengelenir; oyuncuyu asla boğmaz,
    // ancak lokasyon seçimi, yeşil enerji ve ekonomist hedging indirimlerini gerçek anlamda değerli kılar.
    constexpr double COMMERCIAL_FACILITY_SCALE = 65.0;
    return kwh * m_gridPricePerKwh * COMMERCIAL_FACILITY_SCALE;
}

bool PowerGrid::IsBreakerTripped() const {
    return m_breakerTripped;
}

void PowerGrid::ResetBreaker() {
    m_breakerTripped = false;
}

void PowerGrid::SetBreakerTripped(bool tripped) {
    m_breakerTripped = tripped;
}

void PowerGrid::Update(double dt) {
    (void)dt;
    // Suni rastgele spike kaldırıldı; şartel yalnızca gerçek tüketim kapasiteyi aşınca atar
}

bool PowerGrid::IsNetworkSpikeActive() const {
    return false;
}

double PowerGrid::GetNetworkSpikeRemainingSeconds() const {
    return 0.0;
}

double PowerGrid::GetPowerSurgeMultiplier() const {
    return 1.0; // Stabil ve adil tüketim
}

double PowerGrid::GetHashrateSurgeMultiplier() const {
    return 1.0;
}

bool PowerGrid::IsGridStrained() const {
    return (m_currentConsumerWatts >= m_maxCapacityWatts * 0.90);
}

} // namespace Core
