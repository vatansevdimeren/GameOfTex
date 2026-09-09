#include "NewsManager.hpp"
#include "EconomyManager.hpp"
#include <algorithm>
#include <random>

namespace Core {

struct NewsTemplate {
    std::string author;
    std::string handle;
    std::string contentTR;
    std::string contentEN;
    std::string targetCoin;
    NewsSentiment sentiment;
    bool isContrarian; // if true, fake news / opposite effect!
    double impactPercent;
};

static const std::vector<NewsTemplate> g_newsPool = {
    {
        "CryptoWhale 🐋", "@whale_trader",
        "TEX grafiğinde devasa Golden Cross oluştu! Balinalar sessizce topluyor, hedef $3.20! 🚀",
        "Massive Golden Cross formed on TEX chart! Whales are accumulating, target $3.20! 🚀",
        "TEX", NewsSentiment::BULLISH, false, 16.0
    },
    {
        "Bloomberg Crypto", "@bloomberg_desk",
        "SEC madencilik düzenlemesini erteledi. Bitcoin ve altcoin piyasalarında rahatlama rallisi.",
        "SEC delayed crypto mining regulations. Relief rally across Bitcoin and altcoins.",
        "MARKET", NewsSentiment::BULLISH, false, 12.0
    },
    {
        "Elon Byte ⚡", "@elonbyte",
        "Ravencoin (RVN) madenciliği çok nostaljik ve havalı. GPU'ları çalıştırın! 🦅",
        "Ravencoin (RVN) mining is nostalgic and cool. Fire up the GPUs! 🦅",
        "RVN", NewsSentiment::BULLISH, false, 24.0
    },
    {
        "Kripto Masası", "@kriptomasasi",
        "DİKKAT: Büyük bir fonun TEX varlıklarını tasfiye edeceği iddia ediliyor. (FUD / Balina Tuzağı!)",
        "WARNING: Major fund rumored to liquidate TEX holdings. (FUD / Whale Trap!)",
        "TEX", NewsSentiment::BEARISH, true, 18.0 // TERS KÖŞE: FUD yalan çıkar, balinalar fiyatı patlatır!
    },
    {
        "Prof. Blockchain", "@dr_chain",
        "Ethereum Classic (ETC) hash zorluğu sert düştü! GPU madencileri için getiri tavan yaptı.",
        "Ethereum Classic (ETC) hash difficulty dropped! Yields surged for GPU miners.",
        "ETC", NewsSentiment::BULLISH, false, 20.0
    },
    {
        "Piyasa Dedikodusu", "@crypto_rumors",
        "Asya borsalarında Bitcoin madencilik operasyonlarına kısıtlama gelebilir.",
        "Asian exchanges rumored to face new Bitcoin mining power restrictions.",
        "BTC", NewsSentiment::BEARISH, false, -14.0
    },
    {
        "FOMO Günlüğü", "@fomo_daily",
        "ALTCOİN SEZONU BAŞLADI! Bitcoin hakimiyeti eriyor, TEX ve RVN madencileri bayram ediyor!",
        "ALTCOIN SEASON IS HERE! BTC dominance falling, TEX and RVN miners celebrating!",
        "TEX", NewsSentiment::BULLISH, false, 22.0
    },
    {
        "Ayı Avcısı 🐻", "@bear_hunter",
        "Bitcoin 100K yolunda durdurulamaz! Herkes altcoinleri satıp BTC'ye geçiyor!",
        "Bitcoin to 100K is inevitable! Market dumping altcoins to buy BTC!",
        "BTC", NewsSentiment::BULLISH, true, -12.0 // TERS KÖŞE: Sahte boğa tuzağı, BTC düzeltme yer!
    },
    {
        "Madenci Bülteni", "@hashrate_daily",
        "ETHW PoW ağında madenci sayısı ikiye katlandı. Ağ zorluğu fırladı, kârlılık azaldı.",
        "ETHW network miners doubled. Difficulty skyrocketed, profitability squeezed.",
        "ETHW", NewsSentiment::BEARISH, false, -15.0
    },
    {
        "Satoshi Fanatik", "@satoshi_og",
        "Ucuz coinleri kazıp biriktirenler kazanacak. TEX ve RVN'deki arz daralması yaklaşıyor.",
        "Those mining and stacking cheap coins will win. Supply shock approaching for TEX & RVN.",
        "RVN", NewsSentiment::BULLISH, false, 18.0
    },
    {
        "Finans Gurusu", "@macro_guru",
        "Küresel enerji fiyatları artıyor: Verimsiz madenciler makinelerini kapatmak zorunda kalabilir.",
        "Global energy costs rising: Inefficient miners may be forced to shut down rigs.",
        "MARKET", NewsSentiment::BEARISH, false, -10.0
    },
    {
        "Anonim Balina 🕵️", "@insider_whale",
        "TEX geliştiricileri gizli bir yapay zeka bilgi-işlem ortaklığı imzaladı dedikodusu.",
        "Rumor: TEX dev team signed secret AI compute partnership.",
        "TEX", NewsSentiment::BULLISH, false, 25.0
    }
};

NewsManager::NewsManager()
    : m_newsTimer(10.0) // initial news arrives 15s into the game
    , m_nextNewsInterval(25.0)
{
    // Start with 2 initial historic posts so news feed is not blank
    NewsItem item1;
    item1.id = m_nextNewsId++;
    item1.author = "Bloomberg Crypto";
    item1.authorHandle = "@bloomberg_desk";
    item1.contentTR = "Küresel kripto madencilik operasyonları Teksas ve İzlanda tesislerine kayıyor. Sektörde büyüme bekleniyor.";
    item1.contentEN = "Global mining operations shifting to Texas and Iceland facilities. Industry growth forecasted.";
    item1.targetCoinId = "MARKET";
    item1.sentiment = NewsSentiment::BULLISH;
    item1.isContrarian = false;
    item1.timeAgoSeconds = 120.0;
    item1.isRead = true;
    m_news.push_back(item1);

    NewsItem item2;
    item2.id = m_nextNewsId++;
    item2.author = "CryptoWhale 🐋";
    item2.authorHandle = "@whale_trader";
    item2.contentTR = "TEX ve RVN küçük madenciler için en hızlı nakit dönüşü sağlayan pariteler olmaya devam ediyor.";
    item2.contentEN = "TEX and RVN remain highest cash turnover pairs for small GPU miners.";
    item2.targetCoinId = "TEX";
    item2.sentiment = NewsSentiment::BULLISH;
    item2.isContrarian = false;
    item2.timeAgoSeconds = 45.0;
    item2.isRead = false;
    m_news.push_back(item2);
}

void NewsManager::Update(double deltaTimeSeconds, EconomyManager& economy) {
    // Yaşlandırma
    for (auto& item : m_news) {
        item.timeAgoSeconds += deltaTimeSeconds;
    }

    if (m_toastTimer > 0.0f) {
        m_toastTimer -= static_cast<float>(deltaTimeSeconds);
    }

    m_newsTimer += deltaTimeSeconds;
    if (m_newsTimer >= m_nextNewsInterval) {
        m_newsTimer = 0.0;
        // Sonraki haber için 35 ile 55 saniye arası rastgele aralık
        static std::mt19937 intRng(1337);
        std::uniform_real_distribution<double> intDist(35.0, 55.0);
        m_nextNewsInterval = intDist(intRng);

        GenerateNextNews(economy);
    }
}

void NewsManager::GenerateNextNews(EconomyManager& economy) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, g_newsPool.size() - 1);
    const auto& t = g_newsPool[dist(rng)];

    NewsItem item;
    item.id = m_nextNewsId++;
    item.author = t.author;
    item.authorHandle = t.handle;
    item.contentTR = t.contentTR;
    item.contentEN = t.contentEN;
    item.targetCoinId = t.targetCoin;
    item.sentiment = t.sentiment;
    item.isContrarian = t.isContrarian;
    item.priceImpactPercent = t.impactPercent;
    item.timeAgoSeconds = 0.0;
    item.isRead = false;

    // Listeye başa ekle (en yeni en üstte)
    m_news.insert(m_news.begin(), item);
    if (m_news.size() > 25) {
        m_news.pop_back();
    }

    m_toastTimer = 4.0f;

    // Ekonomiye haberin gerçek piyasa etkisini uygula
    // Eğer ters köşeyse (isContrarian), analistin dediğinin tam tersi olur!
    double actualImpact = item.priceImpactPercent;
    if (item.isContrarian) {
        actualImpact = -actualImpact; // Ters köşe!
    }

    double multiplier = 1.0 + (actualImpact / 100.0);

    if (item.targetCoinId == "MARKET") {
        for (auto& coin : economy.GetCoins()) {
            double newP = coin.priceUSD * (1.0 + (actualImpact * 0.5 / 100.0));
            economy.SetCoinPrice(coin.id, newP);
        }
    } else {
        auto* coin = economy.GetCoinById(item.targetCoinId);
        if (coin) {
            double newP = coin->priceUSD * multiplier;
            economy.SetCoinPrice(coin->id, newP);
        }
    }
}

