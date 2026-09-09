#include "CryptoExchangeModal.hpp"
#include "UIFrame.hpp"
#include "../core/Localization.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace Render {

CryptoExchangeModal::CryptoExchangeModal()
    : m_isOpen(false),
      m_selectedCoinIndex(0),
      m_tradeTradePercent(1.0),
      m_btnClose(Rectangle{0, 0, 0, 0}, "X", "", Color{180, 40, 40, 255}, Color{255, 80, 80, 255}),
      m_btnMineThis(Rectangle{0, 0, 0, 0}, "BU COINI KAZ", "", Color{25, 70, 45, 255}, Color{40, 220, 120, 255}),
      m_btnPct25(Rectangle{0, 0, 0, 0}, "%25", "", Color{30, 42, 60, 255}, Color{60, 180, 240, 255}),
      m_btnPct50(Rectangle{0, 0, 0, 0}, "%50", "", Color{30, 42, 60, 255}, Color{60, 180, 240, 255}),
      m_btnPct75(Rectangle{0, 0, 0, 0}, "%75", "", Color{30, 42, 60, 255}, Color{60, 180, 240, 255}),
      m_btnPct100(Rectangle{0, 0, 0, 0}, "%100 (MAX)", "", Color{30, 42, 60, 255}, Color{60, 180, 240, 255}),
      m_btnBuy(Rectangle{0, 0, 0, 0}, "SATIN AL", "", Color{20, 90, 50, 255}, Color{45, 230, 130, 255}),
      m_btnSell(Rectangle{0, 0, 0, 0}, "SAT", "", Color{110, 35, 35, 255}, Color{255, 75, 75, 255})
{
    // 5 Coin Tab Butonu
    for (int i = 0; i < 5; ++i) {
        m_coinTabButtons.emplace_back(Rectangle{0, 0, 0, 0}, "", "", Color{22, 30, 45, 255}, Color{50, 170, 240, 255});
    }
}

void CryptoExchangeModal::Open() {
    m_isOpen = true;
}

void CryptoExchangeModal::Close() {
    m_isOpen = false;
}

bool CryptoExchangeModal::IsOpen() const {
    return m_isOpen;
}

