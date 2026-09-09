#include "MarketModal.hpp"
#include "UIFrame.hpp"
#include "../core/Localization.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace Render {

MarketModal::MarketModal()
    : m_isOpen(false),
      m_currentCategory(MarketCategory::GPUS),
      m_btnTabGPUs(Rectangle{0, 0, 0, 0}, "", "", Color{25, 32, 45, 255}, Color{60, 180, 240, 255}),
      m_btnTabPower(Rectangle{0, 0, 0, 0}, "", "", Color{25, 32, 45, 255}, Color{255, 190, 40, 255}),
      m_btnTabCooling(Rectangle{0, 0, 0, 0}, "", "", Color{25, 32, 45, 255}, Color{40, 220, 200, 255}),
      m_btnTabFacilities(Rectangle{0, 0, 0, 0}, "", "", Color{25, 32, 45, 255}, Color{140, 100, 240, 255}),
      m_btnClose(Rectangle{0, 0, 0, 0}, "X", "", Color{180, 40, 40, 255}, Color{255, 80, 80, 255}),
      m_btnTierAll(Rectangle{0, 0, 0, 0}, "TUM MODELLER", "", Color{30, 42, 60, 255}, Color{80, 180, 255, 255}),
      m_btnTier1(Rectangle{0, 0, 0, 0}, "TIER 1 (GIRIS)", "", Color{25, 36, 45, 255}, Color{60, 220, 150, 255}),
      m_btnTier2(Rectangle{0, 0, 0, 0}, "TIER 2 (PRO)", "", Color{20, 36, 55, 255}, Color{50, 180, 255, 255}),
      m_btnTier3(Rectangle{0, 0, 0, 0}, "TIER 3 (SERVER)", "", Color{35, 25, 55, 255}, Color{180, 100, 255, 255}),
      m_btnTier4(Rectangle{0, 0, 0, 0}, "TIER 4 (ASIC)", "", Color{45, 25, 30, 255}, Color{255, 60, 100, 255})
{
    // 16 GPU Satın Alma Butonu (Tüm modeller için)
    for (int i = 0; i < 16; ++i) {
        m_gpuBuyButtons.emplace_back(Rectangle{0, 0, 0, 0}, "SATIN AL", "", Color{20, 70, 50, 255}, Color{40, 210, 120, 255});
    }

    // 4 Elektrik & Trafo Butonu
    for (int i = 0; i < 4; ++i) {
        m_powerBuyButtons.emplace_back(Rectangle{0, 0, 0, 0}, "YUKSELT", "", Color{70, 55, 20, 255}, Color{240, 180, 40, 255});
    }

    // 3 Soğutma Butonu
    for (int i = 0; i < 3; ++i) {
        m_coolingBuyButtons.emplace_back(Rectangle{0, 0, 0, 0}, "YUKSELT", "", Color{15, 60, 75, 255}, Color{40, 200, 240, 255});
    }

    // 3 Tesis & Depo Butonu
    for (int i = 0; i < 3; ++i) {
        m_facilityBuyButtons.emplace_back(Rectangle{0, 0, 0, 0}, "SATIN AL", "", Color{55, 25, 75, 255}, Color{190, 100, 240, 255});
    }
}

void MarketModal::Open() {
    m_isOpen = true;
}

void MarketModal::Close() {
    m_isOpen = false;
}

bool MarketModal::IsOpen() const {
    return m_isOpen;
}

