#include "RigRenderer.hpp"
#include "UIFrame.hpp"
#include "../core/EconomyManager.hpp"
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
    // Yükselen duman halkaları (GPU sınırları içinde kalır)
    for (int i = 0; i < 4; ++i) {
        float phase = std::fmod(static_cast<float>(animTime * 0.9 + i * 0.25), 1.0f);
        float py = centerY - (phase * 35.0f);
        float px = centerX + std::sin(phase * 6.28f + i * 1.5f) * 8.0f;
        float radius = 5.0f + phase * 12.0f;
        unsigned char alpha = static_cast<unsigned char>((1.0f - phase) * 140.0f);
        DrawCircle(static_cast<int>(px), static_cast<int>(py), radius, Color{35, 35, 40, alpha});
    }

    // Kızgın kor kıvılcımları (Sparks - GPU kartı içinde kalır, üst başlığa taşmaz)
    for (int i = 0; i < 3; ++i) {
        float sparkPhase = std::fmod(static_cast<float>(animTime * 2.8 + i * 0.35), 1.0f);
        float sy = centerY - (sparkPhase * 36.0f);
        float sx = centerX + std::sin(sparkPhase * 14.0f + i * 2.5f) * 10.0f;
        unsigned char sparkAlpha = static_cast<unsigned char>((1.0f - sparkPhase) * 255.0f);
        Color sparkColor = (i % 2 == 0) ? Color{255, 140, 0, sparkAlpha} : Color{255, 60, 20, sparkAlpha};
        DrawCircle(static_cast<int>(sx), static_cast<int>(sy), 2.2f, sparkColor);
    }
}

void RigRenderer::DrawSingleGPU(const Core::GPU* gpu, double tempCelsius, int x, int y, double animTime,
                                bool rigPoweredOn, bool isBreakerTripped, const TextureManager* textureManager,
                                int customWidth) const {
    const int gpuWidth = customWidth;
    constexpr int gpuHeight = 220;
    const float fanRadius = (gpuWidth >= 80) ? 32.0f : ((gpuWidth >= 65) ? 25.0f : 20.0f);

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

    // 1. Ekran Karti Govdesi (Klasik Prosedurel Metalik Tasarim)
    if (isBurnt) {
        DrawRectangle(x, y, gpuWidth, gpuHeight, Color{18, 14, 14, 255});
        DrawRectangleLines(x, y, gpuWidth, gpuHeight, Color{140, 25, 25, 255});
    } else {
        DrawRectangle(x, y, gpuWidth, gpuHeight, Color{30, 32, 38, 255});
        DrawRectangleLines(x, y, gpuWidth, gpuHeight, Color{70, 75, 85, 255});

        // Aluminyum sogutma bloklari (Heatsink fins)
        for (int finY = y + 15; finY < y + gpuHeight - 15; finY += 6) {
            DrawLine(x + 8, finY, x + gpuWidth - 8, finY, Color{50, 55, 65, 255});
        }
    }

    // 2. Donen RGB Fanlar (Elektrik varsa ve yanmadiysa doner)
    if (hasActivePower) {
        DrawSpinningFan(x + gpuWidth / 2, y + 60, fanRadius, currentAngle, fanRgb);
        DrawSpinningFan(x + gpuWidth / 2, y + 145, fanRadius, -currentAngle, fanRgb);
    } else if (isBurnt) {
        // Yanmis kartin ustune alev/ariza etiketi
        DrawRectangle(x + 4, y + 90, gpuWidth - 8, 40, Color{180, 20, 20, 230});
        UIFrame::DrawTextCustom("YANDI!", static_cast<float>(x + (gpuWidth - 54) / 2), static_cast<float>(y + 100), 15.0f, WHITE, true);
        // Duman ve kor kivilcim efektleri
        DrawSmokeAndSparks(x + gpuWidth / 2, y + 30, animTime);
    } else {
        // Rig kapaliyken veya sigorta attiginda duragan duran fanlar
        DrawSpinningFan(x + gpuWidth / 2, y + 60, fanRadius, 0.0f, Color{60, 65, 75, 255});
        DrawSpinningFan(x + gpuWidth / 2, y + 145, fanRadius, 0.0f, Color{60, 65, 75, 255});
    }

    // Status LED
    Color ledColor = !hasActivePower ? Color{50, 55, 65, 255} : (isBurnt ? RED : ((tempCelsius >= 85.0) ? RED : ((tempCelsius >= 70.0) ? ORANGE : GREEN)));
    DrawCircle(x + gpuWidth - 10, y + 12, 3.5f, ledColor);

    // Fare kartın üzerindeyse inceleme çerçevesi
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, Rectangle{static_cast<float>(x), static_cast<float>(y), static_cast<float>(gpuWidth), static_cast<float>(gpuHeight)})) {
        DrawRectangleLinesEx(Rectangle{static_cast<float>(x - 2), static_cast<float>(y - 2), static_cast<float>(gpuWidth + 4), static_cast<float>(gpuHeight + 4)}, 2.0f, Color{0, 240, 255, 220});
        DrawRectangle(x + 4, y + 4, gpuWidth - 8, 18, Color{0, 200, 255, 220});
        float tW = UIFrame::MeasureTextCustom("INCELE", 11.0f, true);
        UIFrame::DrawTextCustom("INCELE", static_cast<float>(x + (gpuWidth - tW) / 2.0f), static_cast<float>(y + 5), 11.0f, BLACK, true);
    }
}

