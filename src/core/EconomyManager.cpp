#include "EconomyManager.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <iomanip>
#include <sstream>

namespace Core {

EconomyManager::EconomyManager(double initialFiat, double initialCryptoPrice, std::string coinSymbol)
    : m_fiatBalance(initialFiat)
    , m_cryptoBalance(0.0)
    , m_coinSymbol(std::move(coinSymbol))
    , m_cryptoPrice(initialCryptoPrice)
    , m_baseDifficulty(350000.0) // 350k MH per coin base rate
    , m_networkDifficulty(350000.0)
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

void EconomyManager::SetCurrency(CurrencyType curr) {
    m_currency = curr;
}

CurrencyType EconomyManager::GetCurrency() const {
    return m_currency;
}

void EconomyManager::NextCurrency() {
    switch (m_currency) {
        case CurrencyType::USD: m_currency = CurrencyType::USDT; break;
        case CurrencyType::USDT: m_currency = CurrencyType::TRY; break;
        case CurrencyType::TRY: m_currency = CurrencyType::EUR; break;
        case CurrencyType::EUR: m_currency = CurrencyType::USD; break;
    }
}

std::string EconomyManager::GetCurrencyCode() const {
    switch (m_currency) {
        case CurrencyType::USD: return "USD";
        case CurrencyType::USDT: return "USDT";
        case CurrencyType::TRY: return "TRY";
        case CurrencyType::EUR: return "EUR";
    }
    return "USD";
}

std::string EconomyManager::GetCurrencySymbol() const {
    switch (m_currency) {
        case CurrencyType::USD: return "$";
        case CurrencyType::USDT: return "₮";
        case CurrencyType::TRY: return "₺";
        case CurrencyType::EUR: return "€";
    }
    return "$";
}

double EconomyManager::GetExchangeRate() const {
    switch (m_currency) {
        case CurrencyType::USD: return 1.0;
        case CurrencyType::USDT: return 1.0;
        case CurrencyType::TRY: return 34.0;
        case CurrencyType::EUR: return 0.92;
    }
    return 1.0;
}

std::string EconomyManager::FormatFiat(double usdAmount) const {
    double converted = usdAmount * GetExchangeRate();
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    switch (m_currency) {
        case CurrencyType::USD:
            ss << "$" << converted;
            break;
        case CurrencyType::USDT:
            ss << converted << " USDT";
            break;
        case CurrencyType::TRY:
            ss << converted << " ₺";
            break;
        case CurrencyType::EUR:
            ss << converted << " €";
            break;
    }
    return ss.str();
}

std::string EconomyManager::FormatPrice(double usdPrice) const {
    double converted = usdPrice * GetExchangeRate();
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(0);
    switch (m_currency) {
        case CurrencyType::USD:
            ss << "$" << converted;
            break;
        case CurrencyType::USDT:
            ss << converted << " USDT";
            break;
        case CurrencyType::TRY:
            ss << converted << " ₺";
            break;
        case CurrencyType::EUR:
            ss << converted << " €";
            break;
    }
    return ss.str();
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

    // Gerçekçi Kripto Dinamik Ağ Zorluğu (Mining Difficulty Adjustment):
    // Oyuncunun kazım gücü büyüdükçe küresel ağ zorluğu da dinamik adapte olur.
    m_networkDifficulty = m_baseDifficulty + (hashrateMHS * 180.0);

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
