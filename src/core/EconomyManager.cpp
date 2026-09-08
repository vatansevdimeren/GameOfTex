#include "EconomyManager.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace Core {

EconomyManager::EconomyManager(double initialFiat, double initialCryptoPrice, std::string coinSymbol)
    : m_fiatBalance(initialFiat)
    , m_cryptoBalance(0.0)
    , m_coinSymbol(std::move(coinSymbol))
    , m_cryptoPrice(initialCryptoPrice)
    , m_networkDifficulty(10000.0) // 10k MH per coin base rate
    , m_marketTimer(0.0)
{
}

double EconomyManager::GetFiatBalance() const {
    return m_fiatBalance;
}

double EconomyManager::GetCryptoBalance() const {
    return m_cryptoBalance;
}

const std::string& EconomyManager::GetCoinSymbol() const {
    return m_coinSymbol;
}

void EconomyManager::AddFiat(double amount) {
    if (amount > 0.0) {
        m_fiatBalance += amount;
    }
}

bool EconomyManager::DeductFiat(double amount) {
    if (amount <= 0.0 || m_fiatBalance < amount) {
        return false;
    }
    m_fiatBalance -= amount;
    return true;
}

double EconomyManager::GetCryptoPrice() const {
    return m_cryptoPrice;
}

double EconomyManager::GetNetworkDifficulty() const {
    return m_networkDifficulty;
}

void EconomyManager::UpdateMarket(double deltaTimeSeconds) {
    m_marketTimer += deltaTimeSeconds;

    // Piyasa fiyatını her saniye ufak bir rastgele yürüyüş (random walk / volatilite) ile sars
    if (m_marketTimer >= 1.0) {
        m_marketTimer = 0.0;

        static std::mt19937 rng(42); // Deterministik/öngörülebilir tohum
        static std::normal_distribution<double> dist(0.0, 0.005); // %0.5 ortalama sapma

        const double percentageChange = dist(rng);
        m_cryptoPrice *= (1.0 + percentageChange);

        // Fiyat 100 doların altına düşmesin
        m_cryptoPrice = std::max(100.0, m_cryptoPrice);
    }
}

double EconomyManager::MineCoins(double hashrateMHS, double deltaTimeSeconds) {
    if (hashrateMHS <= 0.0 || deltaTimeSeconds <= 0.0) {
        return 0.0;
    }

    // Kazılan coin formülü: (Hashrate * saniye) / Ağ Zorluğu
    const double mintedCoins = (hashrateMHS * deltaTimeSeconds) / m_networkDifficulty;
    m_cryptoBalance += mintedCoins;
    return mintedCoins;
}

bool EconomyManager::SellCrypto(double amount) {
    if (amount <= 0.0 || m_cryptoBalance < amount) {
        return false;
    }

    m_cryptoBalance -= amount;
    const double revenue = amount * m_cryptoPrice;
    m_fiatBalance += revenue;
    return true;
}

} // namespace Core