void RigRenderer::DrawRig(const Core::MiningRig& rig, const Core::ThermalModel& thermalModel,
                         int posX, int posY, double animTime, const TextureManager* textureManager,
                         bool isBreakerTripped, const std::string& synergyLabel) const {
    constexpr int rigWidth = 720;
    constexpr int rigHeight = 320;

    bool isPowered = rig.IsPoweredOn() && !isBreakerTripped;
    int rigLevel = rig.GetRigLevel();
    size_t capacity = rig.GetMaxCapacity();

    // 1. Kasa Seviyesine (Level 1-5) Göre Özel Şasi Tasarımı
    Color frameBg;
    Color frameBorder;
    Color barColor;

    if (rigLevel == 1) {
        // LEVEL 1: Ahşap Garaj Kasası (Doğal Meşe & Pirinç Köşebentler)
        frameBg = Color{38, 26, 16, 245};
        frameBorder = isBreakerTripped ? Color{160, 40, 40, 255} : (isPowered ? Color{195, 130, 50, 255} : Color{110, 75, 40, 255});
        barColor = Color{145, 95, 45, 255};
    } else if (rigLevel == 2) {
        // LEVEL 2: Alüminyum Açık Kasa (Mavi-Gri Eloksallı Profiller)
        frameBg = Color{22, 27, 36, 245};
        frameBorder = isBreakerTripped ? Color{160, 40, 40, 255} : (isPowered ? Color{90, 145, 195, 255} : Color{60, 75, 95, 255});
        barColor = Color{85, 115, 150, 255};
    } else if (rigLevel == 3) {
        // LEVEL 3: Çelik Endüstriyel Pro Şasi (Mat Karbon & Neon Mavi Çizgiler)
        frameBg = Color{18, 20, 26, 250};
        frameBorder = isBreakerTripped ? Color{180, 30, 30, 255} : (isPowered ? Color{0, 210, 255, 255} : Color{50, 65, 80, 255});
        barColor = Color{45, 55, 70, 255};
    } else if (rigLevel == 4) {
        // LEVEL 4: 4U Sunucu Kabini (Endüstriyel Grafit, Turuncu Raylar & Delta Fanlar)
        frameBg = Color{14, 16, 20, 255};
        frameBorder = isBreakerTripped ? Color{200, 40, 40, 255} : (isPowered ? Color{255, 140, 30, 255} : Color{80, 60, 40, 255});
        barColor = Color{35, 38, 48, 255};
    } else {
        // LEVEL 5: Kriyojenik Sıvı Daldırma Tankı (Dielektrik Sıvı & Neon Aqua Ultraviyole)
        frameBg = Color{10, 30, 46, 235};
        frameBorder = isBreakerTripped ? Color{220, 50, 50, 255} : (isPowered ? Color{0, 255, 220, 255} : Color{40, 90, 105, 255});
        barColor = Color{0, 190, 220, 255};
    }

    // Ana Şasi Dikdörtgeni
    DrawRectangleRounded(Rectangle{static_cast<float>(posX), static_cast<float>(posY),
                                   static_cast<float>(rigWidth), static_cast<float>(rigHeight)},
                         0.04f, 6, frameBg);
    DrawRectangleRoundedLines(Rectangle{static_cast<float>(posX), static_cast<float>(posY),
                                        static_cast<float>(rigWidth), static_cast<float>(rigHeight)},
                              0.04f, 6, 2.0f, frameBorder);

    // Seviye 1 (Ahşap) için Ahşap Damar Hatları ve Pirinç Köşe Vidaları
    if (rigLevel == 1) {
        for (int yLine = posY + 35; yLine < posY + rigHeight - 35; yLine += 18) {
            DrawLine(posX + 12, yLine, posX + rigWidth - 12, yLine, Color{54, 38, 24, 120});
        }
        // Pirinç köşebentler
        DrawRectangle(posX + 4, posY + 4, 14, 14, Color{195, 140, 60, 255});
        DrawRectangle(posX + rigWidth - 18, posY + 4, 14, 14, Color{195, 140, 60, 255});
        DrawRectangle(posX + 4, posY + rigHeight - 18, 14, 14, Color{195, 140, 60, 255});
        DrawRectangle(posX + rigWidth - 18, posY + rigHeight - 18, 14, 14, Color{195, 140, 60, 255});
    }

    // Seviye 5 (Kriyojenik Daldırma Tankı) için Yükselen Sıvı Kabarcıkları
    if (rigLevel == 5 && isPowered) {
        for (int b = 0; b < 12; ++b) {
            float phase = std::fmod(static_cast<float>(animTime * 0.8 + b * 0.18), 1.0f);
            float bx = posX + 40.0f + (b * 56.0f);
            float by = posY + rigHeight - 40.0f - (phase * 220.0f);
            float r = 2.5f + (b % 3) * 1.5f;
            unsigned char alpha = static_cast<unsigned char>((1.0f - phase) * 180.0f);
            DrawCircle(static_cast<int>(bx), static_cast<int>(by), r, Color{0, 255, 230, alpha});
        }
    }

    // Üst ve alt destek çubukları / montaj rayları
    DrawRectangle(posX + 10, posY + 15, rigWidth - 20, 8, barColor);
    DrawRectangle(posX + 10, posY + rigHeight - 25, rigWidth - 20, 8, barColor);

    // Seviye 4 (Server Rack) Yan Delta Fan Izgaraları
    if (rigLevel == 4) {
        float fanSpin = isPowered ? std::fmod(static_cast<float>(animTime * 3600.0), 360.0f) : 0.0f;
        DrawSpinningFan(posX + rigWidth - 24, posY + 70, 16.0f, fanSpin, Color{255, 120, 20, 255});
        DrawSpinningFan(posX + rigWidth - 24, posY + 110, 16.0f, -fanSpin, Color{255, 120, 20, 255});
    }

    // Rig başlık etiketi & seviye rozeti
    std::string title = rig.GetName();
    Color titleColor;
    if (isBreakerTripped) {
        title += "  ! SALTER ATTI !";
        titleColor = Color{255, 60, 60, 255};
    } else if (rig.IsPSUOverloaded()) {
        title += "  ! PSU ASIRI YUK !";
        titleColor = Color{255, 120, 20, 255};
    } else if (isPowered) {
        title += " - " + rig.GetRigLevelName();
        titleColor = (rigLevel == 5) ? Color{0, 255, 230, 255} : ((rigLevel == 4) ? Color{255, 160, 40, 255} : Color{0, 240, 160, 255});
    } else {
        title += " - KAPALI";
        titleColor = Color{200, 70, 70, 255};
    }
    UIFrame::DrawTextCustom(title, static_cast<float>(posX + 24), static_cast<float>(posY + 23), 17.5f, titleColor, true);

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

    float badgeW1 = UIFrame::MeasureTextCustom(tempBuf, 14.0f, true) + 20.0f;
    Rectangle badgeTempRec{static_cast<float>(posX + rigWidth - badgeW1 - 18.0f), static_cast<float>(posY + 20.0f), badgeW1, 26.0f};
    DrawRectangleRounded(badgeTempRec, 0.3f, 4, Color{28, 36, 50, 240});
    DrawRectangleRoundedLines(badgeTempRec, 0.3f, 4, 1.0f, tempCol);
    UIFrame::DrawTextCustom(tempBuf, badgeTempRec.x + 10.0f, badgeTempRec.y + 4.5f, 14.0f, tempCol, true);

    float badgeW2 = UIFrame::MeasureTextCustom(psuBuf, 14.0f, true) + 20.0f;
    Rectangle badgePsuRec{badgeTempRec.x - badgeW2 - 8.0f, static_cast<float>(posY + 20.0f), badgeW2, 26.0f};
    DrawRectangleRounded(badgePsuRec, 0.3f, 4, Color{28, 36, 50, 240});
    DrawRectangleRoundedLines(badgePsuRec, 0.3f, 4, 1.0f, psuCol);
    UIFrame::DrawTextCustom(psuBuf, badgePsuRec.x + 10.0f, badgePsuRec.y + 4.5f, 14.0f, psuCol, true);

    // 2. Dinamik Slot Yerleşimi (Kapasiteye göre kart genişliği ve aralığı)
    int cardW = (capacity <= 2) ? 90 : ((capacity <= 4) ? 90 : ((capacity <= 6) ? 90 : ((capacity <= 8) ? 74 : 60)));
    int startX = posX + 30;
    int slotSpacing = 112;
    if (capacity <= 2) {
        startX = posX + 160;
        slotSpacing = 220;
    } else if (capacity <= 4) {
        startX = posX + 70;
        slotSpacing = 150;
    } else if (capacity <= 6) {
        startX = posX + 30;
        slotSpacing = 112;
    } else if (capacity <= 8) {
        startX = posX + 24;
        slotSpacing = 84;
    } else {
        startX = posX + 20;
        slotSpacing = 68;
    }
    const int gpuY = posY + 55;

    const auto& gpus = rig.GetGPUs();
    for (size_t i = 0; i < capacity; ++i) {
        int cardX = startX + static_cast<int>(i * slotSpacing);

        if (i < gpus.size() && gpus[i]) {
            double temp = isPowered ? thermalModel.CalculateGPUTemperature(gpus[i]->GetEffectivePowerWatts(), gpus[i]->GetFanSpeedPercent() / 100.0) : thermalModel.GetAmbientTemperature();
            DrawSingleGPU(gpus[i].get(), temp, cardX, gpuY, animTime, rig.IsPoweredOn(), isBreakerTripped, textureManager, cardW);
        } else {
            // Boş PCIe Yuvası
            DrawRectangleLines(cardX, gpuY, cardW, 220, Color{45, 50, 60, 180});
            float emptyTW = UIFrame::MeasureTextCustom("BOS", 16.0f, true);
            UIFrame::DrawTextCustom("BOS", static_cast<float>(cardX + (cardW - emptyTW) / 2), static_cast<float>(gpuY + 98), 16.0f, Color{130, 150, 180, 255}, true);
        }
    }

    // 3. Kasa Sinerji ve Kombo Rozeti
    if (!synergyLabel.empty()) {
        float synW = UIFrame::MeasureTextCustom(synergyLabel, 13.0f, true) + 16.0f;
        Rectangle synRec{static_cast<float>(posX + 24), static_cast<float>(posY + rigHeight - 26), synW, 22.0f};
        DrawRectangleRounded(synRec, 0.3f, 4, Color{36, 30, 10, 230});
        DrawRectangleRoundedLines(synRec, 0.3f, 4, 1.2f, Color{255, 215, 0, 255});
        UIFrame::DrawTextCustom(synergyLabel, synRec.x + 8.0f, synRec.y + 3.5f, 13.0f, Color{255, 225, 60, 255}, true);
    }

    // 4. CPU Madenciliği ve Anakart Soket Alanı (Sol/Orta Alt Çerçeve)
    int cpuX = posX + rigWidth - 305;
    int cpuY = posY + rigHeight - 28;
    Rectangle cpuRec{static_cast<float>(cpuX), static_cast<float>(cpuY), 288.0f, 25.0f};
    DrawRectangleRounded(cpuRec, 0.35f, 4, Color{18, 30, 24, 235});
    DrawRectangleRoundedLines(cpuRec, 0.35f, 4, 1.0f, isPowered ? Color{0, 230, 160, 255} : Color{80, 90, 85, 255});

    // Küçük Dönen İşlemci Fanı
    float cpuAngle = isPowered ? std::fmod(static_cast<float>(animTime * 1800.0), 360.0f) : 0.0f;
    DrawSpinningFan(cpuX + 14, cpuY + 12, 9.0f, cpuAngle, isPowered ? Color{0, 255, 180, 255} : Color{70, 80, 75, 255});

    std::ostringstream ssCpu;
    ssCpu << "CPU: " << rig.GetCPUName() << " | " << std::fixed << std::setprecision(1) << (isPowered ? rig.GetCPUHashrateKH() : 0.0) << " KH/s";
    UIFrame::DrawTextCustom(ssCpu.str(), static_cast<float>(cpuX + 28), static_cast<float>(cpuY + 5.0f), 13.0f, isPowered ? Color{180, 255, 215, 255} : Color{140, 150, 145, 255}, true);
}

