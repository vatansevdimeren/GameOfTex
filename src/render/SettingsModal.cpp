#include "SettingsModal.hpp"
#include "UIFrame.hpp"
#include "../core/Localization.hpp"
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
    , m_btnToggleFullscreen(Rectangle{}, "TAM EKRAN (F11)", "Pencere / Fullscreen", Color{35, 45, 60, 255}, Color{0, 220, 255, 255})
    , m_btnToggleLanguage(Rectangle{}, "DIL: TURKCE 🇹🇷", "Degistir / Switch", Color{25, 45, 50, 255}, Color{0, 240, 180, 255})
    , m_btnToggleCurrency(Rectangle{}, "PARA BIRIMI: USD ($)", "USDT / TRY / EUR", Color{45, 38, 20, 255}, Color{255, 200, 0, 255})
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

void SettingsModal::Update(Core::EconomyManager& economy) {
    if (!m_isOpen) return;

    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    const float modalW = std::clamp(screenW * 0.54f, 580.0f, 750.0f);
    const float modalH = std::clamp(screenH * 0.78f, 540.0f, 680.0f);
    const float modalX = (screenW - modalW) / 2.0f;
    const float modalY = (screenH - modalH) / 2.0f;

    const float innerX = modalX + 40.0f;
    const float innerW = modalW - 80.0f;
    const float btnH = 46.0f;

    // Ölçek butonlarının yerleşimi (2 sütun)
    const float colW = (innerW - 14.0f) / 2.0f;

    m_btnScale100.SetBounds(Rectangle{innerX, modalY + 120.0f, colW, btnH});
    m_btnScale125.SetBounds(Rectangle{innerX + colW + 14.0f, modalY + 120.0f, colW, btnH});

    m_btnScale150.SetBounds(Rectangle{innerX, modalY + 172.0f, colW, btnH});
    m_btnScale175.SetBounds(Rectangle{innerX + colW + 14.0f, modalY + 172.0f, colW, btnH});

    m_btnScale200.SetBounds(Rectangle{innerX, modalY + 224.0f, innerW, btnH});

    // Tam ekran ve Dil butonları (yan yana 2 sütun)
    m_btnToggleFullscreen.SetBounds(Rectangle{innerX, modalY + 300.0f, colW, 52.0f});
    m_btnToggleLanguage.SetBounds(Rectangle{innerX + colW + 14.0f, modalY + 300.0f, colW, 52.0f});

    // Para birimi butonu
    m_btnToggleCurrency.SetBounds(Rectangle{innerX, modalY + 360.0f, innerW, 54.0f});

    m_btnClose.SetBounds(Rectangle{innerX, modalY + modalH - 70.0f, innerW, 50.0f});

    // Buton etiketlerini güncelle
    m_btnToggleLanguage.SetTitle(std::string("DIL / LANG: ") + Core::LocalizationManager::Get().GetLanguageName());
    std::string curTitle = std::string(Core::LocalizationManager::Tr("SETTINGS_CURRENCY")) + economy.GetCurrencyCode() + " (" + economy.GetCurrencySymbol() + ")";
    m_btnToggleCurrency.SetTitle(curTitle);
    m_btnClose.SetTitle(Core::LocalizationManager::Tr("SETTINGS_CLOSE"));

    // Tıklamaları işle
    if (m_btnScale100.UpdateAndCheckClick()) UIFrame::SetUIScale(1.0f);
    if (m_btnScale125.UpdateAndCheckClick()) UIFrame::SetUIScale(1.25f);
    if (m_btnScale150.UpdateAndCheckClick()) UIFrame::SetUIScale(1.50f);
    if (m_btnScale175.UpdateAndCheckClick()) UIFrame::SetUIScale(1.75f);
    if (m_btnScale200.UpdateAndCheckClick()) UIFrame::SetUIScale(2.0f);

    if (m_btnToggleFullscreen.UpdateAndCheckClick()) {
        ToggleFullscreen();
    }

    if (m_btnToggleLanguage.UpdateAndCheckClick()) {
        Core::LocalizationManager::Get().ToggleLanguage();
    }

    if (m_btnToggleCurrency.UpdateAndCheckClick()) {
        economy.NextCurrency();
    }

    if (m_btnClose.UpdateAndCheckClick() || IsKeyPressed(KEY_ESCAPE)) {
        Close();
    }
}

void SettingsModal::Draw(const Core::EconomyManager& economy) const {
    if (!m_isOpen) return;

    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    // Karartma katmanı
    DrawRectangle(0, 0, static_cast<int>(screenW), static_cast<int>(screenH), Color{0, 0, 0, 190});

    const float modalW = std::clamp(screenW * 0.54f, 580.0f, 750.0f);
    const float modalH = std::clamp(screenH * 0.78f, 540.0f, 680.0f);
    const float modalX = (screenW - modalW) / 2.0f;
    const float modalY = (screenH - modalH) / 2.0f;

    const float innerX = modalX + 40.0f;

    UIFrame::DrawCard(Rectangle{modalX, modalY, modalW, modalH}, Core::LocalizationManager::Tr("SETTINGS_TITLE"), Color{0, 220, 255, 255});

    // Başlık ve Açıklamalar
    UIFrame::DrawTextCustom(Core::LocalizationManager::Tr("SETTINGS_SCALE_DESC"), innerX, modalY + 58.0f, 17.0f, RAYWHITE, true);

    std::string currentScaleStr = "Mevcut Olcek: " + std::to_string(static_cast<int>(UIFrame::GetUIScale() * 100)) + "%";
    UIFrame::DrawTextCustom(currentScaleStr, innerX, modalY + 85.0f, 15.0f, Color{0, 255, 180, 255}, true);

    // Butonlar
    m_btnScale100.Draw();
    m_btnScale125.Draw();
    m_btnScale150.Draw();
    m_btnScale175.Draw();
    m_btnScale200.Draw();

    m_btnToggleFullscreen.Draw();
    m_btnToggleLanguage.Draw();
    m_btnToggleCurrency.Draw();

    m_btnClose.Draw();
}

} // namespace Render
