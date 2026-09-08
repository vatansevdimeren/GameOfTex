#include "GPUInspectionModal.hpp"
#include "UIFrame.hpp"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace Render {

GPUInspectionModal::GPUInspectionModal()
    : m_isOpen(false)
    , m_targetGPU(nullptr)
    , m_currentTemp(25.0)
    , m_slotIndex(0)
    , m_rotationAngle(0.0f)
    , m_isDraggingRotation(false)
    , m_lastMouseX(0.0f)
    , m_btnClockDown(Rectangle{}, "-25 MHz", "", Color{35, 42, 55, 255}, Color{0, 200, 255, 255})
    , m_btnClockUp(Rectangle{}, "+25 MHz", "", Color{35, 42, 55, 255}, Color{0, 200, 255, 255})
    , m_btnPowerDown(Rectangle{}, "-5% Guc", "", Color{35, 42, 55, 255}, Color{255, 180, 0, 255})
    , m_btnPowerUp(Rectangle{}, "+5% Guc", "", Color{35, 42, 55, 255}, Color{255, 180, 0, 255})
    , m_btnFanDown(Rectangle{}, "-10% Fan", "", Color{35, 42, 55, 255}, Color{0, 255, 160, 255})
    , m_btnFanUp(Rectangle{}, "+10% Fan", "", Color{35, 42, 55, 255}, Color{0, 255, 160, 255})
    , m_btnRotateLeft(Rectangle{}, "< SOLA DONDUR", "", Color{25, 30, 42, 255}, Color{0, 220, 255, 255})
    , m_btnRotateRight(Rectangle{}, "SAGA DONDUR >", "", Color{25, 30, 42, 255}, Color{0, 220, 255, 255})
    , m_btnRepair(Rectangle{}, "KARTI TAMIR ET", "Maliyet: $400", Color{140, 30, 30, 255}, Color{255, 50, 50, 255})
    , m_btnScrap(Rectangle{}, "KARTI HURDAYA SAT / CIKAR", "+$75 Hurda Parasi", Color{60, 40, 20, 255}, Color{255, 140, 0, 255})
    , m_btnClose(Rectangle{}, "INCELEMEYI KAPAT", "", Color{35, 45, 60, 255}, Color{0, 220, 255, 255})
{
}

void GPUInspectionModal::Open(Core::GPU* gpu, double tempCelsius, size_t slotIndex) {
    m_targetGPU = gpu;
    m_currentTemp = tempCelsius;
    m_slotIndex = slotIndex;
    m_rotationAngle = 0.0f;
    m_isOpen = true;
}

void GPUInspectionModal::Close() {
    m_isOpen = false;
    m_targetGPU = nullptr;
}

bool GPUInspectionModal::IsOpen() const {
    return m_isOpen;
}

size_t GPUInspectionModal::GetSlotIndex() const {
    return m_slotIndex;
}

