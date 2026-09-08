#include "RigRenderer.hpp"
#include "UIFrame.hpp"
#include <cmath>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace Render {

RigRenderer::RigRenderer() = default;

void RigRenderer::DrawSpinningFan(int centerX, int centerY, float radius, float angleDegrees, Color bladeColor) const {
    // Fan motor hub
    DrawCircle(centerX, centerY, radius * 0.35f, DARKGRAY);
    DrawCircleLines(centerX, centerY, radius, GRAY);

    // Draw 4 rotating blades
    constexpr int bladeCount = 4;
    for (int i = 0; i < bladeCount; ++i) {
        float bladeAngle = angleDegrees + (i * 90.0f);
        float rad = bladeAngle * (PI / 180.0f);
        
        float endX = centerX + std::cos(rad) * (radius * 0.9f);
        float endY = centerY + std::sin(rad) * (radius * 0.9f);
        
        DrawLineEx(Vector2{static_cast<float>(centerX), static_cast<float>(centerY)},
                   Vector2{endX, endY}, 3.0f, bladeColor);
    }
}

void RigRenderer::DrawBraidedPCIeCable(int startX, int startY, int endX, int endY) const {
    // 2 örgülü kablo hattı (sarı ve siyah yüksek akım PCIe kabloları)
    for (int offset = -3; offset <= 3; offset += 3) {
        int sx = startX + offset;
        int ex = endX + offset;
        DrawLine(sx, startY, ex, endY, Color{25, 25, 30, 255});
        for (int y = startY; y < endY; y += 6) {
            Color hatchColor = ((y / 6) % 2 == 0) ? Color{255, 210, 0, 255} : Color{45, 45, 55, 255};
            DrawLine(sx - 1, y, sx + 1, y + 2, hatchColor);
        }
    }
    // 8-pin konektör kafası
    DrawRectangle(endX - 8, endY - 4, 16, 6, Color{20, 20, 20, 255});
    DrawRectangleLines(endX - 8, endY - 4, 16, 6, Color{80, 80, 80, 255});
}

void RigRenderer::DrawSmokeAndSparks(int centerX, int centerY, double animTime) const {
    // Yükselen duman halkaları
    for (int i = 0; i < 4; ++i) {
        float phase = std::fmod(static_cast<float>(animTime * 0.9 + i * 0.25), 1.0f);
        float py = centerY - (phase * 65.0f);
        float px = centerX + std::sin(phase * 6.28f + i * 1.5f) * 12.0f;
        float radius = 7.0f + phase * 18.0f;
        unsigned char alpha = static_cast<unsigned char>((1.0f - phase) * 150.0f);
        DrawCircle(static_cast<int>(px), static_cast<int>(py), radius, Color{35, 35, 40, alpha});
    }

    // Kızgın kor kıvılcımları (Sparks)
    for (int i = 0; i < 3; ++i) {
        float sparkPhase = std::fmod(static_cast<float>(animTime * 2.8 + i * 0.35), 1.0f);
        float sy = centerY - (sparkPhase * 80.0f);
        float sx = centerX + std::sin(sparkPhase * 14.0f + i * 2.5f) * 16.0f;
        unsigned char sparkAlpha = static_cast<unsigned char>((1.0f - sparkPhase) * 255.0f);
        Color sparkColor = (i % 2 == 0) ? Color{255, 140, 0, sparkAlpha} : Color{255, 60, 20, sparkAlpha};
        DrawCircle(static_cast<int>(sx), static_cast<int>(sy), 2.5f, sparkColor);
    }
}