void CryptoExchangeModal::Update(Core::EconomyManager& economy, Core::TaskManager& taskManager) {
    if (!m_isOpen) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    float modalW = std::min(960.0f, screenW - 30.0f);
    float modalH = std::min(640.0f, screenH - 40.0f);
    float modalX = (screenW - modalW) * 0.5f;
    float modalY = (screenH - modalH) * 0.5f;

    // Close button
    m_btnClose.SetBounds(Rectangle{modalX + modalW - 46.0f, modalY + 10.0f, 34.0f, 34.0f});
    if (m_btnClose.UpdateAndCheckClick()) {
        Close();
        return;
    }

    auto& coins = economy.GetCoins();
    if (coins.empty()) return;
    if (m_selectedCoinIndex >= coins.size()) {
        m_selectedCoinIndex = 0;
    }

    // Update Coin Tab Buttons
    float tabStartX = modalX + 16.0f;
    float tabY = modalY + 54.0f;
    float tabGap = 8.0f;
    float tabW = (modalW - 32.0f - (tabGap * 4.0f)) / 5.0f;
    float tabH = 36.0f;

    if (m_coinTabButtons.size() < coins.size()) {
        m_coinTabButtons.resize(coins.size(), UIButton(Rectangle{0, 0, 0, 0}, "", "", Color{22, 30, 45, 255}, Color{50, 170, 240, 255}));
    }

    for (size_t i = 0; i < coins.size(); ++i) {
        float tx = tabStartX + i * (tabW + tabGap);
        m_coinTabButtons[i].SetBounds(Rectangle{tx, tabY, tabW, tabH});
        std::string tabTitle = coins[i].symbol + "  " + economy.FormatPrice(coins[i].priceUSD);
        m_coinTabButtons[i].SetTitle(tabTitle);

        if (m_coinTabButtons[i].UpdateAndCheckClick()) {
            m_selectedCoinIndex = i;
        }
    }

    // Layout
    float contentX = modalX + 16.0f;
    float contentY = tabY + tabH + 12.0f;
    float contentW = modalW - 32.0f;
    float contentH = modalH - (contentY - modalY) - 16.0f;

    float chartW = contentW * 0.62f;
    float deskX = contentX + chartW + 12.0f;
    float deskW = contentW - chartW - 12.0f;

    // Mine This Coin button
    float mineBtnW = 160.0f;
    float mineBtnH = 30.0f;
    m_btnMineThis.SetBounds(Rectangle{contentX + chartW - mineBtnW - 10.0f, contentY + 8.0f, mineBtnW, mineBtnH});
    
    bool isCurrentlyMining = (economy.GetActiveCoinIndex() == m_selectedCoinIndex);
    if (isCurrentlyMining) {
        m_btnMineThis.SetTitle(Core::LocalizationManager::Tr("EXCHANGE_MINING_ACTIVE"));
        m_btnMineThis.SetDisabled(true);
    } else {
        m_btnMineThis.SetTitle(Core::LocalizationManager::Tr("EXCHANGE_MINE_THIS"));
        m_btnMineThis.SetDisabled(false);
    }

    if (m_btnMineThis.UpdateAndCheckClick()) {
        economy.SetActiveCoinIndex(m_selectedCoinIndex);
    }

    // Trade Desk Controls
    float pctBtnY = contentY + 110.0f;
    float pctBtnGap = 6.0f;
    float pctBtnW = (deskW - 24.0f - (pctBtnGap * 3.0f)) / 4.0f;
    float pctBtnH = 28.0f;

    m_btnPct25.SetBounds(Rectangle{deskX + 12.0f, pctBtnY, pctBtnW, pctBtnH});
    m_btnPct50.SetBounds(Rectangle{deskX + 12.0f + (pctBtnW + pctBtnGap) * 1, pctBtnY, pctBtnW, pctBtnH});
    m_btnPct75.SetBounds(Rectangle{deskX + 12.0f + (pctBtnW + pctBtnGap) * 2, pctBtnY, pctBtnW, pctBtnH});
    m_btnPct100.SetBounds(Rectangle{deskX + 12.0f + (pctBtnW + pctBtnGap) * 3, pctBtnY, pctBtnW, pctBtnH});

    if (m_btnPct25.UpdateAndCheckClick()) m_tradeTradePercent = 0.25;
    if (m_btnPct50.UpdateAndCheckClick()) m_tradeTradePercent = 0.50;
    if (m_btnPct75.UpdateAndCheckClick()) m_tradeTradePercent = 0.75;
    if (m_btnPct100.UpdateAndCheckClick()) m_tradeTradePercent = 1.00;

    // Buy & Sell Buttons
    float actionBtnY = contentY + contentH - 65.0f;
    float actionBtnW = (deskW - 32.0f) * 0.5f;
    float actionBtnH = 46.0f;

    m_btnBuy.SetBounds(Rectangle{deskX + 12.0f, actionBtnY, actionBtnW, actionBtnH});
    m_btnSell.SetBounds(Rectangle{deskX + 12.0f + actionBtnW + 8.0f, actionBtnY, actionBtnW, actionBtnH});

    const auto& selectedCoin = coins[m_selectedCoinIndex];
    double maxFiatSpend = economy.GetFiatBalance() * m_tradeTradePercent;
    double maxCoinSell = selectedCoin.balance * m_tradeTradePercent;

    m_btnBuy.SetDisabled(maxFiatSpend < 1.0 || selectedCoin.priceUSD <= 0.0);
    m_btnSell.SetDisabled(maxCoinSell <= 0.000001 || selectedCoin.priceUSD <= 0.0);

    if (m_btnBuy.UpdateAndCheckClick()) {
        if (economy.BuyCoin(m_selectedCoinIndex, maxFiatSpend)) {
            taskManager.NotifyTradeExecuted();
        }
    }

    if (m_btnSell.UpdateAndCheckClick()) {
        double revenueUSD = maxCoinSell * selectedCoin.priceUSD;
        if (economy.SellCoin(m_selectedCoinIndex, maxCoinSell)) {
            taskManager.NotifyTradeExecuted();
            taskManager.NotifyCryptoSold(revenueUSD);
        }
    }
}

