#include "NewsModal.hpp"
#include "UIFrame.hpp"
#include "../core/Localization.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace Render {

NewsModal::NewsModal()
    : m_isOpen(false),
      m_btnClose(Rectangle{0, 0, 0, 0}, "X", "", Color{180, 40, 40, 255}, Color{255, 80, 80, 255}),
      m_btnMarkAllRead(Rectangle{0, 0, 0, 0}, "TUMUNU OKUNDU YAP", "", Color{45, 95, 170, 255}, Color{65, 135, 240, 255})
{
}

void NewsModal::Open() {
    m_isOpen = true;
    m_scrollOffset = 0.0f;
}

void NewsModal::Close() {
    m_isOpen = false;
}

bool NewsModal::IsOpen() const {
    return m_isOpen;
}

void NewsModal::Update(Core::NewsManager& newsManager, Core::TaskManager& taskManager) {
    if (!m_isOpen) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    float modalW = std::min(880.0f, screenW - 40.0f);
    float modalH = std::min(640.0f, screenH - 60.0f);
    float modalX = (screenW - modalW) * 0.5f;
    float modalY = (screenH - modalH) * 0.5f;

    // Close button
    m_btnClose.SetBounds(Rectangle{modalX + modalW - 45.0f, modalY + 12.0f, 32.0f, 32.0f});
    if (m_btnClose.UpdateAndCheckClick() || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_N)) {
        Close();
        return;
    }

    // Mark all as read button
    bool isTR = (Core::LocalizationManager::Get().GetLanguage() == Core::Language::TURKISH);
    m_btnMarkAllRead.SetTitle(isTR ? "TUMUNU OKUNDU SAY" : "MARK ALL READ");
    m_btnMarkAllRead.SetBounds(Rectangle{modalX + modalW - 250.0f, modalY + 14.0f, 195.0f, 30.0f});
    if (m_btnMarkAllRead.UpdateAndCheckClick()) {
        newsManager.MarkAllAsRead();
        taskManager.NotifyNewsRead();
    }

    const auto& news = newsManager.GetNews();

    // Mouse wheel scroll handling
    Vector2 mousePos = GetMousePosition();
    Rectangle contentArea{modalX + 20.0f, modalY + 70.0f, modalW - 40.0f, modalH - 90.0f};

    if (CheckCollisionPointRec(mousePos, contentArea)) {
        float wheel = GetMouseWheelMove();
        m_scrollOffset += wheel * 36.0f;
    }

    float itemH = 92.0f;
    float gap = 10.0f;
    float totalContentH = news.size() * (itemH + gap);
    float maxScroll = std::max(0.0f, totalContentH - contentArea.height);
    m_scrollOffset = std::clamp(m_scrollOffset, -maxScroll, 0.0f);

    // Click news to mark single item as read
    if (CheckCollisionPointRec(mousePos, contentArea) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (size_t i = 0; i < news.size(); ++i) {
            float rowY = contentArea.y + m_scrollOffset + i * (itemH + gap);
            Rectangle rowRec{contentArea.x, rowY, contentArea.width, itemH};
            if (CheckCollisionPointRec(mousePos, rowRec)) {
                if (!news[i].isRead) {
                    newsManager.MarkAsRead(news[i].id);
                    taskManager.NotifyNewsRead();
                }
                break;
            }
        }
    }
}

