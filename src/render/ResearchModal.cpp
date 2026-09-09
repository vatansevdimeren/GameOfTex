#include "ResearchModal.hpp"
#include "UIFrame.hpp"
#include "../core/Localization.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace Render {

ResearchModal::ResearchModal()
    : m_btnClose(Rectangle{0, 0, 10, 10}, "KAPAT", "ESC", Color{60, 25, 30, 255}, Color{255, 75, 75, 255})
    , m_btnTabTech(Rectangle{0, 0, 10, 10}, "TEKNOLOJI AGACI", "5 Pasif Carpan", Color{25, 45, 65, 255}, Color{0, 220, 255, 255})
    , m_btnTabIPO(Rectangle{0, 0, 10, 10}, "SIRKET HALKA ARZI (IPO)", "Prestige & Venture", Color{45, 40, 20, 255}, Color{255, 215, 0, 255})
    , m_btnExecuteIPO(Rectangle{0, 0, 10, 10}, "HALKA ARZ ET VE HİSSELERI AL", "Sonsuz Carpan Donemi", Color{70, 50, 15, 255}, Color{255, 215, 0, 255})
{
    m_upgradeButtons.resize(5);
}

void ResearchModal::Open() {
    m_isOpen = true;
}

void ResearchModal::Close() {
    m_isOpen = false;
}

bool ResearchModal::IsOpen() const {
    return m_isOpen;
}

void ResearchModal::Update(Core::ResearchManager& research, Core::EconomyManager& economy, Core::Warehouse& warehouse) {
    if (!m_isOpen) return;

    if (IsKeyPressed(KEY_ESCAPE)) {
        Close();
        return;
    }

    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    const float modalW = std::clamp(screenW * 0.72f, 760.0f, 1000.0f);
    const float modalH = std::clamp(screenH * 0.84f, 580.0f, 750.0f);
    const float modalX = (screenW - modalW) * 0.5f;
    const float modalY = (screenH - modalH) * 0.5f;

    // Kapat butonu
    m_btnClose.SetBounds(Rectangle{modalX + modalW - 110.0f, modalY + 16.0f, 94.0f, 36.0f});
    if (m_btnClose.UpdateAndCheckClick()) {
        Close();
        return;
    }

    // Sekme Butonları
    const float tabW = 240.0f;
    const float tabH = 38.0f;
    const float tabY = modalY + 58.0f;
    m_btnTabTech.SetBounds(Rectangle{modalX + 30.0f, tabY, tabW, tabH});
    m_btnTabIPO.SetBounds(Rectangle{modalX + 30.0f + tabW + 12.0f, tabY, tabW, tabH});

    if (m_btnTabTech.UpdateAndCheckClick()) m_activeTab = 0;
    if (m_btnTabIPO.UpdateAndCheckClick()) m_activeTab = 1;

    // Sekme 0: Teknoloji Ağacı Güncellemeleri
    if (m_activeTab == 0) {
        const auto& techs = research.GetTechnologies();
        float cardY = tabY + tabH + 16.0f;
        float cardH = 78.0f;
        float cardGap = 8.0f;

        for (size_t i = 0; i < techs.size() && i < m_upgradeButtons.size(); ++i) {
            const auto& t = techs[i];
            float itemY = cardY + (i * (cardH + cardGap));
            float btnW = 180.0f;
            float btnH = 50.0f;
            m_upgradeButtons[i].SetBounds(Rectangle{modalX + modalW - btnW - 40.0f, itemY + 14.0f, btnW, btnH});

            bool isMax = (t.currentLevel >= t.maxLevel);
            double cost = research.GetTechCost(t.type);

            if (isMax) {
                m_upgradeButtons[i].SetTitle("MAX SEVIYE");
                m_upgradeButtons[i].SetSubtitle("Tamamlandi");
                m_upgradeButtons[i].SetDisabled(true);
            } else {
                m_upgradeButtons[i].SetTitle("GELISTIR (LVL " + std::to_string(t.currentLevel + 1) + ")");
                m_upgradeButtons[i].SetSubtitle(economy.FormatFiat(cost));
                m_upgradeButtons[i].SetDisabled(economy.GetFiatBalance() < cost);

                if (m_upgradeButtons[i].UpdateAndCheckClick()) {
                    research.UpgradeTech(t.type, economy);
                }
            }
        }
    }
    // Sekme 1: Şirket Halka Arzı (Prestige IPO)
    else if (m_activeTab == 1) {
        double netWorth = economy.GetFiatBalance() + economy.GetTotalPortfolioValueUSD();
        bool canIPO = research.CanExecuteIPO(netWorth);
        int claimable = research.CalculateClaimableShares(netWorth);

        float btnW = 340.0f;
        float btnH = 60.0f;
        m_btnExecuteIPO.SetBounds(Rectangle{modalX + (modalW - btnW) * 0.5f, modalY + modalH - 85.0f, btnW, btnH});
        m_btnExecuteIPO.SetDisabled(!canIPO);

        if (canIPO) {
            m_btnExecuteIPO.SetTitle("HALKA ARZ ET (+" + std::to_string(claimable) + " HISSE)");
            m_btnExecuteIPO.SetSubtitle("Girisim Sermayesi ile Bastan Basla");
            if (m_btnExecuteIPO.UpdateAndCheckClick()) {
                if (research.ExecuteIPO(netWorth, economy, warehouse)) {
                    Close();
                }
            }
        } else {
            m_btnExecuteIPO.SetTitle("GEREKLI: $50,000 SIRKET DEGERI");
            m_btnExecuteIPO.SetSubtitle("Mevcut: " + economy.FormatFiat(netWorth));
        }
    }
}