void RigRenderer::DrawSingleGPU(const Core::GPU* gpu, double tempCelsius, int x, int y, double animTime,
                                bool rigPoweredOn, bool isBreakerTripped, const TextureManager* textureManager) const {
    constexpr int gpuWidth = 90;
    constexpr int gpuHeight = 220;

    bool isBurnt = gpu && gpu->IsBurnt();
    bool hasActivePower = rigPoweredOn && !isBreakerTripped && !isBurnt;

    // Üst PCIe örgü güç kablosu
    DrawBraidedPCIeCable(x + gpuWidth / 2, y - 24, x + gpuWidth / 2, y + 4);

    // Dinamik Fan Hızı: Fan devri (%20 - %100) arttıkça fan kanatları belirgin şekilde hızlanır!
    // Sigorta attıysa (isBreakerTripped) elektrik kesildiği için fan devri anında 0'a düşer ve durur!
    float fanDuty = (gpu ? static_cast<float>(gpu->GetFanSpeedPercent() / 100.0) : 0.7f);
    float speedMultiplier = hasActivePower ? (fanDuty * 2200.0f) : 0.0f;
    float currentAngle = std::fmod(static_cast<float>(animTime * speedMultiplier), 360.0f);

    // RGB Fan Rengi
    Color fanRgb;
    if (!hasActivePower) {
        fanRgb = Color{60, 70, 80, 255};
    } else if (tempCelsius < 55.0) {
        fanRgb = Color{0, 210, 255, 255}; // Cool Cyan
    } else if (tempCelsius < 75.0) {
        fanRgb = Color{0, 255, 120, 255}; // Warm Green
    } else if (tempCelsius < 85.0) {
        fanRgb = Color{255, 160, 0, 255}; // Hot Orange
    } else {
        fanRgb = Color{255, 30, 30, 255}; // Dangerous Red
    }

    // 1. Ekran Kartı Gövdesi
    if (isBurnt) {
        DrawRectangle(x, y, gpuWidth, gpuHeight, Color{18, 14, 14, 255});
        DrawRectangleLines(x, y, gpuWidth, gpuHeight, Color{140, 25, 25, 255});
    } else if (textureManager && textureManager->HasGPUTexture()) {
        textureManager->DrawGPUTexture(Rectangle{static_cast<float>(x), static_cast<float>(y),
                                                 static_cast<float>(gpuWidth), static_cast<float>(gpuHeight)});
    } else {
        DrawRectangle(x, y, gpuWidth, gpuHeight, Color{30, 32, 38, 255});
        DrawRectangleLines(x, y, gpuWidth, gpuHeight, Color{70, 75, 85, 255});

        for (int finY = y + 15; finY < y + gpuHeight - 15; finY += 6) {
            DrawLine(x + 8, finY, x + gpuWidth - 8, finY, Color{50, 55, 65, 255});
        }
    }

    // 2. Dönen Fanlar (Elektrik varsa ve yanmadıysa döner)
    if (hasActivePower) {
        if (textureManager && textureManager->HasFanTexture()) {
            textureManager->DrawFanTexture(x + gpuWidth / 2.0f, y + 60.0f, 32.0f, currentAngle, fanRgb);
            textureManager->DrawFanTexture(x + gpuWidth / 2.0f, y + 145.0f, 32.0f, -currentAngle, fanRgb);
        } else {
            DrawSpinningFan(x + gpuWidth / 2, y + 60, 32.0f, currentAngle, fanRgb);
            DrawSpinningFan(x + gpuWidth / 2, y + 145, 32.0f, -currentAngle, fanRgb);
        }
    } else if (isBurnt) {
        // Yanmış kartın üstüne alev/arıza etiketi
        DrawRectangle(x + 6, y + 90, gpuWidth - 12, 40, Color{180, 20, 20, 230});
        UIFrame::DrawTextCustom("YANDI!", static_cast<float>(x + 18), static_cast<float>(y + 100), 16.0f, WHITE, true);
        // Duman ve kor kıvılcım efektleri
        DrawSmokeAndSparks(x + gpuWidth / 2, y + 30, animTime);
    } else {
        // Rig kapalıyken veya sigorta attığında durağan duran fanlar
        DrawSpinningFan(x + gpuWidth / 2, y + 60, 32.0f, 0.0f, Color{60, 65, 75, 255});
        DrawSpinningFan(x + gpuWidth / 2, y + 145, 32.0f, 0.0f, Color{60, 65, 75, 255});
    }

    // Status LED
    Color ledColor = !hasActivePower ? Color{50, 55, 65, 255} : (isBurnt ? RED : ((tempCelsius >= 85.0) ? RED : ((tempCelsius >= 70.0) ? ORANGE : GREEN)));
    DrawCircle(x + gpuWidth - 12, y + 12, 4.0f, ledColor);

    // Fare kartın üzerindeyse inceleme çerçevesi
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, Rectangle{static_cast<float>(x), static_cast<float>(y), static_cast<float>(gpuWidth), static_cast<float>(gpuHeight)})) {
        DrawRectangleLinesEx(Rectangle{static_cast<float>(x - 2), static_cast<float>(y - 2), static_cast<float>(gpuWidth + 4), static_cast<float>(gpuHeight + 4)}, 2.0f, Color{0, 240, 255, 220});
        DrawRectangle(x + 10, y + 4, gpuWidth - 20, 18, Color{0, 200, 255, 220});
        UIFrame::DrawTextCustom("INCELE", static_cast<float>(x + 20), static_cast<float>(y + 5), 11.0f, BLACK, true);
    }
}