void NewsModal::Draw(const Core::NewsManager& newsManager) const {
    if (!m_isOpen) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Dark semi-transparent backdrop
    DrawRectangle(0, 0, screenW, screenH, Color{10, 12, 18, 220});

    float modalW = std::min(880.0f, screenW - 40.0f);
    float modalH = std::min(640.0f, screenH - 60.0f);
    float modalX = (screenW - modalW) * 0.5f;
    float modalY = (screenH - modalH) * 0.5f;

    // Modal Outer Frame
    Rectangle modalRec{modalX, modalY, modalW, modalH};
    DrawRectangleRounded(modalRec, 0.03f, 8, Color{18, 22, 34, 252});
    DrawRectangleRoundedLines(modalRec, 0.03f, 8, 2.0f, Color{70, 150, 255, 230});

    // Top Header Banner
    DrawRectangleRounded(Rectangle{modalX + 10.0f, modalY + 10.0f, modalW - 20.0f, 48.0f}, 0.08f, 6, Color{25, 33, 50, 255});
    DrawRectangleRoundedLines(Rectangle{modalX + 10.0f, modalY + 10.0f, modalW - 20.0f, 48.0f}, 0.08f, 6, 1.0f, Color{90, 160, 255, 120});

    bool isTR = (Core::LocalizationManager::Get().GetLanguage() == Core::Language::TURKISH);
    const char* titleText = isTR ? "KRIPTO UZMANLARI VE HABER AKISI" : "CRYPTO ANALYSTS & MARKET NEWS";
    UIFrame::DrawTextCustom(titleText, modalX + 25.0f, modalY + 20.0f, 21.0f, Color{255, 215, 60, 255}, true);

    size_t unreadCount = newsManager.GetUnreadCount();
    if (unreadCount > 0) {
        char badgeBuf[48];
        snprintf(badgeBuf, sizeof(badgeBuf), "%zu %s", unreadCount, isTR ? "YENI" : "NEW");
        float badgeW = UIFrame::MeasureTextCustom(badgeBuf, 13.0f, true) + 18.0f;
        Rectangle badgeRec{modalX + 380.0f, modalY + 19.0f, badgeW, 26.0f};
        DrawRectangleRounded(badgeRec, 0.4f, 4, Color{245, 65, 80, 255});
        UIFrame::DrawTextCustom(badgeBuf, badgeRec.x + 8.0f, badgeRec.y + 5.0f, 13.0f, Color{255, 255, 255, 255}, true);
    }

    // Mark all as read button & Close button
    m_btnMarkAllRead.Draw();
    m_btnClose.Draw();

    // Content Area with Scissor
    Rectangle contentArea{modalX + 20.0f, modalY + 70.0f, modalW - 40.0f, modalH - 90.0f};
    BeginScissorMode((int)contentArea.x, (int)contentArea.y, (int)contentArea.width, (int)contentArea.height);

    const auto& news = newsManager.GetNews();
    float itemH = 92.0f;
    float gap = 10.0f;

    for (size_t i = 0; i < news.size(); ++i) {
        const auto& item = news[i];
        float rowY = contentArea.y + m_scrollOffset + i * (itemH + gap);

        // Cull off-screen items
        if (rowY + itemH < contentArea.y || rowY > contentArea.y + contentArea.height) {
            continue;
        }

        Rectangle rowRec{contentArea.x, rowY, contentArea.width, itemH};

        // Card Styling: glow if unread
        Color cardBg = item.isRead ? Color{22, 28, 42, 240} : Color{28, 38, 62, 255};
        Color cardBorder = item.isRead ? Color{45, 60, 85, 220} : Color{80, 160, 255, 230};

        DrawRectangleRounded(rowRec, 0.08f, 6, cardBg);
        DrawRectangleRoundedLines(rowRec, 0.08f, 6, item.isRead ? 1.0f : 2.0f, cardBorder);

        // Unread Indicator dot
        if (!item.isRead) {
            DrawCircle((int)(rowRec.x + 14.0f), (int)(rowRec.y + 20.0f), 4.5f, Color{65, 220, 255, 255});
        }

        // Author Name & Handle
        std::string authorStr = item.author + " (" + item.authorHandle + ")";
        UIFrame::DrawTextCustom(authorStr.c_str(), rowRec.x + 28.0f, rowRec.y + 12.0f, 15.0f, Color{80, 200, 255, 255}, true);

        // Sentiment & Coin Badges
        Color sentColor = Color{40, 225, 120, 255};
        std::string sentStr = isTR ? "BOGA / YUKSELIS" : "BULLISH";
        if (item.sentiment == Core::NewsSentiment::BEARISH) {
            sentColor = Color{255, 75, 85, 255};
            sentStr = isTR ? "AYI / DUSUS" : "BEARISH";
        } else if (item.sentiment == Core::NewsSentiment::RUMOR) {
            sentColor = Color{255, 185, 50, 255};
            sentStr = isTR ? "FISILTI / TUZAK?" : "RUMOR / TRAP?";
        }

        float sentW = UIFrame::MeasureTextCustom(sentStr.c_str(), 12.0f, true) + 16.0f;
        Rectangle sentRec{rowRec.x + rowRec.width - sentW - 14.0f, rowRec.y + 12.0f, sentW, 22.0f};
        DrawRectangleRounded(sentRec, 0.35f, 4, Color{sentColor.r, sentColor.g, sentColor.b, 45});
        DrawRectangleRoundedLines(sentRec, 0.35f, 4, 1.0f, sentColor);
        UIFrame::DrawTextCustom(sentStr.c_str(), sentRec.x + 8.0f, sentRec.y + 4.0f, 12.0f, sentColor, true);

        // Coin Target Badge
        if (!item.targetCoinId.empty()) {
            std::string coinTag = "[" + item.targetCoinId + "]";
            float coinW = UIFrame::MeasureTextCustom(coinTag.c_str(), 12.0f, true) + 12.0f;
            Rectangle coinRec{sentRec.x - coinW - 8.0f, rowRec.y + 12.0f, coinW, 22.0f};
            DrawRectangleRounded(coinRec, 0.35f, 4, Color{35, 48, 70, 255});
            DrawRectangleRoundedLines(coinRec, 0.35f, 4, 1.0f, Color{100, 140, 190, 200});
            UIFrame::DrawTextCustom(coinTag.c_str(), coinRec.x + 6.0f, coinRec.y + 4.0f, 12.0f, Color{220, 235, 255, 255}, true);
        }

        // News Content
        const std::string& content = isTR ? item.contentTR : item.contentEN;
        UIFrame::DrawTextCustom(content.c_str(), rowRec.x + 28.0f, rowRec.y + 38.0f, 14.0f, Color{230, 240, 255, 255}, false);

        // Time ago
        char timeBuf[48];
        int mins = static_cast<int>(item.timeAgoSeconds / 60.0);
        if (mins < 1) {
            snprintf(timeBuf, sizeof(timeBuf), "%s", isTR ? "Az once" : "Just now");
        } else {
            snprintf(timeBuf, sizeof(timeBuf), isTR ? "%d dk once" : "%d min ago", mins);
        }
        UIFrame::DrawTextCustom(timeBuf, rowRec.x + 28.0f, rowRec.y + 68.0f, 12.0f, Color{130, 150, 175, 255}, false);
    }

    EndScissorMode();
}

} // namespace Render