MarketPurchaseAction MarketModal::Update(
    Core::EconomyManager& economy,
    const Core::Warehouse& warehouse,
    const Core::CoolingManager& coolingManager,
    const Core::MarketCatalog& catalog,
    const Core::PowerGrid& powerGrid)
{
    MarketPurchaseAction action;
    if (!m_isOpen) return action;

    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    const float modalW = std::clamp(screenW * 0.90f, 900.0f, 1360.0f);
    const float modalH = std::clamp(screenH * 0.90f, 600.0f, 880.0f);
    const float modalX = (screenW - modalW) * 0.5f;
    const float modalY = (screenH - modalH) * 0.5f;

    // Kapat butonu
    m_btnClose.SetBounds(Rectangle{modalX + modalW - 46.0f, modalY + 12.0f, 34.0f, 34.0f});
    if (m_btnClose.UpdateAndCheckClick() || IsKeyPressed(KEY_ESCAPE)) {
        Close();
        return action;
    }

    // Kategori Sekmeleri
    const float tabY = modalY + 54.0f;
    const float tabGap = 8.0f;
    const float tabW = (modalW - 40.0f - (3.0f * tabGap)) / 4.0f;
    const float tabH = 38.0f;

    m_btnTabGPUs.SetBounds(Rectangle{modalX + 20.0f + 0 * (tabW + tabGap), tabY, tabW, tabH});
    m_btnTabPower.SetBounds(Rectangle{modalX + 20.0f + 1 * (tabW + tabGap), tabY, tabW, tabH});
    m_btnTabCooling.SetBounds(Rectangle{modalX + 20.0f + 2 * (tabW + tabGap), tabY, tabW, tabH});
    m_btnTabFacilities.SetBounds(Rectangle{modalX + 20.0f + 3 * (tabW + tabGap), tabY, tabW, tabH});

    m_btnTabGPUs.SetTitle(Core::LocalizationManager::Tr("MARKET_TAB_GPUS"));
    m_btnTabPower.SetTitle(Core::LocalizationManager::Tr("MARKET_TAB_POWER"));
    m_btnTabCooling.SetTitle(Core::LocalizationManager::Tr("MARKET_TAB_COOLING"));
    m_btnTabFacilities.SetTitle(Core::LocalizationManager::Tr("MARKET_TAB_FACILITIES"));

    if (m_btnTabGPUs.UpdateAndCheckClick()) m_currentCategory = MarketCategory::GPUS;
    if (m_btnTabPower.UpdateAndCheckClick()) m_currentCategory = MarketCategory::POWER;
    if (m_btnTabCooling.UpdateAndCheckClick()) m_currentCategory = MarketCategory::COOLING;
    if (m_btnTabFacilities.UpdateAndCheckClick()) m_currentCategory = MarketCategory::FACILITIES;

    const float contentX = modalX + 20.0f;
    const float contentY = tabY + tabH + 16.0f;
    const float contentW = modalW - 40.0f;
    const float contentH = modalH - (contentY - modalY) - 20.0f;

    // Kategoriye Göre Satın Alma İşlemleri
    if (m_currentCategory == MarketCategory::GPUS) {
        const float filterY = contentY + 22.0f;
        const float filterH = 26.0f;
        const float filterW = (contentW - 32.0f) / 5.0f;

        m_btnTierAll.SetBounds(Rectangle{contentX, filterY, filterW, filterH});
        m_btnTier1.SetBounds(Rectangle{contentX + (filterW + 8.0f) * 1, filterY, filterW, filterH});
        m_btnTier2.SetBounds(Rectangle{contentX + (filterW + 8.0f) * 2, filterY, filterW, filterH});
        m_btnTier3.SetBounds(Rectangle{contentX + (filterW + 8.0f) * 3, filterY, filterW, filterH});
        m_btnTier4.SetBounds(Rectangle{contentX + (filterW + 8.0f) * 4, filterY, filterW, filterH});

        if (m_btnTierAll.UpdateAndCheckClick()) { m_gpuTierFilter = 0; m_gpuScrollOffset = 0.0f; }
        if (m_btnTier1.UpdateAndCheckClick()) { m_gpuTierFilter = 1; m_gpuScrollOffset = 0.0f; }
        if (m_btnTier2.UpdateAndCheckClick()) { m_gpuTierFilter = 2; m_gpuScrollOffset = 0.0f; }
        if (m_btnTier3.UpdateAndCheckClick()) { m_gpuTierFilter = 3; m_gpuScrollOffset = 0.0f; }
        if (m_btnTier4.UpdateAndCheckClick()) { m_gpuTierFilter = 4; m_gpuScrollOffset = 0.0f; }

        const float cardsAreaY = filterY + filterH + 8.0f;
        const float cardsAreaH = contentH - (cardsAreaY - contentY);
        Rectangle cardsAreaRec{contentX, cardsAreaY, contentW, cardsAreaH};

        if (CheckCollisionPointRec(GetMousePosition(), cardsAreaRec)) {
            m_gpuScrollOffset += GetMouseWheelMove() * 38.0f;
        }

        const auto& gpuModels = catalog.GetGPUModels();
        int visibleCount = 0;
        for (const auto& m : gpuModels) {
            if (m_gpuTierFilter == 0 || m.tier == m_gpuTierFilter) visibleCount++;
        }

        const float cardH = 74.0f;
        const float gap = 6.0f;
        const float totalCardsH = visibleCount * (cardH + gap);
        const float maxScroll = std::max(0.0f, totalCardsH - cardsAreaH);
        m_gpuScrollOffset = std::clamp(m_gpuScrollOffset, -maxScroll, 0.0f);

        const auto* activeRig = warehouse.GetActiveRig();
        const bool hasRigSpace = activeRig && (activeRig->GetGPUCount() < activeRig->GetMaxCapacity());
        const float buyBtnW = 145.0f;
        const float buyBtnH = 36.0f;

        if (m_gpuBuyButtons.size() < gpuModels.size()) {
            m_gpuBuyButtons.resize(gpuModels.size(), UIButton(Rectangle{0, 0, 0, 0}, "SATIN AL", "", Color{20, 70, 50, 255}, Color{40, 210, 120, 255}));
        }

        int displayIdx = 0;
        for (size_t i = 0; i < gpuModels.size(); ++i) {
            const auto& model = gpuModels[i];
            if (m_gpuTierFilter != 0 && model.tier != m_gpuTierFilter) {
                m_gpuBuyButtons[i].SetBounds(Rectangle{-1000, -1000, 0, 0});
                continue;
            }

            const float itemY = cardsAreaY + m_gpuScrollOffset + (displayIdx * (cardH + gap));
            const float btnX = contentX + contentW - buyBtnW - 14.0f;
            const float btnY = itemY + 19.0f;
            m_gpuBuyButtons[i].SetBounds(Rectangle{btnX, btnY, buyBtnW, buyBtnH});

            bool canAfford = economy.GetFiatBalance() >= model.priceUSD;
            if (!hasRigSpace) {
                m_gpuBuyButtons[i].SetTitle(Core::LocalizationManager::Tr("MARKET_RIG_FULL"));
                m_gpuBuyButtons[i].SetDisabled(true);
            } else if (!canAfford) {
                m_gpuBuyButtons[i].SetTitle(Core::LocalizationManager::Tr("MARKET_NO_MONEY"));
                m_gpuBuyButtons[i].SetDisabled(true);
            } else {
                m_gpuBuyButtons[i].SetTitle(Core::LocalizationManager::Tr("MARKET_BUY"));
                m_gpuBuyButtons[i].SetDisabled(false);
            }

            if (itemY + cardH >= cardsAreaY && itemY <= cardsAreaY + cardsAreaH) {
                if (m_gpuBuyButtons[i].UpdateAndCheckClick()) {
                    action.type = MarketPurchaseAction::ActionType::BUY_GPU;
                    action.itemIndex = i;
                    return action;
                }
            }

            displayIdx++;
        }
    } else if (m_currentCategory == MarketCategory::POWER) {
        const auto& powerUpgrades = catalog.GetPowerUpgrades();
        const float cardH = (contentH - 30.0f) / 4.0f;
        const float buyBtnW = 160.0f;
        const float buyBtnH = cardH - 20.0f;

        for (size_t i = 0; i < powerUpgrades.size() && i < m_powerBuyButtons.size(); ++i) {
            const auto& upg = powerUpgrades[i];
            const float itemY = contentY + 12.0f + (static_cast<float>(i) * cardH);
            const float btnX = contentX + contentW - buyBtnW - 14.0f;
            const float btnY = itemY + 10.0f;

            m_powerBuyButtons[i].SetBounds(Rectangle{btnX, btnY, buyBtnW, buyBtnH});

            if (upg.isInstalled) {
                m_powerBuyButtons[i].SetTitle(Core::LocalizationManager::Tr("MARKET_INSTALLED"));
                m_powerBuyButtons[i].SetDisabled(true);
            } else {
                bool canAfford = economy.GetFiatBalance() >= upg.priceUSD;
                m_powerBuyButtons[i].SetTitle(canAfford ? Core::LocalizationManager::Tr("MARKET_BUY") : Core::LocalizationManager::Tr("MARKET_NO_MONEY"));
                m_powerBuyButtons[i].SetDisabled(!canAfford);
            }

            if (m_powerBuyButtons[i].UpdateAndCheckClick()) {
                action.type = MarketPurchaseAction::ActionType::BUY_POWER;
                action.itemIndex = i;
                return action;
            }
        }
    } else if (m_currentCategory == MarketCategory::COOLING) {
        const auto& tiers = coolingManager.GetTiers();
        const float cardH = (contentH - 30.0f) / 3.0f;
        const float buyBtnW = 160.0f;
        const float buyBtnH = cardH - 24.0f;

        for (size_t i = 1; i < tiers.size() && (i - 1) < m_coolingBuyButtons.size(); ++i) {
            size_t btnIdx = i - 1;
            const auto& tier = tiers[i];
            const float itemY = contentY + 14.0f + (static_cast<float>(btnIdx) * cardH);
            const float btnX = contentX + contentW - buyBtnW - 14.0f;
            const float btnY = itemY + 12.0f;

            m_coolingBuyButtons[btnIdx].SetBounds(Rectangle{btnX, btnY, buyBtnW, buyBtnH});

            if (tier.isInstalled) {
                m_coolingBuyButtons[btnIdx].SetTitle(Core::LocalizationManager::Tr("MARKET_INSTALLED"));
                m_coolingBuyButtons[btnIdx].SetDisabled(true);
            } else {
                bool canAfford = economy.GetFiatBalance() >= tier.cost;
                m_coolingBuyButtons[btnIdx].SetTitle(canAfford ? Core::LocalizationManager::Tr("MARKET_BUY") : Core::LocalizationManager::Tr("MARKET_NO_MONEY"));
                m_coolingBuyButtons[btnIdx].SetDisabled(!canAfford);
            }

            if (m_coolingBuyButtons[btnIdx].UpdateAndCheckClick()) {
                action.type = MarketPurchaseAction::ActionType::BUY_COOLING;
                action.itemIndex = i;
                return action;
            }
        }
    } else if (m_currentCategory == MarketCategory::FACILITIES) {
        const auto& facilities = catalog.GetFacilityUpgrades();
        const float cardH = (contentH - 30.0f) / 3.0f;
        const float buyBtnW = 160.0f;
        const float buyBtnH = cardH - 24.0f;

        for (size_t i = 0; i < facilities.size() && i < m_facilityBuyButtons.size(); ++i) {
            const auto& fac = facilities[i];
            const float itemY = contentY + 14.0f + (static_cast<float>(i) * cardH);
            const float btnX = contentX + contentW - buyBtnW - 14.0f;
            const float btnY = itemY + 12.0f;

            m_facilityBuyButtons[i].SetBounds(Rectangle{btnX, btnY, buyBtnW, buyBtnH});

            if (fac.isInstalled) {
                m_facilityBuyButtons[i].SetTitle(Core::LocalizationManager::Tr("MARKET_INSTALLED"));
                m_facilityBuyButtons[i].SetDisabled(true);
            } else {
                bool canAfford = economy.GetFiatBalance() >= fac.priceUSD;
                m_facilityBuyButtons[i].SetTitle(canAfford ? Core::LocalizationManager::Tr("MARKET_BUY") : Core::LocalizationManager::Tr("MARKET_NO_MONEY"));
                m_facilityBuyButtons[i].SetDisabled(!canAfford);
            }

            if (m_facilityBuyButtons[i].UpdateAndCheckClick()) {
                action.type = MarketPurchaseAction::ActionType::BUY_FACILITY;
                action.itemIndex = i;
                return action;
            }
        }
    }

    return action;
}