void CryptoExchangeModal::Draw(const Core::EconomyManager& economy) const {
    if (!m_isOpen) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Dark backdrop
    DrawRectangle(0, 0, screenW, screenH, Color{10, 14, 22, 225});

    float modalW = std::min(960.0f, screenW - 30.0f);
    float modalH = std::min(640.0f, screenH - 40.0f);
    float modalX = (screenW - modalW) * 0.5f;
    float modalY = (screenH - modalH) * 0.5f;

    // Modal Background
    Rectangle modalRec{modalX, modalY, modalW, modalH};
    DrawRectangleRounded(modalRec, 0.03f, 8, Color{16, 22, 34, 252});
    DrawRectangleRoundedLines(modalRec, 0.03f, 8, 2.0f, Color{45, 140, 245, 230});

    // Header
    DrawHeader(modalX, modalY, modalW);

    // Coin Selector Tabs
    DrawCoinTabs(modalX, modalY, modalW, economy);

    // Content split: Chart on Left, Trade Desk on Right
    const auto& coins = economy.GetCoins();
    if (m_selectedCoinIndex < coins.size()) {
        const auto& coin = coins[m_selectedCoinIndex];

        float contentX = modalX + 16.0f;
        float contentY = modalY + 54.0f + 36.0f + 12.0f;
        float contentW = modalW - 32.0f;
        float contentH = modalH - (contentY - modalY) - 16.0f;

        float chartW = contentW * 0.62f;
        float deskX = contentX + chartW + 12.0f;
        float deskW = contentW - chartW - 12.0f;

        DrawPriceChart(contentX, contentY, chartW, contentH, coin, economy);
        DrawTradeDesk(deskX, contentY, deskW, contentH, coin, economy);
    }

    m_btnClose.Draw();
}

void CryptoExchangeModal::DrawHeader(float modalX, float modalY, float modalW) const {
    DrawRectangleRounded(Rectangle{modalX + 10.0f, modalY + 8.0f, modalW - 20.0f, 40.0f}, 0.1f, 6, Color{24, 34, 52, 255});
    DrawRectangleRoundedLines(Rectangle{modalX + 10.0f, modalY + 8.0f, modalW - 20.0f, 40.0f}, 0.1f, 6, 1.0f, Color{60, 150, 255, 120});

    const char* title = Core::LocalizationManager::Tr("EXCHANGE_TITLE");
    UIFrame::DrawTextCustom(title, modalX + 22.0f, modalY + 16.0f, 20.0f, Color{255, 215, 60, 255}, true);
}

void CryptoExchangeModal::DrawCoinTabs(float modalX, float modalY, float modalW, const Core::EconomyManager& economy) const {
    const auto& coins = economy.GetCoins();

    for (size_t i = 0; i < coins.size() && i < m_coinTabButtons.size(); ++i) {
        const auto& coin = coins[i];
        bool isSelected = (i == m_selectedCoinIndex);

        m_coinTabButtons[i].Draw();

        Rectangle b = m_coinTabButtons[i].GetBounds();
        if (isSelected) {
            DrawRectangleRoundedLines(b, 0.15f, 4, 2.0f, Color{255, 215, 50, 255});
            DrawRectangle(static_cast<int>(b.x + 4), static_cast<int>(b.y + b.height - 3), static_cast<int>(b.width - 8), 3, Color{255, 215, 50, 255});
        }

        // 24h change color indicator dot
        Color dotColor = (coin.priceChange24hPercent >= 0.0) ? Color{50, 230, 130, 255} : Color{255, 75, 75, 255};
        DrawCircle(static_cast<int>(b.x + 10), static_cast<int>(b.y + b.height * 0.5f), 3.5f, dotColor);
    }
}