void RigRenderer::DrawRig(const Core::MiningRig& rig, const Core::ThermalModel& thermalModel,
                         int posX, int posY, double animTime, const TextureManager* textureManager,
                         bool isBreakerTripped) const {
    constexpr int rigWidth = 720;
    constexpr int rigHeight = 320;

    bool isPowered = rig.IsPoweredOn() && !isBreakerTripped;

    // Aluminum frame (metallic open-air rig)
    Color frameBg = isPowered ? Color{20, 22, 26, 240} : Color{14, 15, 18, 240};
    Color frameBorder = isPowered ? Color{100, 110, 130, 255} : (isBreakerTripped ? Color{140, 30, 30, 255} : Color{60, 65, 75, 255});

    DrawRectangleRounded(Rectangle{static_cast<float>(posX), static_cast<float>(posY),
                                   static_cast<float>(rigWidth), static_cast<float>(rigHeight)},
                         0.04f, 6, frameBg);
    DrawRectangleRoundedLines(Rectangle{static_cast<float>(posX), static_cast<float>(posY),
                                        static_cast<float>(rigWidth), static_cast<float>(rigHeight)},
                              0.04f, 6, 2.0f, frameBorder);

    // Top and bottom aluminum support bars
    DrawRectangle(posX + 10, posY + 15, rigWidth - 20, 8, Color{80, 85, 95, 255});
    DrawRectangle(posX + 10, posY + rigHeight - 25, rigWidth - 20, 8, Color{80, 85, 95, 255});

    // Rig header label & status badge
    std::string title = rig.GetName();
    Color titleColor;
    if (isBreakerTripped) {
        title += "  [! SEBEKE KESILDI - SALTER ATTI !]";
        titleColor = Color{255, 60, 60, 255};
    } else if (rig.IsPSUOverloaded()) {
        title += "  [! PSU ASIRI YUKLENDI - YUKSELTIN !]";
        titleColor = Color{255, 120, 20, 255};
    } else if (isPowered) {
        title += "  [AKTIF / CALISIYOR]";
        titleColor = Color{0, 240, 160, 255};
    } else {
        title += "  [KAPALI / DEVRE DISI]";
        titleColor = Color{200, 70, 70, 255};
    }
    UIFrame::DrawTextCustom(title, static_cast<float>(posX + 24), static_cast<float>(posY + 26), 16.0f, titleColor, true);

    // Sağ Üst: Ortalama Sıcaklık ve PSU Güç Rozetleri
    double avgTemp = rig.CalculateAverageTemperature(thermalModel);
    Color tempCol = (avgTemp >= 85.0) ? Color{255, 60, 60, 255} : ((avgTemp >= 70.0) ? Color{255, 160, 30, 255} : Color{60, 230, 140, 255});
    char tempBuf[32];
    snprintf(tempBuf, sizeof(tempBuf), "Ort: %.1f C", avgTemp);

    double rigWatts = isPowered ? rig.CalculateTotalPowerWatts() : 0.0;
    double psuMax = rig.GetPSUMaxWatts();
    char psuBuf[48];
    snprintf(psuBuf, sizeof(psuBuf), "PSU: %.0fW / %.0fW", rigWatts, psuMax);
    Color psuCol = (rigWatts > psuMax) ? Color{255, 50, 50, 255} : ((rigWatts > psuMax * 0.85) ? Color{255, 180, 40, 255} : Color{80, 200, 255, 255});

    float badgeW1 = UIFrame::MeasureTextCustom(tempBuf, 13.0f, true) + 20.0f;
    Rectangle badgeTempRec{static_cast<float>(posX + rigWidth - badgeW1 - 18.0f), static_cast<float>(posY + 23.0f), badgeW1, 24.0f};
    DrawRectangleRounded(badgeTempRec, 0.3f, 4, Color{28, 36, 50, 240});
    DrawRectangleRoundedLines(badgeTempRec, 0.3f, 4, 1.0f, tempCol);
    UIFrame::DrawTextCustom(tempBuf, badgeTempRec.x + 10.0f, badgeTempRec.y + 4.0f, 13.0f, tempCol, true);

    float badgeW2 = UIFrame::MeasureTextCustom(psuBuf, 13.0f, true) + 20.0f;
    Rectangle badgePsuRec{badgeTempRec.x - badgeW2 - 8.0f, static_cast<float>(posY + 23.0f), badgeW2, 24.0f};
    DrawRectangleRounded(badgePsuRec, 0.3f, 4, Color{28, 36, 50, 240});
    DrawRectangleRoundedLines(badgePsuRec, 0.3f, 4, 1.0f, psuCol);
    UIFrame::DrawTextCustom(psuBuf, badgePsuRec.x + 10.0f, badgePsuRec.y + 4.0f, 13.0f, psuCol, true);

    // Draw installed GPUs
    const auto& gpus = rig.GetGPUs();
    constexpr int slotSpacing = 112;
    const int startX = posX + 30;
    const int gpuY = posY + 55;

    for (size_t i = 0; i < rig.GetMaxCapacity(); ++i) {
        int cardX = startX + static_cast<int>(i * slotSpacing);

        if (i < gpus.size() && gpus[i]) {
            double temp = isPowered ? thermalModel.CalculateGPUTemperature(gpus[i]->GetEffectivePowerWatts(), gpus[i]->GetFanSpeedPercent() / 100.0) : thermalModel.GetAmbientTemperature();
            DrawSingleGPU(gpus[i].get(), temp, cardX, gpuY, animTime, rig.IsPoweredOn(), isBreakerTripped, textureManager);
        } else {
            // Empty PCIe slot placeholder
            DrawRectangleLines(cardX, gpuY, 90, 220, Color{45, 50, 60, 180});
            UIFrame::DrawTextCustom("EMPTY", static_cast<float>(cardX + 22), static_cast<float>(gpuY + 100), 14.0f, Color{80, 90, 105, 255}, false);
        }
    }
}