void GPUInspectionModal::Update(double economyFiat, double& outRepairCost, bool& outScrapRequested) {
    outRepairCost = 0.0;
    outScrapRequested = false;
    if (!m_isOpen || !m_targetGPU) return;

    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    const float modalW = std::clamp(screenW * 0.85f, 900.0f, 1150.0f);
    const float modalH = std::clamp(screenH * 0.85f, 600.0f, 700.0f);
    const float modalX = (screenW - modalW) / 2.0f;
    const float modalY = (screenH - modalH) / 2.0f;

    // 360 Döndürme Etkileşimi (Fare ile sürükleme)
    Rectangle rotateArea{modalX + 30.0f, modalY + 60.0f, modalW * 0.45f, modalH - 140.0f};
    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, rotateArea)) {
        m_isDraggingRotation = true;
        m_lastMouseX = mouse.x;
    }

    if (m_isDraggingRotation) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            float deltaX = mouse.x - m_lastMouseX;
            m_rotationAngle += deltaX * 1.2f;
            m_rotationAngle = std::fmod(m_rotationAngle + 360.0f, 360.0f);
            m_lastMouseX = mouse.x;
        } else {
            m_isDraggingRotation = false;
        }
    }

    // Döndürme butonları
    const float btnRowY = modalY + modalH - 75.0f;
    m_btnRotateLeft.SetBounds(Rectangle{modalX + 40.0f, btnRowY, (modalW * 0.45f - 40.0f) / 2.0f, 44.0f});
    m_btnRotateRight.SetBounds(Rectangle{modalX + 40.0f + (modalW * 0.45f - 40.0f) / 2.0f + 10.0f, btnRowY, (modalW * 0.45f - 40.0f) / 2.0f, 44.0f});

    if (m_btnRotateLeft.UpdateAndCheckClick()) m_rotationAngle = std::fmod(m_rotationAngle - 30.0f + 360.0f, 360.0f);
    if (m_btnRotateRight.UpdateAndCheckClick()) m_rotationAngle = std::fmod(m_rotationAngle + 30.0f, 360.0f);

    // Sağ Ayar Paneli Butonları
    const float rightPanelX = modalX + modalW * 0.48f;
    const float rightPanelW = modalW * 0.48f;
    const float ctrlW = 120.0f;
    const float ctrlH = 42.0f;

    // Saat frekansı butonları
    m_btnClockDown.SetBounds(Rectangle{rightPanelX + rightPanelW - 260.0f, modalY + 230.0f, ctrlW, ctrlH});
    m_btnClockUp.SetBounds(Rectangle{rightPanelX + rightPanelW - 130.0f, modalY + 230.0f, ctrlW, ctrlH});

    // Güç limiti butonları
    m_btnPowerDown.SetBounds(Rectangle{rightPanelX + rightPanelW - 260.0f, modalY + 295.0f, ctrlW, ctrlH});
    m_btnPowerUp.SetBounds(Rectangle{rightPanelX + rightPanelW - 130.0f, modalY + 295.0f, ctrlW, ctrlH});

    // Fan hızı butonları
    m_btnFanDown.SetBounds(Rectangle{rightPanelX + rightPanelW - 260.0f, modalY + 360.0f, ctrlW, ctrlH});
    m_btnFanUp.SetBounds(Rectangle{rightPanelX + rightPanelW - 130.0f, modalY + 360.0f, ctrlW, ctrlH});

    // Tamir, Hurdaya Sat/Çıkar ve Kapat butonları
    m_btnRepair.SetBounds(Rectangle{rightPanelX, modalY + modalH - 160.0f, rightPanelW - 30.0f, 42.0f});
    m_btnScrap.SetBounds(Rectangle{rightPanelX, modalY + modalH - 110.0f, rightPanelW - 30.0f, 42.0f});
    m_btnClose.SetBounds(Rectangle{rightPanelX, modalY + modalH - 60.0f, rightPanelW - 30.0f, 42.0f});

    // Değer artırıp azaltmalar
    if (m_btnClockDown.UpdateAndCheckClick()) m_targetGPU->SetCoreClockOffset(m_targetGPU->GetCoreClockOffset() - 25.0);
    if (m_btnClockUp.UpdateAndCheckClick()) m_targetGPU->SetCoreClockOffset(m_targetGPU->GetCoreClockOffset() + 25.0);

    if (m_btnPowerDown.UpdateAndCheckClick()) m_targetGPU->SetPowerLimitPercent(m_targetGPU->GetPowerLimitPercent() - 5.0);
    if (m_btnPowerUp.UpdateAndCheckClick()) m_targetGPU->SetPowerLimitPercent(m_targetGPU->GetPowerLimitPercent() + 5.0);

    if (m_btnFanDown.UpdateAndCheckClick()) m_targetGPU->SetFanSpeedPercent(m_targetGPU->GetFanSpeedPercent() - 10.0);
    if (m_btnFanUp.UpdateAndCheckClick()) m_targetGPU->SetFanSpeedPercent(m_targetGPU->GetFanSpeedPercent() + 10.0);

    // Tamir butonu
    bool needsRepair = (m_targetGPU->IsBurnt() || m_targetGPU->GetHealthPercent() < 100.0);
    m_btnRepair.SetDisabled(!needsRepair || economyFiat < 400.0);
    if (needsRepair && m_btnRepair.UpdateAndCheckClick()) {
        if (economyFiat >= 400.0) {
            m_targetGPU->Repair();
            outRepairCost = 400.0;
        }
    }

    // Hurdaya Sat / Çöpe At butonu
    if (m_btnScrap.UpdateAndCheckClick()) {
        outScrapRequested = true;
        Close();
        return;
    }

    if (m_btnClose.UpdateAndCheckClick() || IsKeyPressed(KEY_ESCAPE)) {
        Close();
    }
}

