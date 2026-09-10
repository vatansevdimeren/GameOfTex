#include "WorldMapModal.hpp"
#include "UIFrame.hpp"
#include "../core/Localization.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace Render {

WorldMapModal::WorldMapModal()
    : m_isOpen(false),
      m_selectedFacilityIndex(0),
      m_btnClose(Rectangle{0, 0, 0, 0}, "X", "", Color{180, 40, 40, 255}, Color{255, 80, 80, 255}),
      m_btnAction(Rectangle{0, 0, 0, 0}, "SEC", "", Color{30, 80, 60, 255}, Color{40, 220, 140, 255}),
      m_btnUpgradeEconomist(Rectangle{0, 0, 0, 0}, "EKONOMIST", "", Color{35, 45, 65, 255}, Color{255, 210, 60, 255})
{
    for (int i = 0; i < 8; ++i) {
        m_facilityListButtons.emplace_back(Rectangle{0, 0, 0, 0}, "", "", Color{25, 32, 45, 255}, Color{60, 160, 240, 255});
    }
}

void WorldMapModal::Open() {
    m_isOpen = true;
}

void WorldMapModal::Close() {
    m_isOpen = false;
}

bool WorldMapModal::IsOpen() const {
    return m_isOpen;
}

void WorldMapModal::Update(Core::FacilityManager& facilityManager, Core::EconomyManager& economy) {
    if (!m_isOpen) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    float modalW = std::min(980.0f, screenW - 40.0f);
    float modalH = std::min(640.0f, screenH - 50.0f);
    float modalX = (screenW - modalW) * 0.5f;
    float modalY = (screenH - modalH) * 0.5f;

    // Close button
    m_btnClose.SetBounds(Rectangle{modalX + modalW - 45.0f, modalY + 12.0f, 32.0f, 32.0f});
    if (m_btnClose.UpdateAndCheckClick()) {
        Close();
        return;
    }

    const auto& facilities = facilityManager.GetAllFacilities();
    if (m_facilityListButtons.size() < facilities.size()) {
        m_facilityListButtons.resize(facilities.size(), UIButton(Rectangle{0, 0, 0, 0}, "", "", Color{25, 32, 45, 255}, Color{60, 160, 240, 255}));
    }

    // Left map area & Right dossier area
    float mapW = (modalW - 50.0f) * 0.60f;
    float mapH = modalH - 160.0f;
    Rectangle mapArea{modalX + 20.0f, modalY + 70.0f, mapW, mapH};

    Vector2 mousePos = GetMousePosition();

    // Check node clicks on map
    for (size_t i = 0; i < facilities.size(); ++i) {
        const auto& fac = facilities[i];
        float nodeX = mapArea.x + fac.mapNormX * mapArea.width;
        float nodeY = mapArea.y + fac.mapNormY * mapArea.height;
        float nodeRadius = 16.0f;

        if (CheckCollisionPointCircle(mousePos, Vector2{nodeX, nodeY}, nodeRadius)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                m_selectedFacilityIndex = i;
            }
        }
    }

    // Update bottom facility selector tabs
    float tabW = (modalW - 40.0f) / static_cast<float>(facilities.size());
    float tabH = 65.0f;
    float tabY = modalY + modalH - 75.0f;

    for (size_t i = 0; i < facilities.size(); ++i) {
        Rectangle tabRec{modalX + 20.0f + i * tabW, tabY, tabW - 6.0f, tabH};
        m_facilityListButtons[i].SetBounds(tabRec);
        
        std::string title = Core::LocalizationManager::Tr(facilities[i].nameKey);
        if (title.length() > 18) title = title.substr(0, 16) + "..";
        m_facilityListButtons[i].SetTitle(title);
        
        std::string sub = facilities[i].isPurchased ? Core::LocalizationManager::Tr("FAC_OWNED") : economy.FormatFiat(facilities[i].purchaseCostUSD);
        m_facilityListButtons[i].SetSubtitle(sub);

        if (i == m_selectedFacilityIndex) {
            m_facilityListButtons[i].SetAccentColor(Color{255, 210, 40, 255});
            m_facilityListButtons[i].SetBaseColor(Color{40, 45, 60, 255});
        } else if (i == facilityManager.GetActiveFacilityIndex()) {
            m_facilityListButtons[i].SetAccentColor(Color{0, 240, 160, 255});
            m_facilityListButtons[i].SetBaseColor(Color{25, 38, 45, 255});
        } else {
            m_facilityListButtons[i].SetAccentColor(Color{70, 110, 160, 255});
            m_facilityListButtons[i].SetBaseColor(Color{20, 26, 38, 255});
        }

        if (m_facilityListButtons[i].UpdateAndCheckClick()) {
            m_selectedFacilityIndex = i;
        }
    }

    // Update Economist & Action Buttons
    float infoW = (modalW - 50.0f) * 0.40f;
    float infoX = mapArea.x + mapArea.width + 10.0f;
    float infoY = mapArea.y;

    Rectangle btnEcoRec{infoX + 15.0f, infoY + mapH - 96.0f, infoW - 30.0f, 38.0f};
    m_btnUpgradeEconomist.SetBounds(btnEcoRec);

    Rectangle btnActionRec{infoX + 15.0f, infoY + mapH - 50.0f, infoW - 30.0f, 42.0f};
    m_btnAction.SetBounds(btnActionRec);

    if (m_selectedFacilityIndex < facilities.size()) {
        const auto& selFac = facilities[m_selectedFacilityIndex];
        bool isActive = (m_selectedFacilityIndex == facilityManager.GetActiveFacilityIndex());

        // Economist Upgrade / Hire Button
        if (!selFac.isPurchased) {
            m_btnUpgradeEconomist.SetTitle("ONCE TESISI ALIN (EKONOMIST)");
            m_btnUpgradeEconomist.SetSubtitle("");
            m_btnUpgradeEconomist.SetDisabled(true);
            m_btnUpgradeEconomist.SetAccentColor(Color{100, 100, 110, 255});
            m_btnUpgradeEconomist.SetBaseColor(Color{25, 30, 40, 255});
        } else if (selFac.economist.level >= 5) {
            m_btnUpgradeEconomist.SetTitle("MAKSIMUM SEVIYE (Lv 5)");
            m_btnUpgradeEconomist.SetSubtitle("Yapay Zeka Destekli Bas Ekonomist");
            m_btnUpgradeEconomist.SetDisabled(true);
            m_btnUpgradeEconomist.SetAccentColor(Color{180, 140, 255, 255});
            m_btnUpgradeEconomist.SetBaseColor(Color{40, 30, 60, 255});
        } else {
            double cost = facilityManager.GetEconomistUpgradeCost(m_selectedFacilityIndex);
            bool canAfford = economy.GetFiatBalance() >= cost;
            std::string btnText = (selFac.economist.level == 0)
                ? ("EKONOMIST TUT (" + economy.FormatFiat(cost) + ")")
                : ("YUKSELT Lv " + std::to_string(selFac.economist.level + 1) + " (" + economy.FormatFiat(cost) + ")");
            m_btnUpgradeEconomist.SetTitle(btnText);
            m_btnUpgradeEconomist.SetSubtitle(facilityManager.GetEconomistNextTitle(m_selectedFacilityIndex));
            m_btnUpgradeEconomist.SetDisabled(!canAfford);
            m_btnUpgradeEconomist.SetAccentColor(canAfford ? Color{255, 200, 50, 255} : Color{120, 120, 130, 255});
            m_btnUpgradeEconomist.SetBaseColor(canAfford ? Color{45, 50, 65, 255} : Color{25, 28, 38, 255});

            if (m_btnUpgradeEconomist.UpdateAndCheckClick()) {
                facilityManager.HireOrUpgradeEconomist(m_selectedFacilityIndex, economy);
            }
        }

        // Facility Switch / Purchase Button
        if (isActive) {
            m_btnAction.SetTitle(Core::LocalizationManager::Tr("FAC_BTN_ACTIVE"));
            m_btnAction.SetDisabled(true);
            m_btnAction.SetAccentColor(Color{0, 240, 160, 255});
        } else if (selFac.isPurchased) {
            m_btnAction.SetTitle(Core::LocalizationManager::Tr("FAC_BTN_SWITCH"));
            m_btnAction.SetDisabled(false);
            m_btnAction.SetAccentColor(Color{60, 180, 255, 255});
        } else {
            m_btnAction.SetTitle(Core::LocalizationManager::Tr("FAC_BTN_BUY") + std::string(" (") + economy.FormatFiat(selFac.purchaseCostUSD) + ")");
            m_btnAction.SetDisabled(economy.GetFiatBalance() < selFac.purchaseCostUSD);
            m_btnAction.SetAccentColor(Color{255, 190, 40, 255});
        }

        if (m_btnAction.UpdateAndCheckClick()) {
            if (selFac.isPurchased) {
                facilityManager.SwitchFacility(m_selectedFacilityIndex);
            } else {
                facilityManager.PurchaseFacility(m_selectedFacilityIndex, economy);
            }
        }
    }
}

