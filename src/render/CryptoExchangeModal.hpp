#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include "UIButton.hpp"
#include "../core/EconomyManager.hpp"
#include "../core/TaskManager.hpp"

namespace Render {

/**
 * @class CryptoExchangeModal
 * @brief Interactive Cryptocurrency Exchange and Live Price Chart Desk.
 * 
 * Features:
 * - Real-time smooth neon area/line price chart with dynamic scaling and grid lines.
 * - Multi-coin tabs (TEX, RVN, ETC, ETHW, BTC) with live tickers.
 * - Mining coin selector (switch active coin to mine).
 * - Instant Trading Desk with %25, %50, %75, %100 quick volume buttons.
 */
class CryptoExchangeModal {
public:
    CryptoExchangeModal();

    void Open();
    void Close();
    [[nodiscard]] bool IsOpen() const;

    void Update(Core::EconomyManager& economy, Core::TaskManager& taskManager);
    void Draw(const Core::EconomyManager& economy) const;

private:
    void DrawHeader(float modalX, float modalY, float modalW) const;
    void DrawCoinTabs(float modalX, float modalY, float modalW, const Core::EconomyManager& economy) const;
    void DrawPriceChart(float chartX, float chartY, float chartW, float chartH, const Core::CryptoCoin& coin, const Core::EconomyManager& economy) const;
    void DrawTradeDesk(float deskX, float deskY, float deskW, float deskH, const Core::CryptoCoin& coin, const Core::EconomyManager& economy) const;

    bool m_isOpen{false};
    size_t m_selectedCoinIndex{0};
    double m_tradeTradePercent{1.0}; // 0.25, 0.50, 0.75, 1.0

    UIButton m_btnClose;
    std::vector<UIButton> m_coinTabButtons;
    UIButton m_btnMineThis;

    // Quick % buttons
    UIButton m_btnPct25;
    UIButton m_btnPct50;
    UIButton m_btnPct75;
    UIButton m_btnPct100;

    UIButton m_btnBuy;
    UIButton m_btnSell;
};

} // namespace Render