void GPUInspectionModal::Draw3DCardPreview(float centerX, float centerY, float height, double animTime,
                                         const TextureManager* textureManager) const {
    const float rad = m_rotationAngle * (PI / 180.0f);
    const float cosAngle = std::cos(rad);
    const float sinAngle = std::sin(rad);

    const float baseWidth = 160.0f;
    const float projectedWidth = std::max(12.0f, baseWidth * std::abs(cosAngle));
    const float halfW = projectedWidth / 2.0f;
    const float halfH = height / 2.0f;

    const Rectangle cardRect{centerX - halfW, centerY - halfH, projectedWidth, height};

    // Kalınlık yan yüzeyi (3D efekt için)
    float thickness = sinAngle * 24.0f;
    if (std::abs(thickness) > 2.0f) {
        float edgeX = (thickness > 0) ? cardRect.x + cardRect.width : cardRect.x;
        DrawRectangle(static_cast<int>(edgeX), static_cast<int>(cardRect.y + 10),
                      static_cast<int>(std::abs(thickness)), static_cast<int>(cardRect.height - 20),
                      Color{40, 45, 55, 255});
        // Soğutucu ızgara çizgileri
        for (int y = static_cast<int>(cardRect.y + 20); y < static_cast<int>(cardRect.y + cardRect.height - 20); y += 8) {
            DrawLine(static_cast<int>(edgeX), y, static_cast<int>(edgeX + std::abs(thickness)), y, Color{70, 80, 95, 255});
        }
    }

    // YANMIŞ MI?
    bool burnt = m_targetGPU && m_targetGPU->IsBurnt();

    // ÖN YÜZ (cosAngle >= 0)
    if (cosAngle >= 0.0f) {
        if (burnt) {
            // Kömürleşmiş gövde
            DrawRectangleRounded(cardRect, 0.08f, 4, Color{15, 12, 12, 255});
            DrawRectangleRoundedLines(cardRect, 0.08f, 4, 2.0f, Color{80, 20, 20, 255});
        } else if (textureManager && textureManager->HasGPUTexture()) {
            textureManager->DrawGPUTexture(cardRect, WHITE);
        } else {
            // Şık karbon/metalik ön yüz
            DrawRectangleRounded(cardRect, 0.08f, 4, Color{28, 32, 42, 255});
            DrawRectangleRoundedLines(cardRect, 0.08f, 4, 2.0f, Color{60, 72, 95, 255});

            // Alüminyum heatsink ızgaraları
            for (int y = static_cast<int>(cardRect.y + 25); y < static_cast<int>(cardRect.y + cardRect.height - 25); y += 12) {
                DrawLine(static_cast<int>(cardRect.x + 10), y, static_cast<int>(cardRect.x + cardRect.width - 10), y, Color{45, 52, 68, 255});
            }
        }

        // Dönen RGB Fanlar (Ön yüzdeyken)
        if (!burnt) {
            float fanRadius = std::min(halfW * 0.75f, 45.0f);
            if (fanRadius > 10.0f) {
                float fanSpeed = static_cast<float>(m_targetGPU ? m_targetGPU->GetFanSpeedPercent() * 8.0 : 400.0);
                float angle = std::fmod(static_cast<float>(animTime * fanSpeed), 360.0f);

                Color rgbColor = (m_currentTemp > 85.0) ? Color{255, 80, 0, 255} : Color{0, 220, 255, 255};

                // Üst Fan
                DrawCircle(static_cast<int>(centerX), static_cast<int>(centerY - halfH * 0.45f), fanRadius, Color{20, 22, 28, 255});
                DrawCircleLines(static_cast<int>(centerX), static_cast<int>(centerY - halfH * 0.45f), fanRadius, rgbColor);

                // Alt Fan
                DrawCircle(static_cast<int>(centerX), static_cast<int>(centerY + halfH * 0.45f), fanRadius, Color{20, 22, 28, 255});
                DrawCircleLines(static_cast<int>(centerX), static_cast<int>(centerY + halfH * 0.45f), fanRadius, rgbColor);
            }
        }
    }
    // ARKA YÜZ (cosAngle < 0 - Metalik Backplate & Isı Boruları)
    else {
        // Fırçalanmış alüminyum arka plaka
        DrawRectangleRounded(cardRect, 0.08f, 4, burnt ? Color{15, 12, 12, 255} : Color{35, 40, 50, 255});
        DrawRectangleRoundedLines(cardRect, 0.08f, 4, 2.0f, burnt ? Color{80, 20, 20, 255} : Color{70, 85, 110, 255});

        // Bakır Isı Boruları (Copper Heatpipes)
        float pipeX1 = cardRect.x + cardRect.width * 0.35f;
        float pipeX2 = cardRect.x + cardRect.width * 0.65f;
        DrawRectangle(static_cast<int>(pipeX1), static_cast<int>(cardRect.y + 30), 8, static_cast<int>(cardRect.height - 60), Color{185, 100, 45, 255});
        DrawRectangle(static_cast<int>(pipeX2), static_cast<int>(cardRect.y + 30), 8, static_cast<int>(cardRect.height - 60), Color{185, 100, 45, 255});

        // GPU Çekirdek Braketi (X-Bracket)
        DrawRectangleLines(static_cast<int>(centerX - 20), static_cast<int>(centerY - 20), 40, 40, Color{120, 130, 150, 255});

        // Lazer Kazıma Model Yazısı
        if (projectedWidth > 80.0f) {
            UIFrame::DrawTextCustom("RTX BACKPLATE", cardRect.x + 15.0f, centerY + halfH * 0.65f, 13.0f, Color{120, 135, 160, 255}, true);
        }
    }

    // Yanmışsa duman ve alev damgası
    if (burnt) {
        DrawRectangle(static_cast<int>(cardRect.x), static_cast<int>(centerY - 20), static_cast<int>(cardRect.width), 40, Color{180, 0, 0, 220});
        if (projectedWidth > 60.0f) {
            UIFrame::DrawTextCustom("YANDI!", cardRect.x + 10.0f, centerY - 12.0f, 18.0f, WHITE, true);
        }
    }
}