void MarketModal::Draw(
    const Core::EconomyManager& economy,
    const Core::Warehouse& warehouse,
    const Core::CoolingManager& coolingManager,
    const Core::MarketCatalog& catalog,
    const Core::PowerGrid& powerGrid)
{
    if (!m_isOpen) return;

    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    // Karartma katmanı
    DrawRectangle(0, 0, static_cast<int>(screenW), static_cast<int>(screenH), Color{8, 12, 20, 230});

    const float modalW = std::clamp(screenW * 0.90f, 900.0f, 1360.0f);
    const float modalH = std::clamp(screenH * 0.90f, 600.0f, 880.0f);
    const float modalX = (screenW - modalW) * 0.5f;
    const float modalY = (screenH - modalH) * 0.5f;

    // Ana Camgöbeği Çerçeve
    UIFrame::DrawCard(Rectangle{modalX, modalY, modalW, modalH}, "", Color{45, 180, 240, 255});

    DrawHeader(modalX, modalY, modalW);
    DrawTabs(modalX, modalY, modalW);

    const float tabY = modalY + 54.0f;
    const float tabH = 38.0f;
    const float contentX = modalX + 20.0f;
    const float contentY = tabY + tabH + 16.0f;
    const float contentW = modalW - 40.0f;
    const float contentH = modalH - (contentY - modalY) - 20.0f;

    // Kategori İçerik Çizimi
    switch (m_currentCategory) {
        case MarketCategory::GPUS:
            DrawGPUsCategory(contentX, contentY, contentW, contentH, economy, warehouse, catalog);
            break;
        case MarketCategory::POWER:
            DrawPowerCategory(contentX, contentY, contentW, contentH, economy, catalog, powerGrid);
            break;
        case MarketCategory::COOLING:
            DrawCoolingCategory(contentX, contentY, contentW, contentH, economy, coolingManager);
            break;
        case MarketCategory::FACILITIES:
            DrawFacilitiesCategory(contentX, contentY, contentW, contentH, economy, catalog);
            break;
    }

    m_btnClose.Draw();
}

