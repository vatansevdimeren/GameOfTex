#pragma once

#include <string>
#include <vector>
#include <raylib.h>

namespace Core {

class EconomyManager;

enum class NewsSentiment {
    BULLISH,  // Yeşil Boğa
    BEARISH,  // Kırmızı Ayı
    RUMOR     // Sarı Dedikodu / Spekülasyon
};

/**
 * @struct NewsItem
 * @brief Represents a crypto news headline or social media analyst post.
 */
struct NewsItem {
    int id{0};
    std::string author;          // e.g. "@CryptoWhale", "Bloomberg Crypto Desk"
    std::string authorHandle;    // e.g. "@whale_trader"
    std::string contentTR;
    std::string contentEN;
    std::string targetCoinId;    // "TEX", "RVN", "ETC", "ETHW", "BTC", "MARKET"
    NewsSentiment sentiment{NewsSentiment::BULLISH};
    bool isContrarian{false};    // If true, reality is OPPOSITE of analyst claim! (Fake news / Whale Trap)
    double priceImpactPercent{0.0};
    double timeAgoSeconds{0.0};
    bool isRead{false};
};

/**
 * @class NewsManager
 * @brief Manages the live stream of fake analyst predictions, whale rumors, and market news.
 * 
 * SRP: Solely responsible for news generation, sentiment tracking, and applying news impact to economy.
 */
class NewsManager {
public:
    NewsManager();

    void Update(double deltaTimeSeconds, EconomyManager& economy);

    [[nodiscard]] const std::vector<NewsItem>& GetNews() const;
    [[nodiscard]] size_t GetUnreadCount() const;
    void MarkAllAsRead();
    void MarkAsRead(int id);

    // Toast bildirimi (ekranda beliren son haber uyarısı)
    [[nodiscard]] bool HasRecentToast() const;
    [[nodiscard]] std::string GetLatestToastMessage(bool isTR) const;
    void ClearRecentToast();

    // Toplam okunan haber sayısı (görevler için)
    [[nodiscard]] int GetTotalReadCount() const;

private:
    void GenerateNextNews(EconomyManager& economy);

    std::vector<NewsItem> m_news;
    int m_nextNewsId{1};
    double m_newsTimer{0.0};
    double m_nextNewsInterval{35.0}; // seconds between news posts
    float m_toastTimer{0.0f};
    int m_totalReadCount{0};
};

} // namespace Core
