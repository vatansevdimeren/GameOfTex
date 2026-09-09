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
    , m_eventTimer(0.0)
    , m_nextEventInterval(75.0)
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

    auto generateInitialCandles = [](double basePrice, double variance, std::vector<float>& outHistory) {
        std::vector<Candle> candles;
        candles.reserve(28);
        outHistory.reserve(60);

        std::mt19937 rng(1337 + static_cast<unsigned int>(basePrice * 100.0));
        std::normal_distribution<double> dist(0.0, variance);

        double curr = basePrice;
        for (int i = 0; i < 60; ++i) {
            curr += dist(rng);
            curr += (basePrice - curr) * 0.08;
            curr = std::clamp(curr, basePrice * 0.75, basePrice * 1.35);
            outHistory.push_back(static_cast<float>(curr));
        }

        double candleClose = basePrice;
        for (int i = 0; i < 26; ++i) {
            double open = candleClose;
            double change = dist(rng) * 2.2;
            candleClose = open + change;
            candleClose += (basePrice - candleClose) * 0.09;
            candleClose = std::clamp(candleClose, basePrice * 0.75, basePrice * 1.35);

            double high = std::max(open, candleClose) + std::abs(dist(rng) * 0.9);
            double low = std::min(open, candleClose) - std::abs(dist(rng) * 0.9);
            high = std::max(high, std::max(open, candleClose));
            low = std::min(low, std::min(open, candleClose));
            float vol = static_cast<float>(basePrice * (80.0 + (i % 7) * 25.0));

            candles.push_back(Candle{
                static_cast<float>(open),
                static_cast<float>(high),
                static_cast<float>(low),
                static_cast<float>(candleClose),
                vol
            });
        }
        return candles;
    };

    // 1. TEX - TexCoin
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
        tex.baseDifficulty = 75000.0;
        tex.difficulty = tex.baseDifficulty;
        tex.blockReward = 5.0;
        tex.high24h = tex.basePriceUSD * 1.08;
        tex.low24h = tex.basePriceUSD * 0.92;
        tex.volume24hUSD = 850000.0;
        tex.candles = generateInitialCandles(tex.basePriceUSD, 0.025, tex.priceHistory);
        tex.currentCandle = Candle{static_cast<float>(tex.priceUSD), static_cast<float>(tex.priceUSD), static_cast<float>(tex.priceUSD), static_cast<float>(tex.priceUSD), 0.0f};
        tex.profitabilityMultiplier = 1.0;
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
        rvn.baseDifficulty = 25000.0;
        rvn.difficulty = rvn.baseDifficulty;
        rvn.blockReward = 2500.0;
        rvn.high24h = 0.092;
        rvn.low24h = 0.078;
        rvn.volume24hUSD = 3450000.0;
        rvn.candles = generateInitialCandles(rvn.basePriceUSD, 0.0015, rvn.priceHistory);
        rvn.currentCandle = Candle{static_cast<float>(rvn.priceUSD), static_cast<float>(rvn.priceUSD), static_cast<float>(rvn.priceUSD), static_cast<float>(rvn.priceUSD), 0.0f};
        rvn.profitabilityMultiplier = 1.0;
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
        etc.baseDifficulty = 850000.0;
        etc.difficulty = etc.baseDifficulty;
        etc.blockReward = 2.56;
        etc.high24h = 30.40;
        etc.low24h = 26.80;
        etc.volume24hUSD = 18200000.0;
        etc.candles = generateInitialCandles(etc.basePriceUSD, 0.35, etc.priceHistory);
        etc.currentCandle = Candle{static_cast<float>(etc.priceUSD), static_cast<float>(etc.priceUSD), static_cast<float>(etc.priceUSD), static_cast<float>(etc.priceUSD), 0.0f};
        etc.profitabilityMultiplier = 1.0;
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
        ethw.baseDifficulty = 4200000.0;
        ethw.difficulty = ethw.baseDifficulty;
        ethw.blockReward = 2.0;
        ethw.high24h = 153.20;
        ethw.low24h = 138.50;
        ethw.volume24hUSD = 45000000.0;
        ethw.candles = generateInitialCandles(ethw.basePriceUSD, 1.40, ethw.priceHistory);
        ethw.currentCandle = Candle{static_cast<float>(ethw.priceUSD), static_cast<float>(ethw.priceUSD), static_cast<float>(ethw.priceUSD), static_cast<float>(ethw.priceUSD), 0.0f};
        ethw.profitabilityMultiplier = 1.0;
        ethw.requiredTier = 3;
        ethw.isUnlocked = true;
        m_coins.push_back(std::move(ethw));
    }

    // 5. BTC - Bitcoin
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
        btc.candles = generateInitialCandles(btc.basePriceUSD, 240.0, btc.priceHistory);
        btc.currentCandle = Candle{static_cast<float>(btc.priceUSD), static_cast<float>(btc.priceUSD), static_cast<float>(btc.priceUSD), static_cast<float>(btc.priceUSD), 0.0f};
        btc.profitabilityMultiplier = 1.0;
        btc.requiredTier = 4;
        btc.isUnlocked = true;
        m_coins.push_back(std::move(btc));
    }

    // 6. XMR - Monero (CPU RandomX)
    {
        CryptoCoin xmr;
        xmr.id = "XMR";
        xmr.name = "Monero (CPU)";
        xmr.symbol = "XMR";
        xmr.algorithm = "RandomX";
        xmr.unit = "KH/s";
        xmr.balance = 0.0;
        xmr.basePriceUSD = 168.50;
        xmr.priceUSD = xmr.basePriceUSD;
        xmr.baseDifficulty = 42000.0;
        xmr.difficulty = xmr.baseDifficulty;
        xmr.blockReward = 0.60;
        xmr.high24h = 174.20;
        xmr.low24h = 162.80;
        xmr.volume24hUSD = 12500000.0;
        xmr.candles = generateInitialCandles(xmr.basePriceUSD, 1.20, xmr.priceHistory);
        xmr.currentCandle = Candle{static_cast<float>(xmr.priceUSD), static_cast<float>(xmr.priceUSD), static_cast<float>(xmr.priceUSD), static_cast<float>(xmr.priceUSD), 0.0f};
        xmr.profitabilityMultiplier = 1.0;
        xmr.requiredTier = 1;
        xmr.isUnlocked = true;
        xmr.isCPUCoin = true;
        m_coins.push_back(std::move(xmr));
    }

    // 7. RTM - Raptoreum (CPU GhostRider)
    {
        CryptoCoin rtm;
        rtm.id = "RTM";
        rtm.name = "Raptoreum (CPU)";
        rtm.symbol = "RTM";
        rtm.algorithm = "GhostRider";
        rtm.unit = "KH/s";
        rtm.balance = 0.0;
        rtm.basePriceUSD = 0.0022;
        rtm.priceUSD = rtm.basePriceUSD;
        rtm.baseDifficulty = 1800.0;
        rtm.difficulty = rtm.baseDifficulty;
        rtm.blockReward = 750.0;
        rtm.high24h = 0.0025;
        rtm.low24h = 0.0019;
        rtm.volume24hUSD = 1450000.0;
        rtm.candles = generateInitialCandles(rtm.basePriceUSD, 0.00005, rtm.priceHistory);
        rtm.currentCandle = Candle{static_cast<float>(rtm.priceUSD), static_cast<float>(rtm.priceUSD), static_cast<float>(rtm.priceUSD), static_cast<float>(rtm.priceUSD), 0.0f};
        rtm.profitabilityMultiplier = 1.0;
        rtm.requiredTier = 1;
        rtm.isUnlocked = true;
        rtm.isCPUCoin = true;
        m_coins.push_back(std::move(rtm));
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
        // Clamp to realistic band around base price
        active->priceUSD = std::clamp(price, active->basePriceUSD * 0.70, active->basePriceUSD * 1.45);
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
        // Clamp to realistic band around base price so legacy saves never scam
        coin->priceUSD = std::clamp(price, coin->basePriceUSD * 0.70, coin->basePriceUSD * 1.45);
    }
}