void MarketModal::DrawHeader(float modalX, float modalY, float modalW) {
    DrawRectangle(static_cast<int>(modalX), static_cast<int>(modalY), static_cast<int>(modalW), 46, Color{18, 24, 38, 255});
    DrawLine(static_cast<int>(modalX), static_cast<int>(modalY + 46), static_cast<int>(modalX + modalW), static_cast<int>(modalY + 46), Color{45, 180, 240, 180});

    std::string title = std::string("MARKET - ") + Core::LocalizationManager::Tr("MARKET_TITLE");
    UIFrame::DrawTextCustom(title, modalX + 24.0f, modalY + 12.0f, 22.0f, Color{240, 248, 255, 255}, true);
}

void MarketModal::DrawTabs(float modalX, float modalY, float modalW) {
    m_btnTabGPUs.Draw();
    m_btnTabPower.Draw();
    m_btnTabCooling.Draw();
    m_btnTabFacilities.Draw();

    // Aktif sekmenin altına neon çizgi
    UIButton* activeBtn = nullptr;
    Color accentColor = Color{45, 180, 240, 255};
    if (m_currentCategory == MarketCategory::GPUS) {
        activeBtn = &m_btnTabGPUs;
        accentColor = Color{60, 180, 240, 255};
    } else if (m_currentCategory == MarketCategory::POWER) {
        activeBtn = &m_btnTabPower;
        accentColor = Color{255, 190, 40, 255};
    } else if (m_currentCategory == MarketCategory::COOLING) {
        activeBtn = &m_btnTabCooling;
        accentColor = Color{40, 220, 200, 255};
    } else {
        activeBtn = &m_btnTabFacilities;
        accentColor = Color{160, 110, 240, 255};
    }

    if (activeBtn) {
        Rectangle b = activeBtn->GetBounds();
        DrawRectangle(static_cast<int>(b.x), static_cast<int>(b.y + b.height - 3), static_cast<int>(b.width), 3, accentColor);
    }
}

