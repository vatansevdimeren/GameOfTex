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
      m_btnAction(Rectangle{0, 0, 0, 0}, "SEC", "", Color{30, 80, 60, 255}, Color{40, 220, 140, 255})
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

    // Update Action Button
    float infoW = (modalW - 50.0f) * 0.40f;
    float infoX = mapArea.x + mapArea.width + 10.0f;
    float infoY = mapArea.y;
    Rectangle btnActionRec{infoX + 15.0f, infoY + mapH - 50.0f, infoW - 30.0f, 42.0f};
    m_btnAction.SetBounds(btnActionRec);

    if (m_selectedFacilityIndex < facilities.size()) {
        const auto& selFac = facilities[m_selectedFacilityIndex];
        bool isActive = (m_selectedFacilityIndex == facilityManager.GetActiveFacilityIndex());

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
        float ly = mapArea.y + (mapArea.height / 6.0f) * y;
        DrawLineEx(Vector2{mapArea.x + 5.0f, ly}, Vector2{mapArea.x + mapArea.width - 5.0f, ly}, 1.0f, Color{28, 42, 60, 160});
    }
    for (int x = 1; x < 8; ++x) {
        float lx = mapArea.x + (mapArea.width / 8.0f) * x;
        DrawLineEx(Vector2{lx, mapArea.y + 5.0f}, Vector2{lx, mapArea.y + mapArea.height - 5.0f}, 1.0f, Color{28, 42, 60, 160});
    }

    // World Map Continents Simplified Vector Silhouettes
    Color landColor = Color{26, 38, 54, 255};
    Color landBorder = Color{48, 70, 98, 255};

    // North America
    Rectangle na{mapArea.x + mapArea.width * 0.12f, mapArea.y + mapArea.height * 0.22f, mapArea.width * 0.22f, mapArea.height * 0.35f};
    DrawRectangleRounded(na, 0.3f, 4, landColor);
    DrawRectangleRoundedLines(na, 0.3f, 4, 1.0f, landBorder);

    // Europe
    Rectangle eu{mapArea.x + mapArea.width * 0.44f, mapArea.y + mapArea.height * 0.18f, mapArea.width * 0.16f, mapArea.height * 0.26f};
    DrawRectangleRounded(eu, 0.3f, 4, landColor);
    DrawRectangleRoundedLines(eu, 0.3f, 4, 1.0f, landBorder);

    // Asia / Siberia
    Rectangle as{mapArea.x + mapArea.width * 0.60f, mapArea.y + mapArea.height * 0.15f, mapArea.width * 0.32f, mapArea.height * 0.42f};
    DrawRectangleRounded(as, 0.3f, 4, landColor);
    DrawRectangleRoundedLines(as, 0.3f, 4, 1.0f, landBorder);

    // Dynamic scanning radar sweep line
    float scanPhase = std::fmod(static_cast<float>(GetTime()) * 0.25f, 1.0f);
    float scanX = mapArea.x + scanPhase * mapArea.width;
    DrawLineEx(Vector2{scanX, mapArea.y + 4.0f}, Vector2{scanX, mapArea.y + mapArea.height - 4.0f}, 2.0f, Color{0, 220, 255, 90});
}