void GPUInspectionModal::Draw(double animTime, const TextureManager* textureManager) const {
    if (!m_isOpen || !m_targetGPU) return;

    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    // Karartma katmanı
    DrawRectangle(0, 0, static_cast<int>(screenW), static_cast<int>(screenH), Color{0, 0, 0, 210});

    const float modalW = std::clamp(screenW * 0.85f, 900.0f, 1150.0f);
    const float modalH = std::clamp(screenH * 0.85f, 600.0f, 700.0f);
    const float modalX = (screenW - modalW) / 2.0f;
    const float modalY = (screenH - modalH) / 2.0f;

    std::string cardTitle = m_targetGPU->GetName() + " - DETAYLI INCELEME VE OVERCLOCK";
    UIFrame::DrawCard(Rectangle{modalX, modalY, modalW, modalH}, cardTitle, Color{0, 220, 255, 255});

    // 1. SOL ALAN: 360 DÖNDÜRÜLEBİLİR KART ALANI
    const float leftAreaW = modalW * 0.45f;
    const float cardCenterX = modalX + leftAreaW / 2.0f + 20.0f;
    const float cardCenterY = modalY + modalH / 2.0f - 25.0f;

    // Arka plan ışık halkası
    DrawCircleGradient(static_cast<int>(cardCenterX), static_cast<int>(cardCenterY), 160.0f, Color{0, 180, 255, 30}, Color{0, 0, 0, 0});

    Draw3DCardPreview(cardCenterX, cardCenterY, 360.0f, animTime, textureManager);

    UIFrame::DrawTextCustom("Fare ile tutup karti 360 derece cevirebilirsiniz", modalX + 45.0f, modalY + 65.0f, 13.0f, Color{140, 160, 190, 255}, false);

    m_btnRotateLeft.Draw();
    m_btnRotateRight.Draw();

    // 2. SAĞ ALAN: DURUM, TEHLİKE VE HIZ AŞIRTMA AYARLARI
    const float rightPanelX = modalX + modalW * 0.48f;
    const float rightPanelW = modalW * 0.48f;

    // Isı ve Yangın Durum Rozeti
    Color tempBadgeColor = Color{0, 255, 150, 255};
    std::string statusText = "GÜVENLİ";
    if (m_targetGPU->IsBurnt()) {
        tempBadgeColor = Color{255, 30, 30, 255};
        statusText = "KART AŞIRI SICAKLIKTAN YANDI!";
    } else if (m_currentTemp >= 135.0) {
        tempBadgeColor = Color{255, 30, 30, 255};
        statusText = "KRİTİK YANGIN TEHLİKESİ! 140°C'DE YANACAK!";
    } else if (m_currentTemp >= 105.0) {
        tempBadgeColor = Color{255, 120, 0, 255};
        statusText = "TEHLİKELİ SICAKLIK (SAĞLIK ERİYOR)";
    } else if (m_currentTemp >= 85.0) {
        tempBadgeColor = Color{255, 200, 0, 255};
        statusText = "TERMAL YAVAŞLATMA (THROTTLED)";
    }

    Rectangle dangerBox{rightPanelX, modalY + 65.0f, rightPanelW - 30.0f, 50.0f};
    DrawRectangleRounded(dangerBox, 0.2f, 4, Color{25, 20, 25, 240});
    DrawRectangleRoundedLines(dangerBox, 0.2f, 4, 1.8f, tempBadgeColor);
    UIFrame::DrawTextCustom("SICAKLIK DURUMU: " + statusText, rightPanelX + 16.0f, modalY + 80.0f, 15.0f, tempBadgeColor, true);

    // Sağlık Barı (Kart Sağlığı %100 - %0)
    std::string healthStr = "DONANIM SAĞLIĞI: %" + std::to_string(static_cast<int>(m_targetGPU->GetHealthPercent()));
    UIFrame::DrawProgressBar(Rectangle{rightPanelX, modalY + 125.0f, rightPanelW - 30.0f, 24.0f},
                            static_cast<float>(m_targetGPU->GetHealthPercent() / 100.0),
                            m_targetGPU->GetHealthPercent() > 50.0 ? Color{0, 230, 130, 255} : Color{255, 60, 60, 255},
                            healthStr);

    // Silikon Kalitesi ve Anlık Çekirdek Isısı
    std::ostringstream ssSilicon, ssLiveTemp, ssHash, ssWatts;
    ssSilicon << "Silikon Kalitesi: %" << static_cast<int>(m_targetGPU->GetSiliconQuality() * 100.0)
              << (m_targetGPU->GetSiliconQuality() >= 1.05 ? " (Altın Silikon!)" : " (Standart)");
    ssLiveTemp << "Anlik Cekirdek Isisi: " << static_cast<int>(m_currentTemp) << " C";
    ssHash << "Etkin Kazim Gucu: " << std::fixed << std::setprecision(1) << m_targetGPU->GetEffectiveHashrate() << " MH/s";
    ssWatts << "Guc Tuketimi: " << std::fixed << std::setprecision(0) << m_targetGPU->GetEffectivePowerWatts() << " W";

    UIFrame::DrawTextCustom(ssSilicon.str(), rightPanelX, modalY + 160.0f, 15.0f, GOLD, true);
    UIFrame::DrawTextCustom(ssLiveTemp.str(), rightPanelX, modalY + 185.0f, 15.0f, tempBadgeColor, true);
    UIFrame::DrawTextCustom(ssHash.str(), rightPanelX + 260.0f, modalY + 160.0f, 15.0f, Color{0, 220, 255, 255}, true);
    UIFrame::DrawTextCustom(ssWatts.str(), rightPanelX + 260.0f, modalY + 185.0f, 15.0f, ORANGE, true);

    // İnce Ayar Kontrolleri (Overclock & Undervolt)
    std::string clockStr = std::string("Cekirdek Saat Farki: ") + (m_targetGPU->GetCoreClockOffset() >= 0 ? "+" : "") + std::to_string(static_cast<int>(m_targetGPU->GetCoreClockOffset())) + " MHz";
    UIFrame::DrawTextCustom(clockStr, rightPanelX, modalY + 240.0f, 16.0f, WHITE, true);
    m_btnClockDown.Draw();
    m_btnClockUp.Draw();

    std::string powerStr = "Guc Limiti: %" + std::to_string(static_cast<int>(m_targetGPU->GetPowerLimitPercent()));
    UIFrame::DrawTextCustom(powerStr, rightPanelX, modalY + 305.0f, 16.0f, WHITE, true);
    m_btnPowerDown.Draw();
    m_btnPowerUp.Draw();

    std::string fanStr = "Manuel Fan Devri: %" + std::to_string(static_cast<int>(m_targetGPU->GetFanSpeedPercent()));
    UIFrame::DrawTextCustom(fanStr, rightPanelX, modalY + 370.0f, 16.0f, WHITE, true);
    m_btnFanDown.Draw();
    m_btnFanUp.Draw();

    // Tamir, Hurda ve Kapat Butonları
    m_btnRepair.Draw();
    m_btnScrap.Draw();
    m_btnClose.Draw();
}

} // namespace Render