void MarketModal::DrawStatBar(
    float x, float y, float w, float h,
    float ratio, Color barColor,
    const char* labelLeft, const char* labelRight)
{
    ratio = std::clamp(ratio, 0.0f, 1.0f);

    // Arka plan
    DrawRectangleRounded(Rectangle{x, y, w, h}, 0.3f, 4, Color{16, 22, 34, 255});
    DrawRectangleRoundedLines(Rectangle{x, y, w, h}, 0.3f, 4, 1.2f, Color{40, 52, 75, 255});

    // Dolu kısım
    float fillW = w * ratio;
    if (fillW > 4.0f) {
        DrawRectangleRounded(Rectangle{x + 1, y + 1, fillW - 2, h - 2}, 0.3f, 4, barColor);
    }

    // Metinler
    UIFrame::DrawTextCustom(labelLeft, x + 6.0f, y + 1.0f, 11.0f, Color{200, 220, 245, 255}, false);
    float rW = UIFrame::MeasureTextCustom(labelRight, 11.0f, true);
    UIFrame::DrawTextCustom(labelRight, x + w - rW - 6.0f, y + 1.0f, 11.0f, Color{255, 255, 255, 255}, true);
}

void MarketModal::DrawGPUsCategory(
    float startX, float startY, float width, float height,
    const Core::EconomyManager& economy,
    const Core::Warehouse& warehouse,
    const Core::MarketCatalog& catalog)
{
    const auto* activeRig = warehouse.GetActiveRig();
    const auto& gpuModels = catalog.GetGPUModels();

    // 1. Üst Bilgi Rozeti (Aktif Seçili Rig ve Boş Slot Durumu)
    std::string rigInfo = Core::LocalizationManager::Tr("MARKET_ACTIVE_RIG");
    if (activeRig) {
        rigInfo += activeRig->GetName() + " [" + std::to_string(activeRig->GetGPUCount()) + "/" + std::to_string(activeRig->GetMaxCapacity()) + " Slot]";
    } else {
        rigInfo += "Yok";
    }
    UIFrame::DrawTextCustom(rigInfo, startX + 8.0f, startY - 2.0f, 14.0f, Color{140, 180, 220, 255}, true);

    // 2. Tier Filtre Butonları
    m_btnTierAll.Draw();
    m_btnTier1.Draw();
    m_btnTier2.Draw();
    m_btnTier3.Draw();
    m_btnTier4.Draw();

    // Aktif filtre butonuna parlama efekti
    UIButton* activeFilterBtn = nullptr;
    if (m_gpuTierFilter == 0) activeFilterBtn = &m_btnTierAll;
    else if (m_gpuTierFilter == 1) activeFilterBtn = &m_btnTier1;
    else if (m_gpuTierFilter == 2) activeFilterBtn = &m_btnTier2;
    else if (m_gpuTierFilter == 3) activeFilterBtn = &m_btnTier3;
    else if (m_gpuTierFilter == 4) activeFilterBtn = &m_btnTier4;

    if (activeFilterBtn) {
        Rectangle fb = activeFilterBtn->GetBounds();
        DrawRectangleRoundedLines(fb, 0.2f, 4, 2.0f, Color{255, 220, 80, 255});
    }

    // 3. Scissor Mode ile Kaydırılabilir Kart Alanı
    const float filterY = startY + 22.0f;
    const float filterH = 26.0f;
    const float cardsAreaY = filterY + filterH + 8.0f;
    const float cardsAreaH = height - (cardsAreaY - startY);
    Rectangle cardsAreaRec{startX, cardsAreaY, width, cardsAreaH};

    BeginScissorMode((int)cardsAreaRec.x, (int)cardsAreaRec.y, (int)cardsAreaRec.width, (int)cardsAreaRec.height);

    const float cardH = 74.0f;
    const float gap = 6.0f;
    const float buyBtnW = 145.0f;

    int displayIdx = 0;
    for (size_t i = 0; i < gpuModels.size(); ++i) {
        const auto& model = gpuModels[i];
        if (m_gpuTierFilter != 0 && model.tier != m_gpuTierFilter) {
            continue;
        }

        const float itemY = cardsAreaY + m_gpuScrollOffset + (displayIdx * (cardH + gap));

        // Ekran dışı kartları çizme (culling)
        if (itemY + cardH < cardsAreaY || itemY > cardsAreaY + cardsAreaH) {
            displayIdx++;
            continue;
        }

        // Kart Arka Planı
        DrawRectangleRounded(Rectangle{startX, itemY, width, cardH}, 0.12f, 4, Color{18, 25, 38, 230});
        DrawRectangleRoundedLines(Rectangle{startX, itemY, width, cardH}, 0.12f, 4, 1.5f, model.accentColor);

        // Sol Kısım: GPU Adı ve Tier Rozeti
        UIFrame::DrawTextCustom(model.name, startX + 16.0f, itemY + 10.0f, 16.0f, Color{245, 250, 255, 255}, true);

        std::string tierText = "[" + std::string(Core::LocalizationManager::Tr(model.tierKey)) + " - Tier " + std::to_string(model.tier) + "]";
        UIFrame::DrawTextCustom(tierText, startX + 16.0f, itemY + 30.0f, 11.0f, model.accentColor, false);

        // Gerekli Tesis Seviyesi
        if (model.minFacilityTier > 1) {
            std::string facReq = "Gereken Tesis: Seviye " + std::to_string(model.minFacilityTier);
            UIFrame::DrawTextCustom(facReq, startX + 16.0f, itemY + 48.0f, 11.0f, Color{160, 175, 195, 220}, false);
        }

        // Orta Kısım: 3 Adet Performans Barı
        const float barX = startX + 270.0f;
        const float barW = width - 270.0f - buyBtnW - 140.0f;
        const float barH = 13.0f;
        const float barGap = 4.0f;

        // 1. Kazım Gücü Barı (Max 550 MH/s ölçeği)
        std::ostringstream ssHash;
        ssHash << std::fixed << std::setprecision(0) << model.hashrate << " MH/s";
        float hashRatio = static_cast<float>(model.hashrate / 550.0);
        DrawStatBar(barX, itemY + 8.0f, barW, barH, hashRatio, Color{45, 200, 240, 220},
                    Core::LocalizationManager::Tr("MARKET_HASH_BAR"), ssHash.str().c_str());

        // 2. Güç Çekişi Barı (Max 750 W ölçeği)
        std::ostringstream ssPower;
        ssPower << std::fixed << std::setprecision(0) << model.powerWatts << " W";
        float powerRatio = static_cast<float>(model.powerWatts / 750.0);
        DrawStatBar(barX, itemY + 8.0f + barH + barGap, barW, barH, powerRatio, Color{255, 175, 45, 220},
                    Core::LocalizationManager::Tr("MARKET_POWER_BAR"), ssPower.str().c_str());

        // 3. Verimlilik Barı (Max 0.75 MH/W ölçeği)
        std::ostringstream ssEff;
        ssEff << std::fixed << std::setprecision(2) << model.efficiency << " MH/W";
        float effRatio = static_cast<float>(model.efficiency / 0.75);
        DrawStatBar(barX, itemY + 8.0f + (2 * (barH + barGap)), barW, barH, effRatio, Color{50, 225, 140, 220},
                    Core::LocalizationManager::Tr("MARKET_EFF_BAR"), ssEff.str().c_str());

        // Sağ Kısım: Fiyat ve Satın Al Butonu
        const float priceX = barX + barW + 16.0f;
        std::string priceStr = economy.FormatFiat(model.priceUSD);
        UIFrame::DrawTextCustom(priceStr, priceX, itemY + 24.0f, 17.0f, Color{100, 255, 160, 255}, true);

        if (i < m_gpuBuyButtons.size()) {
            m_gpuBuyButtons[i].Draw();
        }

        displayIdx++;
    }

    EndScissorMode();
}