void ResearchModal::Draw(const Core::ResearchManager& research, const Core::EconomyManager& economy) const {
    if (!m_isOpen) return;

    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    const float modalW = std::clamp(screenW * 0.72f, 760.0f, 1000.0f);
    const float modalH = std::clamp(screenH * 0.84f, 580.0f, 750.0f);
    const float modalX = (screenW - modalW) * 0.5f;
    const float modalY = (screenH - modalH) * 0.5f;

    // Arka plan karartma
    DrawRectangle(0, 0, static_cast<int>(screenW), static_cast<int>(screenH), Color{0, 0, 0, 195});

    // Ana Modal Kartı
    UIFrame::DrawCard(Rectangle{modalX, modalY, modalW, modalH},
                      "AR-GE VE TEKNOLOJI LABORATUVARI (RESEARCH & VENTURE IPO)",
                      Color{0, 220, 255, 255});

    m_btnClose.Draw();
    m_btnTabTech.Draw();
    m_btnTabIPO.Draw();

    bool isTR = (Core::LocalizationManager::Get().GetLanguage() == Core::Language::TURKISH);

    // Sekme 0: Teknoloji Ağacı
    if (m_activeTab == 0) {
        const auto& techs = research.GetTechnologies();
        float cardY = modalY + 112.0f;
        float cardH = 78.0f;
        float cardGap = 8.0f;
        float innerW = modalW - 60.0f;

        for (size_t i = 0; i < techs.size(); ++i) {
            const auto& t = techs[i];
            float itemY = cardY + (i * (cardH + cardGap));

            Rectangle itemRect{modalX + 30.0f, itemY, innerW, cardH};
            DrawRectangleRounded(itemRect, 0.15f, 4, Color{22, 28, 38, 240});
            DrawRectangleRoundedLines(itemRect, 0.15f, 4, 1.2f, (t.currentLevel > 0) ? Color{0, 220, 255, 200} : Color{50, 60, 80, 200});

            // Teknoloji Adı ve Seviye
            std::string name = isTR ? t.nameTr : t.nameEn;
            UIFrame::DrawTextCustom(name, itemRect.x + 16.0f, itemRect.y + 12.0f, 16.5f, WHITE, true);

            // Seviye Göstergesi
            std::string lvlStr = "[LVL " + std::to_string(t.currentLevel) + " / " + std::to_string(t.maxLevel) + "]";
            Color lvlCol = (t.currentLevel >= t.maxLevel) ? Color{255, 215, 0, 255} : Color{0, 240, 160, 255};
            UIFrame::DrawTextCustom(lvlStr, itemRect.x + 16.0f + UIFrame::MeasureTextCustom(name, 16.5f, true) + 12.0f, itemRect.y + 12.0f, 15.0f, lvlCol, true);

            // Açıklama
            std::string desc = isTR ? t.descTr : t.descEn;
            UIFrame::DrawTextCustom(desc, itemRect.x + 16.0f, itemRect.y + 40.0f, 13.0f, Color{150, 170, 200, 255}, false);

            if (i < m_upgradeButtons.size()) {
                m_upgradeButtons[i].Draw();
            }
        }
    }
    // Sekme 1: Şirket Halka Arzı (Prestige IPO)
    else if (m_activeTab == 1) {
        float innerX = modalX + 40.0f;
        float innerY = modalY + 115.0f;
        float innerW = modalW - 80.0f;

        double netWorth = economy.GetFiatBalance() + economy.GetTotalPortfolioValueUSD();
        int shares = research.GetVentureShares();
        int claimable = research.CalculateClaimableShares(netWorth);
        double currentMult = research.GetPrestigeMultiplier();

        // 1. Durum Kartı
        Rectangle statCard{innerX, innerY, innerW, 100.0f};
        DrawRectangleRounded(statCard, 0.15f, 4, Color{26, 34, 48, 240});
        DrawRectangleRoundedLines(statCard, 0.15f, 4, 1.5f, Color{255, 200, 0, 255});

        std::string s1 = isTR ? "TOPLAM SIRKET DEGERI:" : "TOTAL COMPANY NET WORTH:";
        std::string s1v = economy.FormatFiat(netWorth);
        UIFrame::DrawTextCustom(s1, innerX + 20.0f, innerY + 18.0f, 14.0f, Color{180, 200, 230, 255}, false);
        UIFrame::DrawTextCustom(s1v, innerX + 20.0f, innerY + 44.0f, 24.0f, Color{0, 255, 150, 255}, true);

        std::string s2 = isTR ? "SAHIP OLUNAN GIRISIM HISSELERI:" : "OWNED VENTURE SHARES:";
        std::string s2v = std::to_string(shares) + (isTR ? " HISSE (x" : " SHARES (x") + std::to_string(currentMult).substr(0, 4) + (isTR ? " Kalici Carpan)" : " Permanent Mult)");
        UIFrame::DrawTextCustom(s2, innerX + innerW * 0.50f, innerY + 18.0f, 14.0f, Color{180, 200, 230, 255}, false);
        UIFrame::DrawTextCustom(s2v, innerX + innerW * 0.50f, innerY + 44.0f, 22.0f, Color{255, 215, 0, 255}, true);

        // 2. Açıklama ve Kurallar Paneli
        Rectangle infoCard{innerX, innerY + 115.0f, innerW, 230.0f};
        DrawRectangleRounded(infoCard, 0.15f, 4, Color{18, 22, 32, 240});
        DrawRectangleRoundedLines(infoCard, 0.15f, 4, 1.2f, Color{60, 75, 100, 255});

        UIFrame::DrawTextCustom(isTR ? "[!] SILIKON VADISI GIRISIM SERMAYESI & IPO NEDIR?" : "[!] WHAT IS VENTURE IPO & PRESTIGE?",
                                innerX + 20.0f, innerY + 130.0f, 17.0f, Color{255, 215, 0, 255}, true);

        const char* linesTr[] = {
            "* Sirketinizi en az $50,000 net portfoye ulastirdiginizda Halka Arz (IPO) acilir.",
            "* Halka arz yapildiginda: Kasalariniz, kartlariniz ve nakdiniz sifirlanir ($2,500 baslangic verilir).",
            "* KAZANIM: Formule gore kalici 'Girisim Hissesi' kazanirsiniz.",
            "* Her 1 hisse, tum kazim hizina ve madencilik karina KALICI KATLANAN +%5 CARPAN verir!",
            "* Ar-Ge laboratuvarinda actiginiz tum teknolojiler ve hisseler KALICI KALIR, asla sifirlanmaz!",
            "* Bir sonraki turunuza devasa bir carpan gucuyle baslayarak cok daha hizli yukselirsiniz."
        };

        const char* linesEn[] = {
            "* Unlock IPO by reaching at least $50,000 company net worth.",
            "* When executing IPO: Your rigs, GPUs, and cash reset to fresh startup ($2,500 bonus).",
            "* REWARD: You earn permanent 'Venture Shares' based on total net worth.",
            "* Each share grants a PERMANENT COMPOUNDING +5% MULTIPLIER to all hashrate and profit!",
            "* All Research Technologies and Venture Shares PERSIST across all future rounds!",
            "* Restart with immense multiplier power to scale up your empire exponentially faster."
        };

        for (int l = 0; l < 6; ++l) {
            UIFrame::DrawTextCustom(isTR ? linesTr[l] : linesEn[l],
                                    innerX + 20.0f, innerY + 165.0f + (l * 24.0f), 14.0f, Color{180, 195, 220, 255}, false);
        }

        m_btnExecuteIPO.Draw();
    }
}

} // namespace Render
