#pragma once

#include <string>
#include <vector>
#include <raylib.h>

namespace Core {

enum class CurrencyType {
    USD,
    USDT,
    TRY,
    EUR
};

/**
 * @struct Candle
 * @brief Represents a candlestick bar (Open, High, Low, Close, Volume).
 */
struct Candle {
    float open{0.0f};
    float high{0.0f};
    float low{0.0f};
    float close{0.0f};
    float volume{0.0f};
};

/**
 * @enum MarketEventType
 * @brief Event types driving macro market news and controlled price waves.
 */
enum class MarketEventType {
    NORMAL,
    BULL_RALLY,
    CORRECTION_CRASH,
    REGULATION_NEWS,
    MINING_BOOM
};

/**
 * @struct MarketEvent
 * @brief Real-time event that temporarily creates positive or negative market trends.
 */
struct MarketEvent {
    MarketEventType type{MarketEventType::NORMAL};
    std::string headlineTR;
    std::string headlineEN;
    std::string targetCoinId; // "" applies to market or specific coin symbol
    double durationSeconds{0.0};
    double remainingSeconds{0.0};
    double driftFactor{0.0};
    Color bannerColor{60, 180, 240, 255};
};

/**
 * @struct CryptoCoin
 * @brief Represents an individual mineable and tradable cryptocurrency.
 */
struct CryptoCoin {
    std::string id;                  // "TEX", "RVN", "ETC", "ETHW", "BTC"
    std::string name;                // "TexCoin", "Ravencoin", "Ethereum Classic", "Ethereum PoW", "Bitcoin"
    std::string symbol;              // "TEX", "RVN", "ETC", "ETHW", "BTC"
    std::string algorithm;           // "KawPow", "X16R", "ETChash", "Ethash", "SHA-256"
    std::string unit;                // "MH/s" or "GH/s"
    double balance{0.0};             // Wallet coin count
    double priceUSD{2.40};           // Current live market price
    double basePriceUSD{2.40};       // Market center anchor
    double difficulty{120000.0};     // Dynamic mining difficulty
    double baseDifficulty{120000.0}; // Base network difficulty
    double blockReward{5.0};         // Reward per block
    double priceChange24hPercent{0.0};
    double high24h{2.55};
    double low24h{2.25};
    double volume24hUSD{1250000.0};
    std::vector<float> priceHistory; // Live historical tick series
    std::vector<Candle> candles;     // Candlestick OHLC series (30 bars)
    Candle currentCandle;            // Active building candlestick
    float candleTimer{0.0f};         // Timer for active candle duration (4 seconds/bar)
    double profitabilityMultiplier{1.0}; // Dynamic rotation boost (0.8x - 2.5x)
    int requiredTier{1};             // 1: Starter, 2: Pro, 3: Server, 4: Industrial ASIC
    bool isUnlocked{true};
    bool isCPUCoin{false};           // True for CPU-mineable coins (XMR, RTM)
};

/**
 * @class EconomyManager
 * @brief Manages fiat bank balance, multi-coin crypto wallets, network difficulty, and live exchange markets.
 * 
 * SRP: This class is solely responsible for economic transactions,
 * multi-coin wallet balances, market fluctuations, price histories, and block mining rewards.
 */
class EconomyManager {
public:
    /**
     * @brief Constructs economy manager with initial fiat cash and coin catalog.
     * @param initialFiat Starting cash in USD (e.g. $1,500).
     * @param initialCryptoPrice Starting price per coin in USD for default coin.
     * @param coinSymbol Ticker symbol (e.g. "TEX").
     */
    EconomyManager(double initialFiat, double initialCryptoPrice = 2.40, std::string coinSymbol = "TEX");

    // Balances
    [[nodiscard]] double GetFiatBalance() const;
    [[nodiscard]] double GetCryptoBalance() const;
    [[nodiscard]] const std::string& GetCoinSymbol() const;

    // Multi-Coin Accessors
    [[nodiscard]] std::vector<CryptoCoin>& GetCoins();
    [[nodiscard]] const std::vector<CryptoCoin>& GetCoins() const;
    [[nodiscard]] size_t GetActiveCoinIndex() const;
    void SetActiveCoinIndex(size_t index);
    [[nodiscard]] CryptoCoin* GetActiveCoin();
    [[nodiscard]] const CryptoCoin* GetActiveCoin() const;
    [[nodiscard]] CryptoCoin* GetCoin(size_t index);
    [[nodiscard]] const CryptoCoin* GetCoin(size_t index) const;
    [[nodiscard]] CryptoCoin* GetCoinById(const std::string& id);
    [[nodiscard]] const CryptoCoin* GetCoinById(const std::string& id) const;

