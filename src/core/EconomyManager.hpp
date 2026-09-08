#pragma once

#include <string>

namespace Core {

enum class CurrencyType {
    USD,
    USDT,
    TRY,
    EUR
};

/**
 * @class EconomyManager
 * @brief Manages fiat bank balance, crypto wallet, network difficulty, and market prices.
 * 
 * SRP: This class is solely responsible for economic transactions,
 * wallet balances, market fluctuations, and block mining rewards.
 */
class EconomyManager {
public:
    /**
     * @brief Constructs economy manager with initial fiat cash and coin price.
     * @param initialFiat Starting cash in USD (e.g. $1,500).
     * @param initialCryptoPrice Starting price per coin in USD (e.g. $2,500).
     * @param coinSymbol Ticker symbol (e.g. "TEX").
     */
    EconomyManager(double initialFiat, double initialCryptoPrice = 2500.0, std::string coinSymbol = "TEX");

    // Balances
    [[nodiscard]] double GetFiatBalance() const;
    [[nodiscard]] double GetCryptoBalance() const;
    [[nodiscard]] const std::string& GetCoinSymbol() const;

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

    // Market simulation
    [[nodiscard]] double GetCryptoPrice() const;
    [[nodiscard]] double GetNetworkDifficulty() const;
    void UpdateMarket(double deltaTimeSeconds);

    /**
     * @brief Mines coins based on hashrate and current network difficulty.
     * @param hashrateMHS Total hashrate in Megahashes per second.
     * @param deltaTimeSeconds Elapsed simulation time.
     * @return Number of newly minted coins credited to the wallet.
     */
    double MineCoins(double hashrateMHS, double deltaTimeSeconds);

    /**
     * @brief Sells an amount of cryptocurrency at current market price.
     * @param amount Amount of crypto to liquidate.
     * @return True if sold successfully, false if insufficient crypto balance.
     */
    bool SellCrypto(double amount);

private:
    double m_fiatBalance;
    double m_cryptoBalance;
    std::string m_coinSymbol;

    CurrencyType m_currency{CurrencyType::USD};
    double m_cryptoPrice;
    double m_networkDifficulty; // Difficulty divisor for mining
    double m_marketTimer;
};

} // namespace Core
