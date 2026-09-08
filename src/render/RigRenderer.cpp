#include "RigRenderer.hpp"
#include "UIFrame.hpp"
#include <cmath>
#include <string>

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

void RigRenderer::DrawSingleGPU(const Core::GPU* gpu, double tempCelsius, int x, int y, double animTime,
                                const TextureManager* textureManager) const {
    constexpr int gpuWidth = 90;
    constexpr int gpuHeight = 220;

    // Fan spinning speed scales with temperature
    float speedMultiplier = static_cast<float>(tempCelsius * 8.0);
    float currentAngle = std::fmod(static_cast<float>(animTime * speedMultiplier), 360.0f);

    // RGB Fan Color based on temperature
    Color fanRgb;
    if (tempCelsius < 55.0) {
        fanRgb = Color{0, 210, 255, 255}; // Cool Cyan
    } else if (tempCelsius < 75.0) {
        fanRgb = Color{0, 255, 120, 255}; // Warm Green
    } else if (tempCelsius < 85.0) {
        fanRgb = Color{255, 160, 0, 255}; // Hot Orange
    } else {
        fanRgb = Color{255, 30, 30, 255}; // Dangerous Red
    }

    // 1. Ekran Kartı Gövdesi (Varsa Ultra-Res doku, yoksa kaliteli prosedürel çizim)
    if (textureManager && textureManager->HasGPUTexture()) {
        textureManager->DrawGPUTexture(Rectangle{static_cast<float>(x), static_cast<float>(y),
                                                 static_cast<float>(gpuWidth), static_cast<float>(gpuHeight)});
    } else {
        DrawRectangle(x, y, gpuWidth, gpuHeight, Color{30, 32, 38, 255});
        DrawRectangleLines(x, y, gpuWidth, gpuHeight, Color{70, 75, 85, 255});

        for (int finY = y + 15; finY < y + gpuHeight - 15; finY += 6) {
            DrawLine(x + 8, finY, x + gpuWidth - 8, finY, Color{50, 55, 65, 255});
        }
    }

    // 2. Dönen Fanlar (Varsa harici pervane görseli)
    if (textureManager && textureManager->HasFanTexture()) {
        textureManager->DrawFanTexture(x + gpuWidth / 2.0f, y + 60.0f, 32.0f, currentAngle, fanRgb);
        textureManager->DrawFanTexture(x + gpuWidth / 2.0f, y + 145.0f, 32.0f, -currentAngle, fanRgb);
    } else {
        DrawSpinningFan(x + gpuWidth / 2, y + 60, 32.0f, currentAngle, fanRgb);
        DrawSpinningFan(x + gpuWidth / 2, y + 145, 32.0f, -currentAngle, fanRgb);
    }

    // Status LED
    Color ledColor = (tempCelsius >= 85.0) ? RED : ((tempCelsius >= 70.0) ? ORANGE : GREEN);
    DrawCircle(x + gpuWidth - 12, y + 12, 4.0f, ledColor);

    // Readout metinleri (Net ve okunaklı)
    if (gpu) {
        std::string tempText = std::to_string(static_cast<int>(tempCelsius)) + "C";
        UIFrame::DrawTextCustom(tempText, static_cast<float>(x + 10), static_cast<float>(y + gpuHeight - 20), 14.0f, WHITE, true);

        std::string hrText = std::to_string(static_cast<int>(gpu->GetEffectiveHashrate())) + "M";
        UIFrame::DrawTextCustom(hrText, static_cast<float>(x + gpuWidth - 38), static_cast<float>(y + gpuHeight - 20), 14.0f, fanRgb, true);
    }
}

void RigRenderer::DrawRig(const Core::MiningRig& rig, const Core::ThermalModel& thermalModel,
                         int posX, int posY, double animTime, const TextureManager* textureManager) const {
    constexpr int rigWidth = 720;
    constexpr int rigHeight = 320;

    // Aluminum frame (metallic open-air rig)
    DrawRectangleRounded(Rectangle{static_cast<float>(posX), static_cast<float>(posY),
                                   static_cast<float>(rigWidth), static_cast<float>(rigHeight)},
                         0.04f, 6, Color{20, 22, 26, 240});
    DrawRectangleRoundedLines(Rectangle{static_cast<float>(posX), static_cast<float>(posY),
                                        static_cast<float>(rigWidth), static_cast<float>(rigHeight)},
                              0.04f, 6, 2.0f, Color{100, 110, 130, 255});

    // Top and bottom aluminum support bars
    DrawRectangle(posX + 10, posY + 15, rigWidth - 20, 8, Color{80, 85, 95, 255});
    DrawRectangle(posX + 10, posY + rigHeight - 25, rigWidth - 20, 8, Color{80, 85, 95, 255});

    // Rig header label
    UIFrame::DrawTextCustom(rig.GetName(), static_cast<float>(posX + 24), static_cast<float>(posY + 26), 18.0f, RAYWHITE, true);

    // Draw installed GPUs
    const auto& gpus = rig.GetGPUs();
    constexpr int slotSpacing = 112;
    const int startX = posX + 30;
    const int gpuY = posY + 55;

    for (size_t i = 0; i < rig.GetMaxCapacity(); ++i) {
        int cardX = startX + static_cast<int>(i * slotSpacing);

        if (i < gpus.size() && gpus[i]) {
            double temp = thermalModel.CalculateGPUTemperature(gpus[i]->GetEffectivePowerWatts(), 0.85);
            DrawSingleGPU(gpus[i].get(), temp, cardX, gpuY, animTime, textureManager);
        } else {
            // Empty PCIe slot placeholder
            DrawRectangleLines(cardX, gpuY, 90, 220, Color{45, 50, 60, 180});
            UIFrame::DrawTextCustom("EMPTY", static_cast<float>(cardX + 22), static_cast<float>(gpuY + 100), 14.0f, Color{80, 90, 105, 255}, false);
        }
    }
}

} // namespace Render