int RigRenderer::GetClickedGPUIndex(int posX, int posY, size_t gpuCount, Vector2 mousePos, size_t capacity) const {
    int cardW = (capacity <= 2) ? 90 : ((capacity <= 4) ? 90 : ((capacity <= 6) ? 90 : ((capacity <= 8) ? 74 : 60)));
    int startX = posX + 30;
    int slotSpacing = 112;
    if (capacity <= 2) {
        startX = posX + 160;
        slotSpacing = 220;
    } else if (capacity <= 4) {
        startX = posX + 70;
        slotSpacing = 150;
    } else if (capacity <= 6) {
        startX = posX + 30;
        slotSpacing = 112;
    } else if (capacity <= 8) {
        startX = posX + 24;
        slotSpacing = 84;
    } else {
        startX = posX + 20;
        slotSpacing = 68;
    }
    const int gpuY = posY + 55;

    for (size_t i = 0; i < gpuCount; ++i) {
        int cardX = startX + static_cast<int>(i * slotSpacing);
        Rectangle cardRect{static_cast<float>(cardX), static_cast<float>(gpuY), static_cast<float>(cardW), 220.0f};

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
        ssM << "Kazim: " << Core::EconomyManager::FormatHashrate(rig->CalculateTotalHashrate()) << "   |   "
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
        std::string sub = isPowered ? (Core::EconomyManager::FormatHashrate(rigs[i]->CalculateTotalHashrate()) + " | " + std::to_string(static_cast<int>(avgT)) + "C") : "OFF";
        UIFrame::DrawTextCustom(sub, px + 8.0f, bounds.y + 18.0f, 11.0f, isPowered ? Color{100, 220, 150, 255} : Color{180, 80, 80, 255}, false);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isHovered) {
            outSelectedRigIndex = static_cast<int>(i);
        }
    }
}

} // namespace Render