void MarketModal::DrawPowerCategory(
    float startX, float startY, float width, float height,
    const Core::EconomyManager& economy,
    const Core::MarketCatalog& catalog,
    const Core::PowerGrid& powerGrid)
{
    // Şebeke Durum Özeti
    std::string gridSummary = "Anlik Sebeke Yuku: " + std::to_string(static_cast<int>(powerGrid.GetTotalConsumptionWatts())) + 
                              " W / Guvenli Trafo Kapasitesi: " + std::to_string(static_cast<int>(powerGrid.GetMaxCapacityWatts())) + " W";
    UIFrame::DrawTextCustom(gridSummary, startX + 8.0f, startY - 2.0f, 14.0f, Color{255, 200, 80, 255}, true);

    const auto& powerUpgrades = catalog.GetPowerUpgrades();
    const float cardH = (height - 24.0f) / 4.0f;
    const float buyBtnW = 160.0f;

    for (size_t i = 0; i < powerUpgrades.size(); ++i) {
        const auto& upg = powerUpgrades[i];
        const float itemY = startY + 18.0f + (static_cast<float>(i) * cardH);
        const float itemH = cardH - 8.0f;

        DrawRectangleRounded(Rectangle{startX, itemY, width, itemH}, 0.12f, 4, Color{18, 25, 38, 230});
        Color border = upg.isInstalled ? Color{50, 210, 120, 255} : Color{240, 175, 45, 200};
        DrawRectangleRoundedLines(Rectangle{startX, itemY, width, itemH}, 0.12f, 4, 1.5f, border);

        std::string title = Core::LocalizationManager::Tr(upg.nameKey);
        UIFrame::DrawTextCustom(title, startX + 20.0f, itemY + 16.0f, 18.0f, Color{245, 250, 255, 255}, true);

        std::string capText = "Maksimum Hat Limiti: " + std::to_string(static_cast<int>(upg.capacityWatts)) + " Watts";
        UIFrame::DrawTextCustom(capText, startX + 20.0f, itemY + 42.0f, 13.0f, Color{180, 195, 220, 255}, false);

        // Kapasite Barı
        const float barX = startX + 380.0f;
        const float barW = width - 380.0f - buyBtnW - 140.0f;
        float capRatio = static_cast<float>(upg.capacityWatts / 30000.0);
        DrawStatBar(barX, itemY + 24.0f, barW, 20.0f, capRatio, Color{245, 170, 40, 220}, "Kapasite", "");

        // Fiyat
        const float priceX = barX + barW + 16.0f;
        std::string priceStr = upg.isInstalled ? "AKTIF" : economy.FormatFiat(upg.priceUSD);
        Color priceColor = upg.isInstalled ? Color{50, 210, 120, 255} : Color{100, 255, 160, 255};
        UIFrame::DrawTextCustom(priceStr, priceX, itemY + 24.0f, 18.0f, priceColor, true);

        if (i < m_powerBuyButtons.size()) {
            m_powerBuyButtons[i].Draw();
        }
    }
}