void WorldMapModal::Draw(const Core::FacilityManager& facilityManager, const Core::EconomyManager& economy) const {
    if (!m_isOpen) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Dark backdrop
    DrawRectangle(0, 0, screenW, screenH, Color{8, 10, 15, 225});

    float modalW = std::min(980.0f, screenW - 40.0f);
    float modalH = std::min(640.0f, screenH - 50.0f);
    float modalX = (screenW - modalW) * 0.5f;
    float modalY = (screenH - modalH) * 0.5f;

    // Modal Outer Frame
    Rectangle modalRec{modalX, modalY, modalW, modalH};
    DrawRectangleRounded(modalRec, 0.03f, 8, Color{16, 20, 30, 252});
    DrawRectangleRoundedLines(modalRec, 0.03f, 8, 2.0f, Color{40, 160, 240, 230});

    // Top Header Banner
    DrawRectangleRounded(Rectangle{modalX + 10.0f, modalY + 10.0f, modalW - 20.0f, 48.0f}, 0.08f, 6, Color{24, 32, 50, 255});
    DrawRectangleRoundedLines(Rectangle{modalX + 10.0f, modalY + 10.0f, modalW - 20.0f, 48.0f}, 0.08f, 6, 1.0f, Color{70, 130, 210, 140});

    const char* titleText = Core::LocalizationManager::Tr("WORLD_MAP_TITLE");
    DrawText(titleText, static_cast<int>(modalX + 25.0f), static_cast<int>(modalY + 22.0f), 22, Color{0, 230, 255, 255});

    m_btnClose.Draw();

    // Map Area & Info Panel
    float mapW = (modalW - 50.0f) * 0.60f;
    float mapH = modalH - 160.0f;
    Rectangle mapArea{modalX + 20.0f, modalY + 70.0f, mapW, mapH};

    DrawWorldMapTacticalGrid(mapArea);

    const auto& facilities = facilityManager.GetAllFacilities();

    // Draw connection lines between facilities
    for (size_t i = 0; i + 1 < facilities.size(); ++i) {
        float x1 = mapArea.x + facilities[i].mapNormX * mapArea.width;
        float y1 = mapArea.y + facilities[i].mapNormY * mapArea.height;
        float x2 = mapArea.x + facilities[i + 1].mapNormX * mapArea.width;
        float y2 = mapArea.y + facilities[i + 1].mapNormY * mapArea.height;
        DrawLineEx(Vector2{x1, y1}, Vector2{x2, y2}, 1.2f, Color{50, 80, 120, 120});
    }

    // Draw Facility Nodes on Map
    for (size_t i = 0; i < facilities.size(); ++i) {
        const auto& fac = facilities[i];
        float nodeX = mapArea.x + fac.mapNormX * mapArea.width;
        float nodeY = mapArea.y + fac.mapNormY * mapArea.height;

        bool isActive = (i == facilityManager.GetActiveFacilityIndex());
        bool isSelected = (i == m_selectedFacilityIndex);

        Color nodeColor = Color{240, 180, 40, 255};
        if (isActive) nodeColor = Color{0, 240, 160, 255};
        else if (fac.isPurchased) nodeColor = Color{60, 180, 255, 255};

        // Concentric pulse if selected or active
        if (isActive || isSelected) {
            float pulseRadius = 14.0f + 6.0f * std::sin(static_cast<float>(GetTime()) * 4.0f);
            DrawCircleLines(static_cast<int>(nodeX), static_cast<int>(nodeY), pulseRadius, nodeColor);
        }

        DrawCircle(static_cast<int>(nodeX), static_cast<int>(nodeY), 9.0f, nodeColor);
        DrawCircle(static_cast<int>(nodeX), static_cast<int>(nodeY), 4.0f, Color{20, 20, 25, 255});

        // City Name Tag
        const char* cityName = Core::LocalizationManager::Tr(fac.countryKey);
        int nameW = MeasureText(cityName, 11);
        DrawRectangle(static_cast<int>(nodeX - nameW / 2.0f - 4.0f), static_cast<int>(nodeY + 12.0f), nameW + 8, 16, Color{15, 20, 30, 220});
        DrawText(cityName, static_cast<int>(nodeX - nameW / 2.0f), static_cast<int>(nodeY + 14.0f), 11, RAYWHITE);
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
        DrawText(Core::LocalizationManager::Tr(fac.nameKey), static_cast<int>(infoX + 16.0f), static_cast<int>(infoY + 16.0f), 18, Color{255, 220, 80, 255});
        DrawText(Core::LocalizationManager::Tr(fac.countryKey), static_cast<int>(infoX + 16.0f), static_cast<int>(infoY + 40.0f), 13, Color{140, 165, 195, 255});

        // Status Badge
        Rectangle statusRec{infoX + infoW - 130.0f, infoY + 14.0f, 115.0f, 24.0f};
        if (isActive) {
            DrawRectangleRounded(statusRec, 0.3f, 4, Color{20, 120, 70, 255});
            DrawText(Core::LocalizationManager::Tr("FAC_BADGE_ACTIVE"), static_cast<int>(statusRec.x + 8.0f), static_cast<int>(statusRec.y + 5.0f), 12, WHITE);
        } else if (fac.isPurchased) {
            DrawRectangleRounded(statusRec, 0.3f, 4, Color{30, 80, 140, 255});
            DrawText(Core::LocalizationManager::Tr("FAC_BADGE_OWNED"), static_cast<int>(statusRec.x + 12.0f), static_cast<int>(statusRec.y + 5.0f), 12, WHITE);
        } else {
            DrawRectangleRounded(statusRec, 0.3f, 4, Color{130, 90, 20, 255});
            DrawText(Core::LocalizationManager::Tr("FAC_BADGE_FOR_SALE"), static_cast<int>(statusRec.x + 10.0f), static_cast<int>(statusRec.y + 5.0f), 12, WHITE);
        }

        // Description
        DrawText(Core::LocalizationManager::Tr(fac.descKey), static_cast<int>(infoX + 16.0f), static_cast<int>(infoY + 68.0f), 12, Color{180, 195, 215, 255});

        // Parameters Badges List
        float paramY = infoY + 115.0f;
        float paramH = 34.0f;
        float gap = 8.0f;

        auto drawParamRow = [&](const char* label, const std::string& val, Color valColor) {
            Rectangle pRec{infoX + 14.0f, paramY, infoW - 28.0f, paramH};
            DrawRectangleRounded(pRec, 0.2f, 4, Color{26, 34, 52, 255});
            DrawRectangleRoundedLines(pRec, 0.2f, 4, 1.0f, Color{45, 60, 85, 200});
            DrawText(label, static_cast<int>(pRec.x + 12.0f), static_cast<int>(pRec.y + 9.0f), 13, Color{150, 170, 195, 255});
            int vw = MeasureText(val.c_str(), 14);
            DrawText(val.c_str(), static_cast<int>(pRec.x + pRec.width - vw - 12.0f), static_cast<int>(pRec.y + 8.0f), 14, valColor);
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

        // Action Button
        m_btnAction.Draw();
    }

    // Bottom Facility Selector Tabs
    for (size_t i = 0; i < facilities.size(); ++i) {
        m_facilityListButtons[i].Draw();
    }
}

} // namespace Render