    // Currency controls
    void SetCurrency(CurrencyType curr);
    [[nodiscard]] CurrencyType GetCurrency() const;
    void NextCurrency();
    [[nodiscard]] std::string GetCurrencyCode() const;
    [[nodiscard]] std::string GetCurrencySymbol() const;
    [[nodiscard]] double GetExchangeRate() const;
    [[nodiscard]] std::string FormatFiat(double usdAmount) const;
    [[nodiscard]] std::string FormatPrice(double usdPrice) const;

    // Direct balance modifiers
    void AddFiat(double amount);
    bool DeductFiat(double amount);
    void SetFiatBalance(double balance);
    void SetCryptoBalance(double balance);
    void SetCryptoPrice(double price);
    void SetCoinBalance(const std::string& coinId, double balance);
    void SetCoinPrice(const std::string& coinId, double price);

    // Market simulation & Events
    [[nodiscard]] double GetCryptoPrice() const;
    [[nodiscard]] double GetNetworkDifficulty() const;
    void UpdateMarket(double deltaTimeSeconds);

    [[nodiscard]] const MarketEvent& GetActiveEvent() const;
    [[nodiscard]] bool HasActiveEvent() const;

    // Kripto Alım-Satım Borsası (Exchange Desk)
    bool BuyCoin(size_t coinIndex, double usdAmount);
    bool SellCoin(size_t coinIndex, double coinAmount);
    [[nodiscard]] double GetTotalPortfolioValueUSD() const;

    /**
     * @brief Mines active coins based on hashrate and current network difficulty.
     * @param hashrateMHS Total hashrate in Megahashes per second.
     * @param deltaTimeSeconds Elapsed simulation time.
     * @return Number of newly minted coins credited to the wallet.
     */
    double MineCoins(double hashrateMHS, double deltaTimeSeconds);

    /**
     * @brief Mines CPU coins (Monero XMR / Raptoreum RTM) based on CPU KH/s.
     * @param cpuHashrateKH Total CPU hashrate in Kilohashes per second.
     * @param deltaTimeSeconds Elapsed simulation time.
     * @return Number of newly minted CPU coins credited to the wallet.
     */
    double MineCPUShare(double cpuHashrateKH, double deltaTimeSeconds);
    [[nodiscard]] double GetCPUCoinBalance() const;
    [[nodiscard]] std::string GetCPUCoinSymbol() const;

    bool SellCrypto(double amount);

    // Saatlik ve Günlük Getiri/Kar/Verimlilik Hesaplamaları
    [[nodiscard]] double CalculateHourlyCoins(double hashrateMHS) const;
    [[nodiscard]] double CalculateHourlyRevenueUSD(double hashrateMHS) const;
    [[nodiscard]] double CalculateDailyRevenueUSD(double hashrateMHS) const;
    [[nodiscard]] double CalculateHourlyElectricityCostUSD(double powerWatts, double electricityRateKWh) const;
    [[nodiscard]] double CalculateHourlyNetProfitUSD(double hashrateMHS, double powerWatts, double electricityRateKWh) const;

    // Coin Başına Özel Karlılık ve Verimlilik Metrikleri
    [[nodiscard]] double CalculateDailyYieldCoinsPer100MH(const CryptoCoin& coin) const;
    [[nodiscard]] double CalculateDailyYieldUSDPer100MH(const CryptoCoin& coin) const;
    [[nodiscard]] double CalculateProfitabilityPercent(const CryptoCoin& coin) const;
    [[nodiscard]] size_t GetMostProfitableCoinIndex() const;

private:
    void InitCoins(double initialTexPrice);
    void TriggerRandomMarketEvent();

    double m_fiatBalance;
    CurrencyType m_currency{CurrencyType::USD};
    double m_marketTimer{0.0};
    double m_eventTimer{0.0};
    double m_rotationTimer{0.0};
    double m_nextEventInterval{80.0}; // seconds between events

    MarketEvent m_activeEvent;

    std::vector<CryptoCoin> m_coins;
    size_t m_activeCoinIndex{0};
};

} // namespace Core
