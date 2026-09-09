#include "Warehouse.hpp"
#include <algorithm>

namespace Core {

Warehouse::Warehouse(std::string facilityName)
    : m_facilityName(std::move(facilityName))
    , m_activeRigIndex(0)
{
    // Varsayılan olarak 1 adet başlangıç rig'i ile başla (Level 1: Ahşap Garaj Kasası, 2 Slot, CPU Madencisi)
    AddNewRig("Rig 01 - Ahsap Garaj Kasasi", 2);
}

const std::string& Warehouse::GetFacilityName() const {
    return m_facilityName;
}

void Warehouse::SetFacilityName(std::string name) {
    m_facilityName = std::move(name);
}

size_t Warehouse::GetRigCount() const {
    return m_rigs.size();
}

size_t Warehouse::GetActiveRigIndex() const {
    return m_activeRigIndex;
}

void Warehouse::SetActiveRigIndex(size_t index) {
    if (index < m_rigs.size()) {
        m_activeRigIndex = index;
    }
}

void Warehouse::NextRig() {
    if (!m_rigs.empty()) {
        m_activeRigIndex = (m_activeRigIndex + 1) % m_rigs.size();
    }
}

void Warehouse::PreviousRig() {
    if (!m_rigs.empty()) {
        m_activeRigIndex = (m_activeRigIndex + m_rigs.size() - 1) % m_rigs.size();
    }
}

MiningRig* Warehouse::GetActiveRig() {
    if (m_activeRigIndex < m_rigs.size()) {
        return m_rigs[m_activeRigIndex].get();
    }
    return nullptr;
}

const MiningRig* Warehouse::GetActiveRig() const {
    if (m_activeRigIndex < m_rigs.size()) {
        return m_rigs[m_activeRigIndex].get();
    }
    return nullptr;
}

MiningRig* Warehouse::GetRig(size_t index) {
    if (index < m_rigs.size()) {
        return m_rigs[index].get();
    }
    return nullptr;
}

const MiningRig* Warehouse::GetRig(size_t index) const {
    if (index < m_rigs.size()) {
        return m_rigs[index].get();
    }
    return nullptr;
}

bool Warehouse::AddNewRig(const std::string& rigName, size_t gpuCapacity) {
    if (m_rigs.size() >= m_maxRigCapacity) {
        return false;
    }
    m_rigs.push_back(std::make_unique<MiningRig>(rigName, gpuCapacity));
    return true;
}

void Warehouse::AddRig(std::unique_ptr<MiningRig> rig) {
    if (rig) {
        m_rigs.push_back(std::move(rig));
    }
}

void Warehouse::ClearRigs() {
    m_rigs.clear();
    m_activeRigIndex = 0;
}

size_t Warehouse::GetMaxRigCapacity() const {
    return m_maxRigCapacity;
}

void Warehouse::SetMaxRigCapacity(size_t cap) {
    m_maxRigCapacity = cap;
}

bool Warehouse::RemoveRig(size_t index) {
    if (index >= m_rigs.size() || m_rigs.size() <= 1) {
        return false;
    }
    m_rigs.erase(m_rigs.begin() + index);
    if (m_activeRigIndex >= m_rigs.size()) {
        m_activeRigIndex = m_rigs.size() - 1;
    }
    return true;
}

double Warehouse::CalculateTotalHashrate() const {
    double total = 0.0;
    for (const auto& rig : m_rigs) {
        if (rig) {
            total += rig->CalculateTotalHashrate();
        }
    }
    return total;
}

double Warehouse::CalculateTotalCPUHashrateKH() const {
    double total = 0.0;
    for (const auto& rig : m_rigs) {
        if (rig && rig->IsPoweredOn()) {
            total += rig->CalculateTotalCPUHashrateKH();
        }
    }
    return total;
}

double Warehouse::CalculateTotalPowerWatts() const {
    double total = 0.0;
    for (const auto& rig : m_rigs) {
        if (rig) {
            total += rig->CalculateTotalPowerWatts();
        }
    }
    return total;
}

const std::vector<std::unique_ptr<MiningRig>>& Warehouse::GetAllRigs() const {
    return m_rigs;
}

} // namespace Core