void WorldMapModal::DrawWorldMapTacticalGrid(Rectangle mapArea) const {
    // Tactical radar background
    DrawRectangleRounded(mapArea, 0.04f, 6, Color{12, 16, 24, 255});
    DrawRectangleRoundedLines(mapArea, 0.04f, 6, 1.5f, Color{45, 65, 95, 255});

    // Radar coordinate grid lines (Latitude & Longitude)
    for (int y = 1; y < 6; ++y) {
        float lineY = mapArea.y + (mapArea.height / 6.0f) * y;
        DrawLine(static_cast<int>(mapArea.x), static_cast<int>(lineY), static_cast<int>(mapArea.x + mapArea.width), static_cast<int>(lineY), Color{35, 50, 75, 120});
    }
    for (int x = 1; x < 8; ++x) {
        float lineX = mapArea.x + (mapArea.width / 8.0f) * x;
        DrawLine(static_cast<int>(lineX), static_cast<int>(mapArea.y), static_cast<int>(lineX), static_cast<int>(mapArea.y + mapArea.height), Color{35, 50, 75, 120});
    }

    // Concentric radar scan rings
    Vector2 center{mapArea.x + mapArea.width * 0.5f, mapArea.y + mapArea.height * 0.5f};
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), mapArea.height * 0.25f, Color{40, 60, 90, 100});
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), mapArea.height * 0.45f, Color{40, 60, 90, 80});

    // Approximate continent stylized shapes (tactical vector polygons)
    // North America
    DrawTriangle(Vector2{mapArea.x + mapArea.width * 0.15f, mapArea.y + mapArea.height * 0.25f},
                 Vector2{mapArea.x + mapArea.width * 0.28f, mapArea.y + mapArea.height * 0.45f},
                 Vector2{mapArea.x + mapArea.width * 0.10f, mapArea.y + mapArea.height * 0.48f},
                 Color{35, 52, 78, 160});
    // South America
    DrawTriangle(Vector2{mapArea.x + mapArea.width * 0.24f, mapArea.y + mapArea.height * 0.55f},
                 Vector2{mapArea.x + mapArea.width * 0.35f, mapArea.y + mapArea.height * 0.65f},
                 Vector2{mapArea.x + mapArea.width * 0.27f, mapArea.y + mapArea.height * 0.88f},
                 Color{35, 52, 78, 140});
    // Europe
    DrawTriangle(Vector2{mapArea.x + mapArea.width * 0.45f, mapArea.y + mapArea.height * 0.20f},
                 Vector2{mapArea.x + mapArea.width * 0.56f, mapArea.y + mapArea.height * 0.35f},
                 Vector2{mapArea.x + mapArea.width * 0.46f, mapArea.y + mapArea.height * 0.42f},
                 Color{40, 60, 90, 180});
    // Africa
    DrawTriangle(Vector2{mapArea.x + mapArea.width * 0.48f, mapArea.y + mapArea.height * 0.45f},
                 Vector2{mapArea.x + mapArea.width * 0.60f, mapArea.y + mapArea.height * 0.52f},
                 Vector2{mapArea.x + mapArea.width * 0.53f, mapArea.y + mapArea.height * 0.82f},
                 Color{35, 52, 78, 140});
    // Asia / Siberia
    DrawTriangle(Vector2{mapArea.x + mapArea.width * 0.55f, mapArea.y + mapArea.height * 0.18f},
                 Vector2{mapArea.x + mapArea.width * 0.88f, mapArea.y + mapArea.height * 0.28f},
                 Vector2{mapArea.x + mapArea.width * 0.68f, mapArea.y + mapArea.height * 0.52f},
                 Color{40, 62, 92, 180});
}

