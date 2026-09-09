#include "EconomyManager.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <iomanip>
#include <sstream>

namespace Core {

EconomyManager::EconomyManager(double initialFiat, double initialCryptoPrice, std::string coinSymbol)
    : m_fiatBalance(initialFiat)
    , m_currency(CurrencyType::USD)
    , m_marketTimer(0.0)
    , m_activeCoinIndex(0)
{
    InitCoins(initialCryptoPrice);
    if (!coinSymbol.empty()) {
        for (size_t i = 0; i < m_coins.size(); ++i) {
            if (m_coins[i].symbol == coinSymbol) {
                m_activeCoinIndex = i;
                break;
            }
        }
    }
}

void EconomyManager::InitCoins(double initialTexPrice) {
    m_coins.clear();

    auto generateHistory = [](double basePrice, double variance) {
        std::vector<float> hist;
        hist.reserve(60);
        std::mt19937 rng(1337 + static_cast<unsigned int>(basePrice * 10.0));
        std::normal_distribution<double> dist(0.0, variance);
        double curr = basePrice;
        for (int i = 0; i < 60; ++i) {
            curr += dist(rng);
            curr += (basePrice - curr) * 0.08; // mean-reversion
            if (curr < basePrice * 0.2) curr = basePrice * 0.2;
            hist.push_back(static_cast<float>(curr));
        }
        return hist;
    };

    // 1. TEX - TexCoin (Starter GPU coin)
    {
        CryptoCoin tex;
        tex.id = "TEX";
        tex.name = "TexCoin";
        tex.symbol = "TEX";
        tex.algorithm = "KawPow";
        tex.unit = "MH/s";
        tex.balance = 0.0;
        tex.basePriceUSD = initialTexPrice > 0.0 ? initialTexPrice : 2.40;
        tex.priceUSD = tex.basePriceUSD;
        tex.baseDifficulty = 120000.0;
        tex.difficulty = tex.baseDifficulty;
        tex.blockReward = 5.0;
        tex.high24h = tex.basePriceUSD * 1.08;
        tex.low24h = tex.basePriceUSD * 0.92;
        tex.volume24hUSD = 850000.0;
        tex.priceHistory = generateHistory(tex.basePriceUSD, 0.035);
        tex.requiredTier = 1;
        tex.isUnlocked = true;
        m_coins.push_back(std::move(tex));
    }

    // 2. RVN - Ravencoin
    {
        CryptoCoin rvn;
        rvn.id = "RVN";
        rvn.name = "Ravencoin";
        rvn.symbol = "RVN";
        rvn.algorithm = "X16R";
        rvn.unit = "MH/s";
        rvn.balance = 0.0;
        rvn.basePriceUSD = 0.085;
        rvn.priceUSD = rvn.basePriceUSD;
        rvn.baseDifficulty = 45000.0;
        rvn.difficulty = rvn.baseDifficulty;
        rvn.blockReward = 2500.0;
        rvn.high24h = 0.092;
        rvn.low24h = 0.078;
        rvn.volume24hUSD = 3450000.0;
        rvn.priceHistory = generateHistory(rvn.basePriceUSD, 0.002);
        rvn.requiredTier = 1;
        rvn.isUnlocked = true;
        m_coins.push_back(std::move(rvn));
    }

    // 3. ETC - Ethereum Classic
    {
        CryptoCoin etc;
        etc.id = "ETC";
        etc.name = "Ethereum Classic";
        etc.symbol = "ETC";
        etc.algorithm = "ETChash";
        etc.unit = "MH/s";
        etc.balance = 0.0;
        etc.basePriceUSD = 28.50;
        etc.priceUSD = etc.basePriceUSD;
        etc.baseDifficulty = 450000.0;
        etc.difficulty = etc.baseDifficulty;
        etc.blockReward = 2.56;
        etc.high24h = 30.40;
        etc.low24h = 26.80;
        etc.volume24hUSD = 18200000.0;
        etc.priceHistory = generateHistory(etc.basePriceUSD, 0.45);
        etc.requiredTier = 2;
        etc.isUnlocked = true;
        m_coins.push_back(std::move(etc));
    }

    // 4. ETHW - Ethereum PoW
    {
        CryptoCoin ethw;
        ethw.id = "ETHW";
        ethw.name = "Ethereum PoW";
        ethw.symbol = "ETHW";
        ethw.algorithm = "Ethash";
        ethw.unit = "MH/s";
        ethw.balance = 0.0;
        ethw.basePriceUSD = 145.00;
        ethw.priceUSD = ethw.basePriceUSD;
        ethw.baseDifficulty = 1200000.0;
        ethw.difficulty = ethw.baseDifficulty;
        ethw.blockReward = 2.0;
        ethw.high24h = 153.20;
        ethw.low24h = 138.50;
        ethw.volume24hUSD = 45000000.0;
        ethw.priceHistory = generateHistory(ethw.basePriceUSD, 1.80);
        ethw.requiredTier = 3;
        ethw.isUnlocked = true;
        m_coins.push_back(std::move(ethw));
    }

    // 5. BTC - Bitcoin (Industrial ASIC mining)
    {
        CryptoCoin btc;
        btc.id = "BTC";
        btc.name = "Bitcoin";
        btc.symbol = "BTC";
        btc.algorithm = "SHA-256";
        btc.unit = "GH/s";
        btc.balance = 0.0;
        btc.basePriceUSD = 64250.00;
        btc.priceUSD = btc.basePriceUSD;
        btc.baseDifficulty = 85000000.0;
        btc.difficulty = btc.baseDifficulty;
        btc.blockReward = 3.125;
        btc.high24h = 65800.00;
        btc.low24h = 63200.00;
        btc.volume24hUSD = 820000000.0;
        btc.priceHistory = generateHistory(btc.basePriceUSD, 320.0);
        btc.requiredTier = 4;
        btc.isUnlocked = true;
        m_coins.push_back(std::move(btc));
    }
}

double EconomyManager::GetFiatBalance() const {
    return m_fiatBalance;
}

double EconomyManager::GetCryptoBalance() const {
    const CryptoCoin* active = GetActiveCoin();
    return active ? active->balance : 0.0;
}

const std::string& EconomyManager::GetCoinSymbol() const {
    static const std::string fallback = "TEX";
    const CryptoCoin* active = GetActiveCoin();
    return active ? active->symbol : fallback;
}

std::vector<CryptoCoin>& EconomyManager::GetCoins() {
    return m_coins;
}

const std::vector<CryptoCoin>& EconomyManager::GetCoins() const {
    return m_coins;
}

size_t EconomyManager::GetActiveCoinIndex() const {
    return m_activeCoinIndex;
}

void EconomyManager::SetActiveCoinIndex(size_t index) {
    if (index < m_coins.size()) {
        m_activeCoinIndex = index;
    }
}

CryptoCoin* EconomyManager::GetActiveCoin() {
    if (m_activeCoinIndex < m_coins.size()) {
        return &m_coins[m_activeCoinIndex];
    }
    return m_coins.empty() ? nullptr : &m_coins[0];
}

const CryptoCoin* EconomyManager::GetActiveCoin() const {
    if (m_activeCoinIndex < m_coins.size()) {
        return &m_coins[m_activeCoinIndex];
    }
    return m_coins.empty() ? nullptr : &m_coins[0];
}

CryptoCoin* EconomyManager::GetCoin(size_t index) {
    if (index < m_coins.size()) {
        return &m_coins[index];
    }
    return nullptr;
}

const CryptoCoin* EconomyManager::GetCoin(size_t index) const {
    if (index < m_coins.size()) {
        return &m_coins[index];
    }
    return nullptr;
}

CryptoCoin* EconomyManager::GetCoinById(const std::string& id) {
    for (auto& coin : m_coins) {
        if (coin.id == id) {
            return &coin;
        }
    }
    return nullptr;
}

const CryptoCoin* EconomyManager::GetCoinById(const std::string& id) const {
    for (const auto& coin : m_coins) {
        if (coin.id == id) {
            return &coin;
        }
    }
    return nullptr;
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
    if (usdPrice < 1.0) {
        ss << std::fixed << std::setprecision(4);
    } else if (usdPrice < 100.0) {
        ss << std::fixed << std::setprecision(2);
    } else {
        ss << std::fixed << std::setprecision(0);
    }

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

void EconomyManager::SetFiatBalance(double balance) {
    m_fiatBalance = std::max(0.0, balance);
}

void EconomyManager::SetCryptoBalance(double balance) {
    CryptoCoin* active = GetActiveCoin();
    if (active) {
        active->balance = std::max(0.0, balance);
    }
}

void EconomyManager::SetCryptoPrice(double price) {
    CryptoCoin* active = GetActiveCoin();
    if (active && price > 0.0) {
        active->priceUSD = price;
    }
}

void EconomyManager::SetCoinBalance(const std::string& coinId, double balance) {
    CryptoCoin* coin = GetCoinById(coinId);
    if (coin) {
        coin->balance = std::max(0.0, balance);
    }
}

void EconomyManager::SetCoinPrice(const std::string& coinId, double price) {
    CryptoCoin* coin = GetCoinById(coinId);
    if (coin && price > 0.0) {
        coin->priceUSD = price;
    }
}

double EconomyManager::GetCryptoPrice() const {
    const CryptoCoin* active = GetActiveCoin();
    return active ? active->priceUSD : 2.40;
}

double EconomyManager::GetNetworkDifficulty() const {
    const CryptoCoin* active = GetActiveCoin();
    return active ? active->difficulty : 150000.0;
}

void EconomyManager::UpdateMarket(double deltaTimeSeconds) {
    m_marketTimer += deltaTimeSeconds;

    if (m_marketTimer >= 1.0) {
        m_marketTimer = 0.0;

        static std::mt19937 rng(42);
        static std::normal_distribution<double> dist(0.0, 0.007); // %0.7 volatility

        for (auto& coin : m_coins) {
            const double percentageChange = dist(rng);
            coin.priceUSD *= (1.0 + percentageChange);

            // Mean-reversion pull towards base price so it stays realistic
            coin.priceUSD += (coin.basePriceUSD - coin.priceUSD) * 0.015;

            // Strict floor
            double floorPrice = coin.basePriceUSD * 0.20;
            coin.priceUSD = std::max(floorPrice, coin.priceUSD);

            // Update high / low
            if (coin.priceUSD > coin.high24h) coin.high24h = coin.priceUSD;
            if (coin.priceUSD < coin.low24h) coin.low24h = coin.priceUSD;

            // Push to history
            coin.priceHistory.push_back(static_cast<float>(coin.priceUSD));
            if (coin.priceHistory.size() > 60) {
                coin.priceHistory.erase(coin.priceHistory.begin());
            }

            // 24h change calculation
            if (!coin.priceHistory.empty()) {
                double firstPrice = coin.priceHistory.front();
                if (firstPrice > 0.00001) {
                    coin.priceChange24hPercent = ((coin.priceUSD - firstPrice) / firstPrice) * 100.0;
                }
            }
        }
    }
}

bool EconomyManager::BuyCoin(size_t coinIndex, double usdAmount) {
    if (coinIndex >= m_coins.size() || usdAmount <= 0.0 || m_fiatBalance < usdAmount) {
        return false;
    }
    CryptoCoin& coin = m_coins[coinIndex];
    if (coin.priceUSD <= 0.0) return false;

    double coinsBought = usdAmount / coin.priceUSD;
    m_fiatBalance -= usdAmount;
    coin.balance += coinsBought;
    return true;
}

bool EconomyManager::SellCoin(size_t coinIndex, double coinAmount) {
    if (coinIndex >= m_coins.size() || coinAmount <= 0.0) {
        return false;
    }
    CryptoCoin& coin = m_coins[coinIndex];
    if (coin.balance < coinAmount || coin.priceUSD <= 0.0) {
        return false;
    }
    coin.balance -= coinAmount;
    double revenueUSD = coinAmount * coin.priceUSD;
    m_fiatBalance += revenueUSD;
    return true;
}

double EconomyManager::GetTotalPortfolioValueUSD() const {
    double total = m_fiatBalance;
    for (const auto& coin : m_coins) {
        total += coin.balance * coin.priceUSD;
    }
    return total;
}

double EconomyManager::MineCoins(double hashrateMHS, double deltaTimeSeconds) {
    if (hashrateMHS <= 0.0 || deltaTimeSeconds <= 0.0) {
        return 0.0;
    }

    CryptoCoin* active = GetActiveCoin();
    if (!active) return 0.0;

    // Difficulty adjusts smoothly with player's hashrate
    active->difficulty = active->baseDifficulty + (hashrateMHS * 160.0);

    const double mintedCoins = (hashrateMHS * deltaTimeSeconds) / active->difficulty;
    active->balance += mintedCoins;
    return mintedCoins;
}

bool EconomyManager::SellCrypto(double amount) {
    return SellCoin(m_activeCoinIndex, amount);
}

double EconomyManager::CalculateHourlyCoins(double hashrateMHS) const {
    const CryptoCoin* active = GetActiveCoin();
    if (!active || hashrateMHS <= 0.0 || active->difficulty <= 0.0) return 0.0;
    return (hashrateMHS * 3600.0) / active->difficulty;
}

double EconomyManager::CalculateHourlyRevenueUSD(double hashrateMHS) const {
    const CryptoCoin* active = GetActiveCoin();
    if (!active) return 0.0;
    return CalculateHourlyCoins(hashrateMHS) * active->priceUSD;
}

double EconomyManager::CalculateDailyRevenueUSD(double hashrateMHS) const {
    return CalculateHourlyRevenueUSD(hashrateMHS) * 24.0;
}

double EconomyManager::CalculateHourlyElectricityCostUSD(double powerWatts, double electricityRateKWh) const {
    if (powerWatts <= 0.0 || electricityRateKWh <= 0.0) return 0.0;
    return (powerWatts / 1000.0) * electricityRateKWh;
}

double EconomyManager::CalculateHourlyNetProfitUSD(double hashrateMHS, double powerWatts, double electricityRateKWh) const {
    return CalculateHourlyRevenueUSD(hashrateMHS) - CalculateHourlyElectricityCostUSD(powerWatts, electricityRateKWh);
}

} // namespace Core