double EconomyManager::GetCryptoPrice() const {
    const CryptoCoin* active = GetActiveCoin();
    return active ? active->priceUSD : 2.40;
}

double EconomyManager::GetNetworkDifficulty() const {
    const CryptoCoin* active = GetActiveCoin();
    return active ? active->difficulty : 120000.0;
}

const MarketEvent& EconomyManager::GetActiveEvent() const {
    return m_activeEvent;
}

bool EconomyManager::HasActiveEvent() const {
    return (m_activeEvent.type != MarketEventType::NORMAL && m_activeEvent.remainingSeconds > 0.0);
}

void EconomyManager::TriggerRandomMarketEvent() {
    static std::mt19937 evRng(std::random_device{}());
    std::uniform_int_distribution<int> evDist(0, 3);
    int pick = evDist(evRng);

    switch (pick) {
        case 0:
            m_activeEvent.type = MarketEventType::BULL_RALLY;
            m_activeEvent.headlineTR = "BOĞA RALLİSİ! Kurumsal Yatırımcı Girişi Başladı (+%18)";
            m_activeEvent.headlineEN = "BULL RUN! Institutional Inflow Surge (+18%)";
            m_activeEvent.durationSeconds = 24.0;
            m_activeEvent.remainingSeconds = 24.0;
            m_activeEvent.driftFactor = 0.0035; // ~+8% over duration
            m_activeEvent.bannerColor = Color{40, 225, 120, 255};
            break;
        case 1:
            m_activeEvent.type = MarketEventType::CORRECTION_CRASH;
            m_activeEvent.headlineTR = "PİYASA DÜZELTMESİ! Kâr Satışları ve Tasfiyeler (-%16)";
            m_activeEvent.headlineEN = "CORRECTION CRASH! Liquidation Wave (-16%)";
            m_activeEvent.durationSeconds = 24.0;
            m_activeEvent.remainingSeconds = 24.0;
            m_activeEvent.driftFactor = -0.0035; // ~-8% over duration
            m_activeEvent.bannerColor = Color{255, 65, 75, 255};
            break;
        case 2:
            m_activeEvent.type = MarketEventType::MINING_BOOM;
            m_activeEvent.headlineTR = "MADENCİLİK PATLAMASI! Donanım ve Ağ Hacmi Arttı (+%12)";
            m_activeEvent.headlineEN = "MINING BOOM! Global Hashrate Expansion (+12%)";
            m_activeEvent.durationSeconds = 20.0;
            m_activeEvent.remainingSeconds = 20.0;
            m_activeEvent.driftFactor = 0.0025;
            m_activeEvent.bannerColor = Color{255, 215, 60, 255};
            break;
        case 3:
            m_activeEvent.type = MarketEventType::REGULATION_NEWS;
            m_activeEvent.headlineTR = "DÜZENLEME BASKISI! Piyasa Bekle-Gör Modunda (-%10)";
            m_activeEvent.headlineEN = "REGULATORY WATCH! Short-term Dip (-10%)";
            m_activeEvent.durationSeconds = 20.0;
            m_activeEvent.remainingSeconds = 20.0;
            m_activeEvent.driftFactor = -0.0022;
            m_activeEvent.bannerColor = Color{255, 150, 45, 255};
            break;
    }
}