const std::vector<NewsItem>& NewsManager::GetNews() const {
    return m_news;
}

size_t NewsManager::GetUnreadCount() const {
    size_t unread = 0;
    for (const auto& item : m_news) {
        if (!item.isRead) unread++;
    }
    return unread;
}

void NewsManager::MarkAllAsRead() {
    for (auto& item : m_news) {
        if (!item.isRead) {
            item.isRead = true;
            m_totalReadCount++;
        }
    }
}

void NewsManager::MarkAsRead(int id) {
    for (auto& item : m_news) {
        if (item.id == id && !item.isRead) {
            item.isRead = true;
            m_totalReadCount++;
            break;
        }
    }
}

bool NewsManager::HasRecentToast() const {
    return m_toastTimer > 0.0f && !m_news.empty();
}

std::string NewsManager::GetLatestToastMessage(bool isTR) const {
    if (m_news.empty()) return "";
    const auto& latest = m_news.front();
    if (isTR) {
        return "[YENİ ANALİZ] " + latest.author + ": " + latest.contentTR.substr(0, 60) + "... (Açmak için [N])";
    } else {
        return "[NEW POST] " + latest.author + ": " + latest.contentEN.substr(0, 60) + "... (Press [N])";
    }
}

void NewsManager::ClearRecentToast() {
    m_toastTimer = 0.0f;
}

int NewsManager::GetTotalReadCount() const {
    return m_totalReadCount;
}

} // namespace Core