void CryptoExchangeModal::DrawPriceChart(
    float chartX, float chartY, float chartW, float chartH,
    const Core::CryptoCoin& coin,
    const Core::EconomyManager& economy) const
{
    // Outer Frame
    Rectangle frameRec{chartX, chartY, chartW, chartH};
    DrawRectangleRounded(frameRec, 0.06f, 6, Color{20, 28, 44, 240});
    DrawRectangleRoundedLines(frameRec, 0.06f, 6, 1.2f, Color{45, 65, 95, 255});

    // Top Info Bar
    float infoY = chartY + 10.0f;
    std::string coinFullName = coin.name + " (" + coin.symbol + ")";
    UIFrame::DrawTextCustom(coinFullName, chartX + 16.0f, infoY, 18.0f, Color{255, 255, 255, 255}, true);

    std::string priceStr = economy.FormatPrice(coin.priceUSD);
    UIFrame::DrawTextCustom(priceStr, chartX + 16.0f, infoY + 22.0f, 24.0f, Color{255, 225, 80, 255}, true);

    // 24h Change badge
    char changeBuf[32];
    snprintf(changeBuf, sizeof(changeBuf), "%+.2f%%", coin.priceChange24hPercent);
    Color changeColor = (coin.priceChange24hPercent >= 0.0) ? Color{60, 230, 140, 255} : Color{255, 80, 80, 255};
    UIFrame::DrawTextCustom(changeBuf, chartX + 160.0f, infoY + 26.0f, 16.0f, changeColor, true);

    // Mine button
    m_btnMineThis.Draw();

    // Stats Grid
    float statsY = infoY + 54.0f;
    std::string algoStr = Core::LocalizationManager::Tr("EXCHANGE_ALGO") + coin.algorithm;
    UIFrame::DrawTextCustom(algoStr, chartX + 16.0f, statsY, 12.0f, Color{140, 165, 195, 255}, false);

    std::ostringstream ssDiff;
    ssDiff << std::fixed << std::setprecision(0) << coin.difficulty;
    std::string diffStr = Core::LocalizationManager::Tr("EXCHANGE_DIFFICULTY") + ssDiff.str();
    UIFrame::DrawTextCustom(diffStr, chartX + 16.0f, statsY + 16.0f, 12.0f, Color{140, 165, 195, 255}, false);

    std::string highStr = Core::LocalizationManager::Tr("EXCHANGE_HIGH") + economy.FormatPrice(coin.high24h);
    UIFrame::DrawTextCustom(highStr, chartX + 220.0f, statsY, 12.0f, Color{140, 220, 160, 255}, false);

    std::string lowStr = Core::LocalizationManager::Tr("EXCHANGE_LOW") + economy.FormatPrice(coin.low24h);
    UIFrame::DrawTextCustom(lowStr, chartX + 220.0f, statsY + 16.0f, 12.0f, Color{230, 140, 140, 255}, false);

    // Actual Chart Area
    float cAreaX = chartX + 14.0f;
    float cAreaY = statsY + 38.0f;
    float cAreaW = chartW - 28.0f;
    float cAreaH = chartH - (cAreaY - chartY) - 14.0f;

    DrawRectangleRounded(Rectangle{cAreaX, cAreaY, cAreaW, cAreaH}, 0.04f, 4, Color{14, 18, 28, 255});
    DrawRectangleRoundedLines(Rectangle{cAreaX, cAreaY, cAreaW, cAreaH}, 0.04f, 4, 1.0f, Color{35, 48, 70, 255});

    const auto& history = coin.priceHistory;
    if (history.size() < 2) return;

    // Calculate Min & Max in history with padding
    float minPrice = history[0];
    float maxPrice = history[0];
    for (float p : history) {
        if (p < minPrice) minPrice = p;
        if (p > maxPrice) maxPrice = p;
    }
    if (std::abs(maxPrice - minPrice) < 0.0001f) {
        minPrice *= 0.95f;
        maxPrice *= 1.05f;
    }
    float pricePadding = (maxPrice - minPrice) * 0.12f;
    minPrice -= pricePadding;
    maxPrice += pricePadding;

    // Grid lines (3 horizontal lines)
    for (int g = 0; g <= 3; ++g) {
        float ratio = static_cast<float>(g) / 3.0f;
        float gy = cAreaY + cAreaH - (cAreaH * ratio);
        DrawLine(static_cast<int>(cAreaX + 6), static_cast<int>(gy), static_cast<int>(cAreaX + cAreaW - 6), static_cast<int>(gy), Color{28, 38, 55, 200});

        float gridPrice = minPrice + (maxPrice - minPrice) * ratio;
        std::string gpStr = economy.FormatPrice(gridPrice);
        UIFrame::DrawTextCustom(gpStr, cAreaX + cAreaW - 68.0f, gy - 7.0f, 10.0f, Color{100, 125, 155, 200}, false);
    }

    // Transform points to screen coords
    std::vector<Vector2> screenPoints;
    screenPoints.reserve(history.size());
    float stepX = (cAreaW - 74.0f) / static_cast<float>(history.size() - 1);

    for (size_t i = 0; i < history.size(); ++i) {
        float px = cAreaX + 10.0f + (static_cast<float>(i) * stepX);
        float normY = (history[i] - minPrice) / (maxPrice - minPrice);
        float py = cAreaY + cAreaH - 12.0f - (normY * (cAreaH - 24.0f));
        screenPoints.push_back(Vector2{px, py});
    }

    // Chart Area Gradient Fill
    Color themeColor = (coin.priceChange24hPercent >= 0.0) ? Color{40, 210, 130, 255} : Color{250, 75, 75, 255};
    Color themeFillTop = Color{themeColor.r, themeColor.g, themeColor.b, 65};
    Color themeFillBottom = Color{themeColor.r, themeColor.g, themeColor.b, 5};

    for (size_t i = 0; i + 1 < screenPoints.size(); ++i) {
        Vector2 p1 = screenPoints[i];
        Vector2 p2 = screenPoints[i + 1];
        float baseY = cAreaY + cAreaH - 4.0f;

        // Draw quad as two triangles
        DrawTriangle(Vector2{p1.x, baseY}, p1, p2, themeFillTop);
        DrawTriangle(Vector2{p1.x, baseY}, p2, Vector2{p2.x, baseY}, themeFillBottom);
    }

    // Top Glowing Line
    for (size_t i = 0; i + 1 < screenPoints.size(); ++i) {
        DrawLineEx(screenPoints[i], screenPoints[i + 1], 2.2f, themeColor);
    }

    // Interactive Hover Point
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, Rectangle{cAreaX, cAreaY, cAreaW - 70.0f, cAreaH})) {
        // Find closest point
        size_t closestIdx = 0;
        float minDist = 99999.0f;
        for (size_t i = 0; i < screenPoints.size(); ++i) {
            float dist = std::abs(screenPoints[i].x - mousePos.x);
            if (dist < minDist) {
                minDist = dist;
                closestIdx = i;
            }
        }

        Vector2 pt = screenPoints[closestIdx];
        DrawLine(static_cast<int>(pt.x), static_cast<int>(cAreaY), static_cast<int>(pt.x), static_cast<int>(cAreaY + cAreaH), Color{200, 220, 255, 90});
        DrawCircle(static_cast<int>(pt.x), static_cast<int>(pt.y), 5.0f, Color{255, 255, 255, 255});
        DrawCircle(static_cast<int>(pt.x), static_cast<int>(pt.y), 8.0f, Color{themeColor.r, themeColor.g, themeColor.b, 120});

        // Hover price tooltip
        std::string tipPrice = economy.FormatPrice(history[closestIdx]);
        float tw = UIFrame::MeasureTextCustom(tipPrice, 12.0f, true) + 12.0f;
        float tipX = std::clamp(pt.x - (tw * 0.5f), cAreaX + 5.0f, cAreaX + cAreaW - tw - 5.0f);
        float tipY = std::max(cAreaY + 5.0f, pt.y - 24.0f);
        DrawRectangleRounded(Rectangle{tipX, tipY, tw, 18.0f}, 0.3f, 4, Color{25, 35, 50, 240});
        DrawRectangleRoundedLines(Rectangle{tipX, tipY, tw, 18.0f}, 0.3f, 4, 1.0f, themeColor);
        UIFrame::DrawTextCustom(tipPrice, tipX + 6.0f, tipY + 2.0f, 12.0f, Color{255, 255, 255, 255}, true);
    }
}