void EconomyManager::UpdateMarket(double deltaTimeSeconds) {
    m_marketTimer += deltaTimeSeconds;
    m_eventTimer += deltaTimeSeconds;
    m_rotationTimer += deltaTimeSeconds;

    // 1. Piyasa Olayı Zamanlayıcısı
    if (m_activeEvent.type != MarketEventType::NORMAL) {
        m_activeEvent.remainingSeconds -= deltaTimeSeconds;
        if (m_activeEvent.remainingSeconds <= 0.0) {
            m_activeEvent.type = MarketEventType::NORMAL;
            m_activeEvent.remainingSeconds = 0.0;
        }
    } else if (m_eventTimer >= m_nextEventInterval) {
        m_eventTimer = 0.0;
        TriggerRandomMarketEvent();
    }

    // 2. Dönemsel Kârlılık Rotasyonu (Her 50 saniyede bir coin parlar / altcoin season)
    if (m_rotationTimer >= 50.0) {
        m_rotationTimer = 0.0;
        static std::mt19937 rotRng(std::random_device{}());
        std::uniform_int_distribution<size_t> coinDist(0, m_coins.size() - 1);
        size_t luckyIndex = coinDist(rotRng);

        for (size_t i = 0; i < m_coins.size(); ++i) {
            if (i == luckyIndex) {
                // Şanslı coin'e %180 - %240 madencilik verim artışı
                std::uniform_real_distribution<double> boostDist(1.85, 2.35);
                m_coins[i].profitabilityMultiplier = boostDist(rotRng);
            } else {
                // Diğer coinler dengeli seviyede
                std::uniform_real_distribution<double> normDist(0.92, 1.12);
                m_coins[i].profitabilityMultiplier = normDist(rotRng);
            }
        }
    }

    // 3. Mikro Volatilite ve Mum Güncellemesi (Her saniye bağımsız yürüyüş)
    if (m_marketTimer >= 1.0) {
        m_marketTimer = 0.0;

        static std::mt19937 rng(1337);
        static std::normal_distribution<double> dist(0.0, 0.0011);

        for (size_t i = 0; i < m_coins.size(); ++i) {
            auto& coin = m_coins[i];
            // Her coin için bağımsız rassal değişim
            const double percentageChange = dist(rng);
            coin.priceUSD *= (1.0 + percentageChange);

            // Aktif olay etkisi (hedef coin varsa veya genel piyasaysa)
            if (m_activeEvent.type != MarketEventType::NORMAL) {
                if (m_activeEvent.targetCoinId.empty() || m_activeEvent.targetCoinId == coin.id) {
                    coin.priceUSD *= (1.0 + m_activeEvent.driftFactor);
                }
            }

            // Ortalama fiyata çekim (Mean Reversion)
            coin.priceUSD += (coin.basePriceUSD - coin.priceUSD) * 0.010;

            // Kesin Fiyat Koridoru / Bollinger Kanalı (%70 ile %145 arası)
            const double minAllowed = coin.basePriceUSD * 0.70;
            const double maxAllowed = coin.basePriceUSD * 1.45;
            coin.priceUSD = std::clamp(coin.priceUSD, minAllowed, maxAllowed);

            // 24h En Yüksek / En Düşük
            if (coin.priceUSD > coin.high24h) coin.high24h = coin.priceUSD;
            if (coin.priceUSD < coin.low24h) coin.low24h = coin.priceUSD;

            // Çizgi Grafik Geçmişi
            coin.priceHistory.push_back(static_cast<float>(coin.priceUSD));
            if (coin.priceHistory.size() > 60) {
                coin.priceHistory.erase(coin.priceHistory.begin());
            }

            // 24h Yüzde Değişim
            if (!coin.priceHistory.empty()) {
                double firstPrice = coin.priceHistory.front();
                if (firstPrice > 0.000001) {
                    coin.priceChange24hPercent = ((coin.priceUSD - firstPrice) / firstPrice) * 100.0;
                }
            }

            // Mum (Candlestick) Verilerini Güncelle
            coin.currentCandle.high = std::max(coin.currentCandle.high, static_cast<float>(coin.priceUSD));
            coin.currentCandle.low = std::min(coin.currentCandle.low, static_cast<float>(coin.priceUSD));
            coin.currentCandle.close = static_cast<float>(coin.priceUSD);
            coin.currentCandle.volume += static_cast<float>(coin.priceUSD * (30.0 + (i * 12.0)));
        }
    }

    // 3. Mum Kapanış Kontrolü (Her 4 saniyede bir yeni mum açılır)
    for (auto& coin : m_coins) {
        coin.candleTimer += static_cast<float>(deltaTimeSeconds);
        if (coin.candleTimer >= 4.0f) {
            coin.candleTimer = 0.0f;

            // Mevcut mumu listeye ekle
            coin.candles.push_back(coin.currentCandle);
            if (coin.candles.size() > 30) {
                coin.candles.erase(coin.candles.begin());
            }

            // Yeni mumu başlat
            float lastClose = coin.currentCandle.close;
            coin.currentCandle.open = lastClose;
            coin.currentCandle.high = lastClose;
            coin.currentCandle.low = lastClose;
            coin.currentCandle.close = lastClose;
            coin.currentCandle.volume = 0.0f;
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

    // Unit conversion: GH/s coins (like BTC) receive 1/1000 hashrate from MH/s rigs
    const double effectiveHash = (active->unit == "GH/s" ? (hashrateMHS / 1000.0) : hashrateMHS) * active->profitabilityMultiplier;

    // Difficulty adjusts smoothly with player's hashrate
    active->difficulty = active->baseDifficulty + (effectiveHash * 160.0);

    const double mintedCoins = (effectiveHash * deltaTimeSeconds) / active->difficulty;
    active->balance += mintedCoins;
    return mintedCoins;
}

double EconomyManager::MineCPUShare(double cpuHashrateKH, double deltaTimeSeconds) {
    if (cpuHashrateKH <= 0.0 || deltaTimeSeconds <= 0.0) {
        return 0.0;
    }

    // Hedef CPU coini: Aktif coin bir CPU coini ise onu kaz, değilse otomatik olarak Monero (XMR) kaz!
    CryptoCoin* target = nullptr;
    CryptoCoin* active = GetActiveCoin();
    if (active && active->isCPUCoin) {
        target = active;
    } else {
        for (auto& c : m_coins) {
            if (c.symbol == "XMR") {
                target = &c;
                break;
            }
        }
    }

    if (!target) return 0.0;

    const double effectiveHash = cpuHashrateKH * target->profitabilityMultiplier;
    target->difficulty = target->baseDifficulty + (effectiveHash * 45.0);

    const double minted = (effectiveHash * deltaTimeSeconds) / target->difficulty;
    target->balance += minted;
    return minted;
}

double EconomyManager::GetCPUCoinBalance() const {
    for (const auto& c : m_coins) {
        if (c.symbol == "XMR") return c.balance;
    }
    return 0.0;
}

std::string EconomyManager::GetCPUCoinSymbol() const {
    return "XMR";
}

bool EconomyManager::SellCrypto(double amount) {
    return SellCoin(m_activeCoinIndex, amount);
}

double EconomyManager::CalculateHourlyCoins(double hashrateMHS) const {
    const CryptoCoin* active = GetActiveCoin();
    if (!active || hashrateMHS <= 0.0 || active->difficulty <= 0.0) return 0.0;
    const double effectiveHash = (active->unit == "GH/s" ? (hashrateMHS / 1000.0) : hashrateMHS) * active->profitabilityMultiplier;
    return (effectiveHash * 3600.0) / active->difficulty;
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

double EconomyManager::CalculateDailyYieldCoinsPer100MH(const CryptoCoin& coin) const {
    if (coin.difficulty <= 0.0) return 0.0;
    const double effectiveHash = (coin.unit == "GH/s" ? (100.0 / 1000.0) : 100.0) * coin.profitabilityMultiplier;
    return (effectiveHash * 86400.0) / coin.difficulty;
}

double EconomyManager::CalculateDailyYieldUSDPer100MH(const CryptoCoin& coin) const {
    return CalculateDailyYieldCoinsPer100MH(coin) * coin.priceUSD;
}

double EconomyManager::CalculateProfitabilityPercent(const CryptoCoin& coin) const {
    if (coin.basePriceUSD <= 0.00001 || coin.baseDifficulty <= 0.00001) return 100.0;
    double currentYieldUSD = CalculateDailyYieldUSDPer100MH(coin);
    double baseEffective = (coin.unit == "GH/s" ? (100.0 / 1000.0) : 100.0);
    double baselineYieldUSD = (baseEffective * 86400.0 / coin.baseDifficulty) * coin.basePriceUSD;
    if (baselineYieldUSD <= 0.00001) return 100.0;
    return (currentYieldUSD / baselineYieldUSD) * 100.0;
}

size_t EconomyManager::GetMostProfitableCoinIndex() const {
    if (m_coins.empty()) return 0;
    size_t bestIdx = 0;
    double bestUSD = -1.0;
    for (size_t i = 0; i < m_coins.size(); ++i) {
        double yieldUSD = CalculateDailyYieldUSDPer100MH(m_coins[i]);
        if (yieldUSD > bestUSD) {
            bestUSD = yieldUSD;
            bestIdx = i;
        }
    }
    return bestIdx;
}

} // namespace Core
