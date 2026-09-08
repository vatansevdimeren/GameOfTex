#include "SettingsModal.hpp"
#include "UIFrame.hpp"
#include <algorithm>

namespace Render {

SettingsModal::SettingsModal()
    : m_isOpen(false)
    , m_btnClose(Rectangle{}, "KAPAT VE DEVAM ET", "", Color{140, 30, 30, 255}, Color{255, 70, 70, 255})
    , m_btnScale100(Rectangle{}, "1.0x STANDART", "Normal Boyut", Color{30, 35, 45, 255}, Color{0, 200, 255, 255})
    , m_btnScale125(Rectangle{}, "1.25x BUYUK", "Daha Okunakli", Color{30, 35, 45, 255}, Color{0, 220, 255, 255})
    , m_btnScale150(Rectangle{}, "1.50x COK BUYUK", "Buyuk Ekranlar Icin", Color{30, 35, 45, 255}, Color{0, 240, 255, 255})
    , m_btnScale175(Rectangle{}, "1.75x DEV", "Ultra Genis Font", Color{30, 35, 45, 255}, Color{100, 255, 200, 255})
    , m_btnScale200(Rectangle{}, "2.0x ULTRA", "Maksimum Boyut", Color{30, 35, 45, 255}, Color{255, 200, 0, 255})
    , m_btnToggleFullscreen(Rectangle{}, "TAM EKRAN AC / KAPA", "[F11] Kisa Yolu", Color{35, 45, 60, 255}, Color{0, 220, 255, 255})
{
}

void SettingsModal::Open() {
    m_isOpen = true;
}

void SettingsModal::Close() {
    m_isOpen = false;
}

bool SettingsModal::IsOpen() const {
    return m_isOpen;
}

void SettingsModal::Update() {
    if (!m_isOpen) return;

    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    const float modalW = std::clamp(screenW * 0.50f, 540.0f, 720.0f);
    const float modalH = std::clamp(screenH * 0.72f, 500.0f, 640.0f);
    const float modalX = (screenW - modalW) / 2.0f;
    const float modalY = (screenH - modalH) / 2.0f;

    const float innerX = modalX + 40.0f;
    const float innerW = modalW - 80.0f;
    const float btnH = 50.0f;

    // Ölçek butonlarının yerleşimi (2 sütun veya alt alta)
    const float colW = (innerW - 14.0f) / 2.0f;

    m_btnScale100.SetBounds(Rectangle{innerX, modalY + 140.0f, colW, btnH});
    m_btnScale125.SetBounds(Rectangle{innerX + colW + 14.0f, modalY + 140.0f, colW, btnH});

    m_btnScale150.SetBounds(Rectangle{innerX, modalY + 200.0f, colW, btnH});
    m_btnScale175.SetBounds(Rectangle{innerX + colW + 14.0f, modalY + 200.0f, colW, btnH});

    m_btnScale200.SetBounds(Rectangle{innerX, modalY + 260.0f, innerW, btnH});

    m_btnToggleFullscreen.SetBounds(Rectangle{innerX, modalY + 340.0f, innerW, btnH});
    m_btnClose.SetBounds(Rectangle{innerX, modalY + modalH - 75.0f, innerW, 54.0f});

    // Tıklamaları işle
    if (m_btnScale100.UpdateAndCheckClick()) UIFrame::SetUIScale(1.0f);
    if (m_btnScale125.UpdateAndCheckClick()) UIFrame::SetUIScale(1.25f);
    if (m_btnScale150.UpdateAndCheckClick()) UIFrame::SetUIScale(1.50f);
    if (m_btnScale175.UpdateAndCheckClick()) UIFrame::SetUIScale(1.75f);
    if (m_btnScale200.UpdateAndCheckClick()) UIFrame::SetUIScale(2.0f);

    if (m_btnToggleFullscreen.UpdateAndCheckClick()) {
        ToggleFullscreen();
    }

    if (m_btnClose.UpdateAndCheckClick() || IsKeyPressed(KEY_ESCAPE)) {
        Close();
    }
}

void SettingsModal::Draw() const {
    if (!m_isOpen) return;

    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    // Karartma katmanı
    DrawRectangle(0, 0, static_cast<int>(screenW), static_cast<int>(screenH), Color{0, 0, 0, 190});

    const float modalW = std::clamp(screenW * 0.50f, 540.0f, 720.0f);
    const float modalH = std::clamp(screenH * 0.72f, 500.0f, 640.0f);
    const float modalX = (screenW - modalW) / 2.0f;
    const float modalY = (screenH - modalH) / 2.0f;

    const float innerX = modalX + 40.0f;

    UIFrame::DrawCard(Rectangle{modalX, modalY, modalW, modalH}, "SISTEM VE GORSEL AYARLARI", Color{0, 220, 255, 255});

    // Başlık ve Açıklamalar
    UIFrame::DrawTextCustom("YAZI VE ARAYUZ BOYUTLANDIRMA (UI SCALE)", innerX, modalY + 65.0f, 18.0f, RAYWHITE, true);

    std::string currentScaleStr = "Mevcut Olcek: " + std::to_string(static_cast<int>(UIFrame::GetUIScale() * 100)) + "%";
    UIFrame::DrawTextCustom(currentScaleStr, innerX, modalY + 95.0f, 16.0f, Color{0, 255, 180, 255}, true);

    // Butonlar
    m_btnScale100.Draw();
    m_btnScale125.Draw();
    m_btnScale150.Draw();
    m_btnScale175.Draw();
    m_btnScale200.Draw();

    UIFrame::DrawTextCustom("GORUNTU MODU:", innerX, modalY + 318.0f, 15.0f, Color{180, 195, 220, 255}, true);
    m_btnToggleFullscreen.Draw();

    m_btnClose.Draw();
}

} // namespace Render