void CryptoExchangeModal::DrawTradeDesk(
    float deskX, float deskY, float deskW, float deskH,
    const Core::CryptoCoin& coin,
    const Core::EconomyManager& economy) const
{
    // Desk Container Frame
    Rectangle deskRec{deskX, deskY, deskW, deskH};
    DrawRectangleRounded(deskRec, 0.06f, 6, Color{22, 30, 48, 245});
    DrawRectangleRoundedLines(deskRec, 0.06f, 6, 1.2f, Color{55, 75, 110, 255});

    // Desk Header
    DrawRectangleRounded(Rectangle{deskX + 8.0f, deskY + 8.0f, deskW - 16.0f, 32.0f}, 0.15f, 4, Color{30, 40, 65, 255});
    UIFrame::DrawTextCustom("HIZLI ALIM & SATIM", deskX + 16.0f, deskY + 14.0f, 15.0f, Color{255, 220, 90, 255}, true);

    // Current Balances Panel
    float balY = deskY + 48.0f;
    UIFrame::DrawTextCustom("Cuzdan Bakiyesi:", deskX + 14.0f, balY, 13.0f, Color{150, 175, 205, 255}, false);

    std::ostringstream ssCoinBal;
    ssCoinBal << std::fixed << std::setprecision(6) << coin.balance << " " << coin.symbol;
    UIFrame::DrawTextCustom(ssCoinBal.str(), deskX + 14.0f, balY + 18.0f, 16.0f, Color{240, 248, 255, 255}, true);

    double coinUSDVal = coin.balance * coin.priceUSD;
    std::string valUSDStr = "≈ " + economy.FormatFiat(coinUSDVal);
    UIFrame::DrawTextCustom(valUSDStr, deskX + 14.0f, balY + 38.0f, 13.0f, Color{80, 220, 140, 255}, false);

    // Quick Volume % Buttons
    m_btnPct25.Draw();
    m_btnPct50.Draw();
    m_btnPct75.Draw();
    m_btnPct100.Draw();

    // Highlight selected percent
    UIButton* activePctBtn = nullptr;
    if (std::abs(m_tradeTradePercent - 0.25) < 0.01) activePctBtn = const_cast<UIButton*>(&m_btnPct25);
    else if (std::abs(m_tradeTradePercent - 0.50) < 0.01) activePctBtn = const_cast<UIButton*>(&m_btnPct50);
    else if (std::abs(m_tradeTradePercent - 0.75) < 0.01) activePctBtn = const_cast<UIButton*>(&m_btnPct75);
    else if (std::abs(m_tradeTradePercent - 1.00) < 0.01) activePctBtn = const_cast<UIButton*>(&m_btnPct100);

    if (activePctBtn) {
        Rectangle pb = activePctBtn->GetBounds();
        DrawRectangleRoundedLines(pb, 0.2f, 4, 1.5f, Color{255, 215, 60, 255});
    }

    // Trade Previews
    float calcY = deskY + 150.0f;
    double maxFiatSpend = economy.GetFiatBalance() * m_tradeTradePercent;
    double maxCoinSell = coin.balance * m_tradeTradePercent;

    // 1. Buy Preview Card
    Rectangle buyPrevRec{deskX + 12.0f, calcY, deskW - 24.0f, 66.0f};
    DrawRectangleRounded(buyPrevRec, 0.12f, 4, Color{18, 28, 24, 230});
    DrawRectangleRoundedLines(buyPrevRec, 0.12f, 4, 1.0f, Color{40, 120, 75, 180});

    UIFrame::DrawTextCustom("ALIM HESAPLAMASI", buyPrevRec.x + 10.0f, buyPrevRec.y + 8.0f, 11.0f, Color{60, 210, 130, 255}, true);
    std::string spendStr = "Nakit: " + economy.FormatFiat(maxFiatSpend);
    UIFrame::DrawTextCustom(spendStr, buyPrevRec.x + 10.0f, buyPrevRec.y + 24.0f, 13.0f, Color{200, 230, 215, 255}, false);

    double estCoinsBought = (coin.priceUSD > 0.0) ? (maxFiatSpend / coin.priceUSD) : 0.0;
    std::ostringstream ssEstBuy;
    ssEstBuy << "+ " << std::fixed << std::setprecision(6) << estCoinsBought << " " << coin.symbol;
    UIFrame::DrawTextCustom(ssEstBuy.str(), buyPrevRec.x + 10.0f, buyPrevRec.y + 42.0f, 14.0f, Color{80, 255, 150, 255}, true);

    // 2. Sell Preview Card
    Rectangle sellPrevRec{deskX + 12.0f, calcY + 74.0f, deskW - 24.0f, 66.0f};
    DrawRectangleRounded(sellPrevRec, 0.12f, 4, Color{28, 20, 20, 230});
    DrawRectangleRoundedLines(sellPrevRec, 0.12f, 4, 1.0f, Color{130, 50, 50, 180});

    UIFrame::DrawTextCustom("SATIM HESAPLAMASI", sellPrevRec.x + 10.0f, sellPrevRec.y + 8.0f, 11.0f, Color{245, 90, 90, 255}, true);
    std::ostringstream ssSellAmt;
    ssSellAmt << "- " << std::fixed << std::setprecision(6) << maxCoinSell << " " << coin.symbol;
    UIFrame::DrawTextCustom(ssSellAmt.str(), sellPrevRec.x + 10.0f, sellPrevRec.y + 24.0f, 13.0f, Color{240, 195, 195, 255}, false);

    double estRevenue = maxCoinSell * coin.priceUSD;
    std::string revStr = "+ " + economy.FormatFiat(estRevenue);
    UIFrame::DrawTextCustom(revStr, sellPrevRec.x + 10.0f, sellPrevRec.y + 42.0f, 14.0f, Color{255, 215, 80, 255}, true);

    // Total Portfolio Summary
    float portY = calcY + 150.0f;
    std::string portLabel = Core::LocalizationManager::Tr("EXCHANGE_PORTFOLIO");
    UIFrame::DrawTextCustom(portLabel, deskX + 14.0f, portY, 12.0f, Color{140, 170, 200, 255}, false);

    double totalPortVal = economy.GetTotalPortfolioValueUSD();
    std::string portStr = economy.FormatFiat(totalPortVal);
    UIFrame::DrawTextCustom(portStr, deskX + 14.0f, portY + 16.0f, 18.0f, Color{255, 230, 90, 255}, true);

    // Action Buttons
    m_btnBuy.Draw();
    m_btnSell.Draw();
}

} // namespace Render