void WorldMapModal::Draw(const Core::FacilityManager& facilityManager, const Core::EconomyManager& economy) const {
    if (!m_isOpen) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Dark semi-transparent modal backdrop
    DrawRectangle(0, 0, screenW, screenH, Color{0, 0, 0, 180});

    float modalW = std::min(980.0f, screenW - 40.0f);
    float modalH = std::min(640.0f, screenH - 50.0f);
    float modalX = (screenW - modalW) * 0.5f;
    float modalY = (screenH - modalH) * 0.5f;
    Rectangle modalRec{modalX, modalY, modalW, modalH};

    // Modal base card
    DrawRectangleRounded(modalRec, 0.03f, 8, Color{16, 20, 32, 255});
    DrawRectangleRoundedLines(modalRec, 0.03f, 8, 2.0f, Color{45, 65, 100, 255});

    // Header Title
    UIFrame::DrawTextCustom(Core::LocalizationManager::Tr("FAC_MAP_TITLE"), modalX + 24.0f, modalY + 18.0f, 20.0f, Color{255, 220, 80, 255}, true);
    UIFrame::DrawTextCustom(Core::LocalizationManager::Tr("FAC_MAP_SUBTITLE"), modalX + 24.0f, modalY + 44.0f, 13.0f, Color{140, 160, 190, 255}, false);

    // Close button
    m_btnClose.Draw();

    const auto& facilities = facilityManager.GetAllFacilities();

    // Left map area & Right dossier area
    float mapW = (modalW - 50.0f) * 0.60f;
    float mapH = modalH - 160.0f;
    Rectangle mapArea{modalX + 20.0f, modalY + 70.0f, mapW, mapH};

    // 1. Draw World Map Tactical Grid
    DrawWorldMapTacticalGrid(mapArea);

    // 2. Draw Facility Nodes on Map
    for (size_t i = 0; i < facilities.size(); ++i) {
        const auto& fac = facilities[i];
        float nodeX = mapArea.x + fac.mapNormX * mapArea.width;
        float nodeY = mapArea.y + fac.mapNormY * mapArea.height;
        bool isSelected = (i == m_selectedFacilityIndex);
        bool isActive = (i == facilityManager.GetActiveFacilityIndex());

        Color ringColor = isSelected ? Color{255, 220, 60, 255} : (isActive ? Color{0, 240, 160, 255} : (fac.isPurchased ? Color{60, 160, 240, 255} : Color{180, 120, 30, 255}));
        Color fillColor = isSelected ? Color{255, 220, 60, 80} : (isActive ? Color{0, 240, 160, 60} : Color{30, 40, 60, 180});

        DrawCircle(static_cast<int>(nodeX), static_cast<int>(nodeY), 14.0f, fillColor);
        DrawCircleLines(static_cast<int>(nodeX), static_cast<int>(nodeY), 14.0f, ringColor);
        DrawCircle(static_cast<int>(nodeX), static_cast<int>(nodeY), 4.0f, Color{20, 20, 25, 255});

        // City Name Tag
        const char* cityName = Core::LocalizationManager::Tr(fac.countryKey);
        float nameW = UIFrame::MeasureTextCustom(cityName, 11.0f, true);
        DrawRectangle(static_cast<int>(nodeX - nameW / 2.0f - 6.0f), static_cast<int>(nodeY + 16.0f), static_cast<int>(nameW + 12.0f), 18, Color{16, 20, 30, 230});
        UIFrame::DrawTextCustom(cityName, nodeX - nameW / 2.0f, nodeY + 17.0f, 11.0f, RAYWHITE, true);
    }

    // Right Location Dossier Panel
    float infoW = (modalW - 50.0f) * 0.40f;
    float infoX = mapArea.x + mapArea.width + 10.0f;
    float infoY = mapArea.y;
    Rectangle infoArea{infoX, infoY, infoW, mapH};

    DrawRectangleRounded(infoArea, 0.04f, 6, Color{20, 26, 40, 255});
    DrawRectangleRoundedLines(infoArea, 0.04f, 6, 1.5f, Color{55, 80, 115, 255});

    if (m_selectedFacilityIndex < facilities.size()) {
        const auto& fac = facilities[m_selectedFacilityIndex];
        bool isActive = (m_selectedFacilityIndex == facilityManager.GetActiveFacilityIndex());

        // Location Title & Country
        UIFrame::DrawTextCustom(Core::LocalizationManager::Tr(fac.nameKey), infoX + 16.0f, infoY + 12.0f, 16.0f, Color{255, 220, 80, 255}, true);
        UIFrame::DrawTextCustom(Core::LocalizationManager::Tr(fac.countryKey), infoX + 16.0f, infoY + 32.0f, 12.0f, Color{140, 165, 195, 255}, false);

        // Status Badge
        Rectangle statusRec{infoX + infoW - 130.0f, infoY + 12.0f, 115.0f, 22.0f};
        if (isActive) {
            DrawRectangleRounded(statusRec, 0.3f, 4, Color{20, 120, 70, 255});
            UIFrame::DrawTextCustom(Core::LocalizationManager::Tr("FAC_BADGE_ACTIVE"), statusRec.x + 8.0f, statusRec.y + 3.0f, 11.0f, WHITE, true);
        } else if (fac.isPurchased) {
            DrawRectangleRounded(statusRec, 0.3f, 4, Color{30, 80, 140, 255});
            UIFrame::DrawTextCustom(Core::LocalizationManager::Tr("FAC_BADGE_OWNED"), statusRec.x + 12.0f, statusRec.y + 3.0f, 11.0f, WHITE, true);
        } else {
            DrawRectangleRounded(statusRec, 0.3f, 4, Color{130, 90, 20, 255});
            UIFrame::DrawTextCustom(Core::LocalizationManager::Tr("FAC_BADGE_FOR_SALE"), statusRec.x + 10.0f, statusRec.y + 3.0f, 11.0f, WHITE, true);
        }

        // Description
        UIFrame::DrawTextCustom(Core::LocalizationManager::Tr(fac.descKey), infoX + 16.0f, infoY + 56.0f, 11.0f, Color{180, 195, 215, 255}, false);

        // Parameters Badges List
        float paramY = infoY + 98.0f;
        float paramH = 26.0f;
        float gap = 4.0f;

        auto drawParamRow = [&](const char* label, const std::string& val, Color valColor) {
            Rectangle pRec{infoX + 14.0f, paramY, infoW - 28.0f, paramH};
            DrawRectangleRounded(pRec, 0.2f, 4, Color{26, 34, 52, 255});
            DrawRectangleRoundedLines(pRec, 0.2f, 4, 1.0f, Color{45, 60, 85, 200});
            UIFrame::DrawTextCustom(label, pRec.x + 10.0f, pRec.y + 6.0f, 11.0f, Color{150, 170, 195, 255}, false);
            float vw = UIFrame::MeasureTextCustom(val.c_str(), 12.0f, true);
            UIFrame::DrawTextCustom(val.c_str(), pRec.x + pRec.width - vw - 10.0f, pRec.y + 5.0f, 12.0f, valColor, true);
            paramY += paramH + gap;
        };

        // 1. Climate Temperature
        char tempBuf[32];
        snprintf(tempBuf, sizeof(tempBuf), "%.1f C", fac.climateTempCelsius);
        Color tempCol = (fac.climateTempCelsius <= 0.0) ? Color{80, 220, 255, 255} : ((fac.climateTempCelsius > 25.0) ? Color{255, 120, 60, 255} : Color{120, 240, 160, 255});
        drawParamRow(Core::LocalizationManager::Tr("FAC_PARAM_CLIMATE"), tempBuf, tempCol);

        // 2. Maximum Rig Capacity
        char rigBuf[32];
        size_t currentRigs = fac.warehouse ? fac.warehouse->GetRigCount() : 0;
        snprintf(rigBuf, sizeof(rigBuf), "%zu / %zu RIG", currentRigs, fac.maxRigCapacity);
        drawParamRow(Core::LocalizationManager::Tr("FAC_PARAM_CAPACITY"), rigBuf, Color{255, 220, 80, 255});

        // 3. Power Grid Capacity
        char powerBuf[32];
        snprintf(powerBuf, sizeof(powerBuf), "%.0f W (%.0f kW)", fac.baseGridWatts, fac.baseGridWatts / 1000.0);
        drawParamRow(Core::LocalizationManager::Tr("FAC_PARAM_GRID"), powerBuf, Color{255, 170, 40, 255});

        // 4. Electricity Unit Rate
        char rateBuf[32];
        snprintf(rateBuf, sizeof(rateBuf), "$%.2f / kWh", fac.gridPricePerKwh);
        drawParamRow(Core::LocalizationManager::Tr("FAC_PARAM_PRICE"), rateBuf, Color{70, 230, 150, 255});

        // 5. Purchase Cost
        std::string costStr = fac.isPurchased ? Core::LocalizationManager::Tr("FAC_ALREADY_PURCHASED") : economy.FormatFiat(fac.purchaseCostUSD);
        drawParamRow(Core::LocalizationManager::Tr("FAC_PARAM_COST"), costStr, fac.isPurchased ? Color{100, 240, 150, 255} : Color{255, 210, 80, 255});

        // Economist & Arbitrage Management Card
        Rectangle ecoBox{infoX + 14.0f, infoY + 250.0f, infoW - 28.0f, 124.0f};
        DrawRectangleRounded(ecoBox, 0.08f, 4, Color{24, 32, 48, 255});
        DrawRectangleRoundedLines(ecoBox, 0.08f, 4, 1.2f, (fac.economist.level > 0) ? Color{60, 160, 240, 200} : Color{50, 65, 90, 180});

        // Header: Economist Name & Status Badge
        std::string ecoTitle = (fac.economist.level == 0) ? (fac.economist.name + " (Ise Alinabilir)") : (fac.economist.name + " - " + fac.economist.title);
        UIFrame::DrawTextCustom("TESIS FINANS UZMANI", ecoBox.x + 10.0f, ecoBox.y + 8.0f, 11.0f, Color{130, 160, 200, 255}, true);
        
        std::string lvlBadge = (fac.economist.level == 0) ? "PASIF (LV 0)" : ("SEVIYE " + std::to_string(fac.economist.level));
        float lvlW = UIFrame::MeasureTextCustom(lvlBadge.c_str(), 11.0f, true);
        DrawRectangleRounded(Rectangle{ecoBox.x + ecoBox.width - lvlW - 16.0f, ecoBox.y + 6.0f, lvlW + 12.0f, 18.0f}, 0.3f, 4, (fac.economist.level > 0) ? Color{30, 100, 160, 255} : Color{60, 65, 75, 255});
        UIFrame::DrawTextCustom(lvlBadge.c_str(), ecoBox.x + ecoBox.width - lvlW - 10.0f, ecoBox.y + 9.0f, 11.0f, (fac.economist.level > 0) ? Color{100, 220, 255, 255} : Color{180, 180, 190, 255}, true);

        UIFrame::DrawTextCustom(ecoTitle.c_str(), ecoBox.x + 10.0f, ecoBox.y + 28.0f, 13.0f, Color{255, 225, 110, 255}, true);

        // Stats: Win Rate & Hedge Discount & Lifetime PnL
        char statsBuf[128];
        snprintf(statsBuf, sizeof(statsBuf), "Kazanma: %%%.0f  |  Hedge: -%%%.0f Enerji  |  K/Z: %s",
                 fac.economist.winRate * 100.0,
                 fac.economist.hedgeDiscountPercent * 100.0,
                 economy.FormatFiat(fac.economist.totalProfitLifetime).c_str());
        UIFrame::DrawTextCustom(statsBuf, ecoBox.x + 10.0f, ecoBox.y + 50.0f, 11.0f, Color{200, 215, 235, 255}, false);

        // Trade Counts & Last Trade Log
        char tradeStatsBuf[128];
        snprintf(tradeStatsBuf, sizeof(tradeStatsBuf), "Islem Gecmisi: %d Basarili / %d Zarar", fac.economist.successfulTrades, fac.economist.failedTrades);
        UIFrame::DrawTextCustom(tradeStatsBuf, ecoBox.x + 10.0f, ecoBox.y + 70.0f, 11.0f, Color{140, 165, 190, 255}, false);

        std::string logText = fac.economist.lastTradeLog.empty() 
            ? (fac.economist.level == 0 ? "Ekonomist tutuldugunda periyodik arbitraj ve elektrik indirimi kazandirir." : "Piyasa analizi yapiliyor, arbitraj firsati bekleniyor...")
            : ("Son: " + fac.economist.lastTradeLog);
        Color logCol = (fac.economist.lastTradeProfit >= 0.0) ? Color{80, 240, 140, 255} : Color{255, 110, 110, 255};
        if (fac.economist.lastTradeLog.empty()) logCol = Color{150, 170, 190, 255};
        UIFrame::DrawTextCustom(logText.c_str(), ecoBox.x + 10.0f, ecoBox.y + 90.0f, 11.0f, logCol, false);

        // Buttons
        m_btnUpgradeEconomist.Draw();
        m_btnAction.Draw();
    }

    // Bottom Facility Selector Tabs
    for (size_t i = 0; i < facilities.size(); ++i) {
        m_facilityListButtons[i].Draw();
    }
}

} // namespace Render