void MarketModal::DrawCoolingCategory(
    float startX, float startY, float width, float height,
    const Core::EconomyManager& economy,
    const Core::CoolingManager& coolingManager)
{
    std::string coolSummary = "Tesis Toplam Isi Tahliye Kapasitesi: " + 
                              std::to_string(static_cast<int>(coolingManager.CalculateTotalCoolingWatts())) + " Watts";
    UIFrame::DrawTextCustom(coolSummary, startX + 8.0f, startY - 2.0f, 14.0f, Color{45, 210, 240, 255}, true);

    const auto& tiers = coolingManager.GetTiers();
    const float cardH = (height - 24.0f) / 3.0f;
    const float buyBtnW = 160.0f;

    for (size_t i = 1; i < tiers.size(); ++i) {
        size_t btnIdx = i - 1;
        const auto& tier = tiers[i];
        const float itemY = startY + 18.0f + (static_cast<float>(btnIdx) * cardH);
        const float itemH = cardH - 10.0f;

        DrawRectangleRounded(Rectangle{startX, itemY, width, itemH}, 0.12f, 4, Color{18, 25, 38, 230});
        Color border = tier.isInstalled ? Color{50, 210, 120, 255} : Color{45, 200, 240, 200};
        DrawRectangleRoundedLines(Rectangle{startX, itemY, width, itemH}, 0.12f, 4, 1.5f, border);

        UIFrame::DrawTextCustom(tier.name, startX + 20.0f, itemY + 18.0f, 19.0f, Color{245, 250, 255, 255}, true);

        std::string desc = "+" + std::to_string(static_cast<int>(tier.addedCoolingWatts)) + "W Endustriyel Isi Tahliyesi";
        UIFrame::DrawTextCustom(desc, startX + 20.0f, itemY + 46.0f, 13.0f, Color{160, 210, 240, 255}, false);

        // Bar
        const float barX = startX + 380.0f;
        const float barW = width - 380.0f - buyBtnW - 140.0f;
        float coolRatio = static_cast<float>(tier.addedCoolingWatts / 8000.0);
        DrawStatBar(barX, itemY + 28.0f, barW, 20.0f, coolRatio, Color{45, 215, 245, 220}, "Sogutma Gucu", "");

        // Fiyat
        const float priceX = barX + barW + 16.0f;
        std::string priceStr = tier.isInstalled ? "KURULDU" : economy.FormatFiat(tier.cost);
        Color priceColor = tier.isInstalled ? Color{50, 210, 120, 255} : Color{100, 255, 160, 255};
        UIFrame::DrawTextCustom(priceStr, priceX, itemY + 28.0f, 18.0f, priceColor, true);

        if (btnIdx < m_coolingBuyButtons.size()) {
            m_coolingBuyButtons[btnIdx].Draw();
        }
    }
}