int RigRenderer::GetClickedGPUIndex(int posX, int posY, size_t gpuCount, Vector2 mousePos) const {
    constexpr int slotSpacing = 112;
    const int startX = posX + 30;
    const int gpuY = posY + 55;

    for (size_t i = 0; i < gpuCount; ++i) {
        int cardX = startX + static_cast<int>(i * slotSpacing);
        Rectangle cardRect{static_cast<float>(cardX), static_cast<float>(gpuY), 90.0f, 220.0f};

        if (CheckCollisionPointRec(mousePos, cardRect)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void RigRenderer::DrawWarehouseOverviewGrid(const Core::Warehouse& warehouse, const Core::ThermalModel& thermalModel,
                                           const Rectangle& bounds, double animTime, Vector2 mousePos,
                                           float scrollOffsetY, float& outMaxScrollY,
                                           int& outSelectedRigIndex, int& outToggledRigIndex) const {
    outSelectedRigIndex = -1;
    outToggledRigIndex = -1;

    const auto& rigs = warehouse.GetAllRigs();
    if (rigs.empty()) {
        outMaxScrollY = 0.0f;
        return;
    }

    const float cardH = 92.0f;
    const float cardGap = 10.0f;
    const float totalContentHeight = 20.0f + static_cast<float>(rigs.size()) * (cardH + cardGap);
    outMaxScrollY = std::max(0.0f, totalContentHeight - bounds.height);
    const float effectiveScroll = std::clamp(scrollOffsetY, 0.0f, outMaxScrollY);

    const float startY = bounds.y + 10.0f - effectiveScroll;
    const float cardW = bounds.width - (outMaxScrollY > 0.0f ? 32.0f : 24.0f);
    const float cardX = bounds.x + 12.0f;

    bool mouseInBounds = CheckCollisionPointRec(mousePos, bounds);

    // Kırpma Modu (Scissor): Kartların başlık veya alt barın dışına taşmasını engeller
    BeginScissorMode(static_cast<int>(bounds.x), static_cast<int>(bounds.y),
                     static_cast<int>(bounds.width), static_cast<int>(bounds.height));

    for (size_t i = 0; i < rigs.size(); ++i) {
        const auto* rig = rigs[i].get();
        if (!rig) continue;

        float cy = startY + (static_cast<float>(i) * (cardH + cardGap));

        // Ekran dışındaki kartları hesaplamadan geç (Culling)
        if (cy + cardH < bounds.y || cy > bounds.y + bounds.height) {
            continue;
        }

        Rectangle rigCardRect{cardX, cy, cardW, cardH};
        bool isHovered = mouseInBounds && CheckCollisionPointRec(mousePos, rigCardRect);
        bool isCurrentActive = (i == warehouse.GetActiveRigIndex());
        bool isPowered = rig->IsPoweredOn();

        // Kart arka planı
        Color bg = isCurrentActive ? Color{28, 38, 54, 245} : (isHovered ? Color{24, 30, 42, 240} : Color{18, 22, 30, 230});
        Color border = isCurrentActive ? Color{0, 220, 255, 255} : (isHovered ? Color{100, 140, 180, 255} : Color{45, 55, 75, 255});

        DrawRectangleRounded(rigCardRect, 0.12f, 4, bg);
        DrawRectangleRoundedLines(rigCardRect, 0.12f, 4, isCurrentActive ? 2.0f : 1.0f, border);

        // Sağ Taraf Butonları: [AÇ / KAPAT] ve [İNCELE] (Garantili Sağ Kenar Hizalama)
        const float btnW = 82.0f;
        const float btnGap = 8.0f;
        const float rightMargin = 16.0f;
        const float btnInspectX = cardX + cardW - rightMargin - btnW;
        const float btnPowerX = btnInspectX - btnGap - btnW;
        const float btnY = cy + (cardH - 38.0f) / 2.0f;

        Rectangle btnPowerRect{btnPowerX, btnY, btnW, 38.0f};
        Rectangle btnInspectRect{btnInspectX, btnY, btnW, 38.0f};

        // Butonlardan önceki içerik için maksimum güvenli X koordinatı
        const float contentMaxX = btnPowerX - 14.0f;

        // 1. Üst Satır (Sol): Rig Adı
        std::string rigTitle = "#" + std::to_string(i + 1) + " " + rig->GetName();
        UIFrame::DrawTextCustom(rigTitle, cardX + 16.0f, cy + 12.0f, 16.0f, WHITE, true);
        float titleW = UIFrame::MeasureTextCustom(rigTitle, 16.0f, true);

        // Durum Rozeti (Rig Adının hemen yanında)
        std::string statusText = isPowered ? "[CALISIYOR]" : "[KAPALI]";
        Color statusCol = isPowered ? Color{0, 240, 140, 255} : Color{255, 100, 100, 255};
        if (rig->IsPSUOverloaded()) {
            statusText = "[PSU ASIRI YUK]";
            statusCol = Color{255, 140, 30, 255};
        }
        float statusW = UIFrame::MeasureTextCustom(statusText, 11.0f, true) + 12.0f;
        float statusX = cardX + 26.0f + titleW;
        if (statusX + statusW < contentMaxX - 100.0f) {
            Rectangle sBadgeRec{statusX, cy + 11.0f, statusW, 20.0f};
            DrawRectangleRounded(sBadgeRec, 0.3f, 4, Color{20, 28, 38, 220});
            DrawRectangleRoundedLines(sBadgeRec, 0.3f, 4, 1.0f, statusCol);
            UIFrame::DrawTextCustom(statusText, sBadgeRec.x + 6.0f, sBadgeRec.y + 3.0f, 11.0f, statusCol, true);
        }

        // 1. Üst Satır (Sağ): PSU Güç Durumu
        double psuWatts = isPowered ? rig->CalculateTotalPowerWatts() : 0.0;
        double psuMax = rig->GetPSUMaxWatts();
        std::string psuStr = "PSU: " + std::to_string(static_cast<int>(psuWatts)) + "W / " + std::to_string(static_cast<int>(psuMax)) + "W";
        float psuW = UIFrame::MeasureTextCustom(psuStr, 12.0f, false);
        if (contentMaxX - psuW > cardX + 250.0f) {
            Color psuColor = (psuWatts > psuMax) ? Color{255, 60, 60, 255} : Color{130, 175, 215, 255};
            UIFrame::DrawTextCustom(psuStr, contentMaxX - psuW, cy + 14.0f, 12.0f, psuColor, false);
        }

        // 2. Alt Satır: 6 Adet Mini GPU Yuvası (Kompakt)
        const float slotStartX = cardX + 16.0f;
        const float slotY = cy + 44.0f;
        const float slotBoxW = 24.0f;
        const float slotBoxH = 32.0f;
        const float slotGap = 4.0f;

        const auto& gpus = rig->GetGPUs();
        double maxTemp = 0.0;

        for (size_t s = 0; s < rig->GetMaxCapacity(); ++s) {
            Rectangle sRect{slotStartX + s * (slotBoxW + slotGap), slotY, slotBoxW, slotBoxH};
            if (s < gpus.size() && gpus[s]) {
                const auto* gpu = gpus[s].get();
                double temp = isPowered ? thermalModel.CalculateGPUTemperature(gpu->GetEffectivePowerWatts(), 0.85) : 22.0;
                if (temp > maxTemp) maxTemp = temp;

                Color slotBg = Color{25, 30, 40, 255};
                Color slotBorder = Color{0, 220, 255, 255};
                const char* icon = "GPU";

                if (gpu->IsBurnt()) {
                    slotBg = Color{80, 15, 15, 255};
                    slotBorder = RED;
                    icon = "YAN";
                } else if (temp >= 85.0) {
                    slotBg = Color{70, 45, 10, 255};
                    slotBorder = ORANGE;
                } else if (!isPowered) {
                    slotBorder = GRAY;
                }

                DrawRectangleRounded(sRect, 0.2f, 3, slotBg);
                DrawRectangleRoundedLines(sRect, 0.2f, 3, 1.2f, slotBorder);
                UIFrame::DrawTextCustom(icon, sRect.x + 2.0f, sRect.y + 9.0f, 10.0f, slotBorder, true);
            } else {
                // Boş slot
                DrawRectangleRounded(sRect, 0.2f, 3, Color{15, 18, 24, 255});
                DrawRectangleRoundedLines(sRect, 0.2f, 3, 1.0f, Color{40, 45, 55, 255});
                UIFrame::DrawTextCustom("-", sRect.x + 9.0f, sRect.y + 8.0f, 11.0f, Color{70, 75, 85, 255}, false);
            }
        }

        // 2. Alt Satır (Orta): Metrikler (Hashrate, Watt, Max Isı) - Asla butonların üzerine taşmaz!
        const float metricsX = slotStartX + 6.0f * (slotBoxW + slotGap) + 14.0f;
        std::ostringstream ssM;
        ssM << "Kazim: " << std::fixed << std::setprecision(1) << rig->CalculateTotalHashrate() << " MH/s   |   "
            << "Guc: " << static_cast<int>(rig->CalculateTotalPowerWatts()) << "W   |   "
            << "Max: " << static_cast<int>(maxTemp) << " C";
        UIFrame::DrawTextCustom(ssM.str(), metricsX, cy + 50.0f, 13.0f, Color{175, 205, 235, 255}, false);

        // Buton Tıklama ve Hover Durumları
        bool hoverPower = mouseInBounds && CheckCollisionPointRec(mousePos, btnPowerRect);
        bool hoverInspect = mouseInBounds && CheckCollisionPointRec(mousePos, btnInspectRect);

        // Güç Butonu
        Color pBtnBg = isPowered ? (hoverPower ? Color{100, 30, 30, 255} : Color{70, 25, 25, 255})
                                 : (hoverPower ? Color{25, 90, 50, 255} : Color{20, 65, 35, 255});
        DrawRectangleRounded(btnPowerRect, 0.25f, 4, pBtnBg);
        DrawRectangleRoundedLines(btnPowerRect, 0.25f, 4, 1.2f, isPowered ? RED : GREEN);
        float pTxtW = UIFrame::MeasureTextCustom(isPowered ? "KAPAT" : "AC", 13.0f, true);
        UIFrame::DrawTextCustom(isPowered ? "KAPAT" : "AC", btnPowerRect.x + (btnW - pTxtW) / 2.0f, btnPowerRect.y + 11.0f, 13.0f, WHITE, true);

        // İncele / Seç Butonu
        Color iBtnBg = hoverInspect ? Color{35, 55, 85, 255} : Color{25, 40, 65, 255};
        DrawRectangleRounded(btnInspectRect, 0.25f, 4, iBtnBg);
        DrawRectangleRoundedLines(btnInspectRect, 0.25f, 4, 1.2f, Color{0, 220, 255, 255});
        float iTxtW = UIFrame::MeasureTextCustom("INCELE", 13.0f, true);
        UIFrame::DrawTextCustom("INCELE", btnInspectRect.x + (btnW - iTxtW) / 2.0f, btnInspectRect.y + 11.0f, 13.0f, Color{0, 220, 255, 255}, true);

        // Tıklama Kontrolleri
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouseInBounds) {
            if (hoverPower) {
                outToggledRigIndex = static_cast<int>(i);
            } else if (hoverInspect || (isHovered && !hoverPower && !hoverInspect)) {
                outSelectedRigIndex = static_cast<int>(i);
            }
        }
    }

    EndScissorMode();

    // Dikey Neon Kaydırma Çubuğu (Scrollbar)
    if (outMaxScrollY > 0.0f) {
        const float sbX = bounds.x + bounds.width - 12.0f;
        const float sbY = bounds.y + 6.0f;
        const float sbH = bounds.height - 12.0f;
        const float sbW = 6.0f;

        // İz (Track)
        DrawRectangleRounded(Rectangle{sbX, sbY, sbW, sbH}, 0.5f, 4, Color{20, 25, 35, 200});

        // Kaydırma Butonu (Thumb)
        float thumbH = std::max(28.0f, sbH * (bounds.height / totalContentHeight));
        float scrollRatio = effectiveScroll / outMaxScrollY;
        float thumbY = sbY + scrollRatio * (sbH - thumbH);

        DrawRectangleRounded(Rectangle{sbX, thumbY, sbW, thumbH}, 0.5f, 4, Color{0, 220, 255, 230});
    }
}

void RigRenderer::DrawQuickRigSelector(const Core::Warehouse& warehouse, const Core::ThermalModel& thermalModel,
                                      const Rectangle& bounds, Vector2 mousePos, int& outSelectedRigIndex) const {
    outSelectedRigIndex = -1;
    const auto& rigs = warehouse.GetAllRigs();
    if (rigs.empty()) return;

    const size_t activeIdx = warehouse.GetActiveRigIndex();
    const float pillH = bounds.height;
    const float pillGap = 6.0f;
    const float pillW = std::clamp((bounds.width - (rigs.size() - 1) * pillGap) / static_cast<float>(rigs.size()), 70.0f, 130.0f);

    for (size_t i = 0; i < rigs.size(); ++i) {
        float px = bounds.x + (static_cast<float>(i) * (pillW + pillGap));
        if (px + pillW > bounds.x + bounds.width) break;

        Rectangle pillRect{px, bounds.y, pillW, pillH};
        bool isHovered = CheckCollisionPointRec(mousePos, pillRect);
        bool isActive = (i == activeIdx);
        bool isPowered = rigs[i]->IsPoweredOn();

        Color bg = isActive ? Color{35, 55, 80, 255} : (isHovered ? Color{25, 35, 50, 240} : Color{16, 20, 28, 230});
        Color border = isActive ? Color{0, 230, 255, 255} : (isPowered ? Color{50, 80, 110, 255} : Color{80, 35, 35, 255});

        DrawRectangleRounded(pillRect, 0.25f, 4, bg);
        DrawRectangleRoundedLines(pillRect, 0.25f, 4, isActive ? 1.8f : 1.0f, border);

        std::string label = "Rig " + std::to_string(i + 1);
        Color textColor = isActive ? Color{0, 240, 255, 255} : (isPowered ? RAYWHITE : Color{255, 100, 100, 255});
        UIFrame::DrawTextCustom(label, px + 8.0f, bounds.y + 4.0f, 12.0f, textColor, true);

        double avgT = rigs[i]->CalculateAverageTemperature(thermalModel);
        std::string sub = isPowered ? (std::to_string(static_cast<int>(rigs[i]->CalculateTotalHashrate())) + "M | " + std::to_string(static_cast<int>(avgT)) + "C") : "OFF";
        UIFrame::DrawTextCustom(sub, px + 8.0f, bounds.y + 18.0f, 11.0f, isPowered ? Color{100, 220, 150, 255} : Color{180, 80, 80, 255}, false);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isHovered) {
            outSelectedRigIndex = static_cast<int>(i);
        }
    }
}

} // namespace Render