void MarketModal::DrawFacilitiesCategory(
    float startX, float startY, float width, float height,
    const Core::EconomyManager& economy,
    const Core::MarketCatalog& catalog)
{
    std::string facSummary = "Yesil Gunes Enerjisi: +" + std::to_string(static_cast<int>(catalog.GetTotalGreenWatts())) + " W" +
                             (catalog.HasAutoFireSuppression() ? " | Termal Yangin Sondurucu: AKTIF" : " | Termal Yangin Sondurucu: YOK");
    UIFrame::DrawTextCustom(facSummary, startX + 8.0f, startY - 2.0f, 14.0f, Color{180, 120, 250, 255}, true);

    const auto& facilities = catalog.GetFacilityUpgrades();
    const float cardH = (height - 24.0f) / 3.0f;
    const float buyBtnW = 160.0f;

    for (size_t i = 0; i < facilities.size(); ++i) {
        const auto& fac = facilities[i];
        const float itemY = startY + 18.0f + (static_cast<float>(i) * cardH);
        const float itemH = cardH - 10.0f;

        DrawRectangleRounded(Rectangle{startX, itemY, width, itemH}, 0.12f, 4, Color{18, 25, 38, 230});
        Color border = fac.isInstalled ? Color{50, 210, 120, 255} : Color{170, 110, 245, 200};
        DrawRectangleRoundedLines(Rectangle{startX, itemY, width, itemH}, 0.12f, 4, 1.5f, border);

        std::string title = Core::LocalizationManager::Tr(fac.nameKey);
        UIFrame::DrawTextCustom(title, startX + 20.0f, itemY + 18.0f, 19.0f, Color{245, 250, 255, 255}, true);

        std::string desc = Core::LocalizationManager::Tr(fac.descKey);
        UIFrame::DrawTextCustom(desc, startX + 20.0f, itemY + 46.0f, 13.0f, Color{200, 180, 240, 255}, false);

        // Fiyat
        const float priceX = startX + width - buyBtnW - 140.0f;
        std::string priceStr = fac.isInstalled ? "AKTIF" : economy.FormatFiat(fac.priceUSD);
        Color priceColor = fac.isInstalled ? Color{50, 210, 120, 255} : Color{100, 255, 160, 255};
        UIFrame::DrawTextCustom(priceStr, priceX, itemY + 28.0f, 18.0f, priceColor, true);

        if (i < m_facilityBuyButtons.size()) {
            m_facilityBuyButtons[i].Draw();
        }
    }
}

} // namespace Render
