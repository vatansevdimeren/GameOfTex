#include "MainMenuScreen.hpp"
#include "UIFrame.hpp"
#include "../core/Localization.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace Render {

MainMenuScreen::MainMenuScreen()
    : m_btnContinue(Rectangle{0, 0, 10, 10}, "DEVAM ET", "Kayittan Yukle", Color{20, 50, 40, 255}, Color{0, 255, 140, 255})
    , m_btnNewGame(Rectangle{0, 0, 10, 10}, "YENI OYUNA BASLA", "+$1,000 Baslangic Bonusu", Color{25, 45, 65, 255}, Color{0, 220, 255, 255})
    , m_btnSettings(Rectangle{0, 0, 10, 10}, "AYARLAR", "Dil, Para Birimi ve Ekran", Color{35, 40, 55, 255}, Color{170, 190, 230, 255})
    , m_btnQuit(Rectangle{0, 0, 10, 10}, "OYUNDAN CIKIS", "Masaustune Don", Color{50, 25, 30, 255}, Color{255, 75, 75, 255})
    , m_btnConfirmReset(Rectangle{0, 0, 10, 10}, "EVET, SIFIRLA VE BASLA", "", Color{70, 25, 25, 255}, Color{255, 60, 60, 255})
    , m_btnCancelReset(Rectangle{0, 0, 10, 10}, "VAZGEC / GERI", "", Color{30, 40, 55, 255}, Color{100, 200, 255, 255})
{
    InitParticles();
    RefreshSaveState();
}

void MainMenuScreen::RefreshSaveState() {
    m_hasSave = Core::SaveManager::HasSaveFile();
    if (m_hasSave) {
        m_hasSave = Core::SaveManager::ReadSaveMetadata(m_saveMeta);
    }
}

void MainMenuScreen::InitParticles() {
    m_particles.clear();
    for (int i = 0; i < 45; ++i) {
        MenuParticle p;
        p.x = static_cast<float>(std::rand() % 1920);
        p.y = static_cast<float>(std::rand() % 1080);
        p.speedY = -15.0f - static_cast<float>(std::rand() % 35);
        p.speedX = -10.0f + static_cast<float>(std::rand() % 20);
        p.size = 1.5f + static_cast<float>(std::rand() % 4);
        p.alpha = 0.2f + static_cast<float>(std::rand() % 60) / 100.0f;
        int colType = std::rand() % 3;
        if (colType == 0) p.color = Color{0, 220, 255, 255};
        else if (colType == 1) p.color = Color{0, 255, 140, 255};
        else p.color = Color{180, 100, 255, 255};
        m_particles.push_back(p);
    }
}

void MainMenuScreen::UpdateParticles(float dt) {
    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    for (auto& p : m_particles) {
        p.y += p.speedY * dt;
        p.x += p.speedX * dt;
        if (p.y < -10.0f) {
            p.y = screenH + 10.0f;
            p.x = static_cast<float>(std::rand() % static_cast<int>(std::max(1.0f, screenW)));
        }
        if (p.x < -10.0f) p.x = screenW + 10.0f;
        if (p.x > screenW + 10.0f) p.x = -10.0f;
    }
}

MainMenuAction MainMenuScreen::Update(float dt) {
    m_animTime += dt;
    UpdateParticles(dt);

    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    // Onay penceresi açıksa sadece onay butonlarını güncelle
    if (m_showConfirmNewGame) {
        float modalW = 540.0f;
        float modalH = 240.0f;
        float modalX = (screenW - modalW) * 0.5f;
        float modalY = (screenH - modalH) * 0.5f;

        m_btnConfirmReset.SetBounds(Rectangle{modalX + 30.0f, modalY + modalH - 65.0f, 230.0f, 48.0f});
        m_btnCancelReset.SetBounds(Rectangle{modalX + modalW - 260.0f, modalY + modalH - 65.0f, 230.0f, 48.0f});

        if (m_btnConfirmReset.UpdateAndCheckClick()) {
            m_showConfirmNewGame = false;
            return MainMenuAction::START_NEW_GAME;
        }
        if (m_btnCancelReset.UpdateAndCheckClick() || IsKeyPressed(KEY_ESCAPE)) {
            m_showConfirmNewGame = false;
            return MainMenuAction::NONE;
        }
        return MainMenuAction::NONE;
    }

    // Menü Kartı Boyutlandırması
    const float menuW = std::clamp(screenW * 0.38f, 440.0f, 520.0f);
    const float btnH = 58.0f;
    const float btnGap = 12.0f;
    const float menuX = (screenW - menuW) * 0.5f;
    const float startY = screenH * 0.44f;

    m_btnContinue.SetBounds(Rectangle{menuX, startY + (0 * (btnH + btnGap)), menuW, btnH});
    m_btnNewGame.SetBounds(Rectangle{menuX, startY + (1 * (btnH + btnGap)), menuW, btnH});
    m_btnSettings.SetBounds(Rectangle{menuX, startY + (2 * (btnH + btnGap)), menuW, btnH});
    m_btnQuit.SetBounds(Rectangle{menuX, startY + (3 * (btnH + btnGap)), menuW, btnH});

    // Buton etiketlerini güncelle
    bool isTR = (Core::LocalizationManager::Get().GetLanguage() == Core::Language::TURKISH);

    if (m_hasSave) {
        m_btnContinue.SetDisabled(false);
        m_btnContinue.SetTitle(isTR ? "DEVAM ET (KAYITTAN YUKLE)" : "CONTINUE (LOAD GAME)");
        std::ostringstream ss;
        ss << m_saveMeta.companyName << " | $" << static_cast<long long>(m_saveMeta.fiatBalance)
           << " | " << m_saveMeta.activeFacilityName;
        m_btnContinue.SetSubtitle(ss.str());
        m_btnContinue.SetAccentColor(Color{0, 255, 150, 255});
    } else {
        m_btnContinue.SetDisabled(true);
        m_btnContinue.SetTitle(isTR ? "DEVAM ET" : "CONTINUE");
        m_btnContinue.SetSubtitle(isTR ? "Kayitli oyun bulunamadi" : "No saved game found");
        m_btnContinue.SetAccentColor(Color{80, 100, 120, 255});
    }

    m_btnNewGame.SetTitle(isTR ? "YENI OYUNA BASLA" : "START NEW GAME");
    m_btnNewGame.SetSubtitle(isTR ? "+$1,000 Baslangic Bonusu ve Teksas Tesisi" : "+$1,000 Bonus & Texas Facility");

    m_btnSettings.SetTitle(Core::LocalizationManager::Tr("TITLE_SETTINGS"));
    m_btnSettings.SetSubtitle(isTR ? "Dil, Para Birimi, Olcek ve Gorunum" : "Language, Currency, Scale & Screen");

    m_btnQuit.SetTitle(isTR ? "OYUNDAN CIKIS" : "EXIT GAME");
    m_btnQuit.SetSubtitle(isTR ? "Masaustune Guvenle Don" : "Quit to Desktop");

    // Buton Tıklamaları
    if (m_hasSave && m_btnContinue.UpdateAndCheckClick()) {
        return MainMenuAction::CONTINUE_GAME;
    }

    if (m_btnNewGame.UpdateAndCheckClick()) {
        if (m_hasSave) {
            m_showConfirmNewGame = true;
            return MainMenuAction::NONE;
        } else {
            return MainMenuAction::START_NEW_GAME;
        }
    }

    if (m_btnSettings.UpdateAndCheckClick()) {
        return MainMenuAction::OPEN_SETTINGS;
    }

    if (m_btnQuit.UpdateAndCheckClick()) {
        return MainMenuAction::QUIT_GAME;
    }

    return MainMenuAction::NONE;
}

void MainMenuScreen::Draw() const {
    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    ClearBackground(Color{10, 13, 18, 255});

    // 1. Dinamik Siber Izgara (Matrix Grid Effect)
    float gridOffset = std::fmod(m_animTime * 20.0f, 48.0f);
    for (float x = 0; x < screenW; x += 48.0f) {
        DrawLine(static_cast<int>(x), 0, static_cast<int>(x), static_cast<int>(screenH), Color{18, 24, 34, 255});
    }
    for (float y = gridOffset; y < screenH; y += 48.0f) {
        DrawLine(0, static_cast<int>(y), static_cast<int>(screenW), static_cast<int>(y), Color{16, 22, 32, 255});
    }

    // 2. Animasyonlu Yükselen Parçacıklar
    for (const auto& p : m_particles) {
        Color c = p.color;
        c.a = static_cast<unsigned char>(p.alpha * 255.0f);
        DrawCircle(static_cast<int>(p.x), static_cast<int>(p.y), p.size, c);
    }

    // 3. Merkez Başlık ve Cyber Logo
    const float titleY = screenH * 0.16f;
    std::string mainTitle = "GAMEOFTEX";
    float titleW = UIFrame::MeasureTextCustom(mainTitle, 62.0f, true);
    float titleX = (screenW - titleW) * 0.5f;

    // Glowing Neon Aura
    float pulse = (std::sin(m_animTime * 2.5f) + 1.0f) * 0.5f;
    Color glowColor{0, 220, 255, static_cast<unsigned char>(40 + pulse * 60)};
    for (int offset = -3; offset <= 3; offset += 3) {
        UIFrame::DrawTextCustom(mainTitle, titleX + offset, titleY + offset, 62.0f, glowColor, true);
    }
    UIFrame::DrawTextCustom(mainTitle, titleX, titleY, 62.0f, Color{0, 240, 255, 255}, true);

    // Alt Başlık
    std::string subTitle = "CRYPTO MINING & ENERGY EMPIRE TYCOON";
    float subW = UIFrame::MeasureTextCustom(subTitle, 20.0f, true);
    float subX = (screenW - subW) * 0.5f;
    UIFrame::DrawTextCustom(subTitle, subX, titleY + 70.0f, 20.0f, Color{255, 200, 40, 255}, true);

    // Versiyon Rozeti
    std::string verTag = "[ENTERPRISE EDITION v1.9 - AUTOSAVE & PERSISTENCE ENGINE]";
    float verW = UIFrame::MeasureTextCustom(verTag, 13.0f, false);
    UIFrame::DrawTextCustom(verTag, (screenW - verW) * 0.5f, titleY + 102.0f, 13.0f, Color{130, 160, 200, 255}, false);

    // 4. Menü Butonları
    m_btnContinue.Draw();
    m_btnNewGame.Draw();
    m_btnSettings.Draw();
    m_btnQuit.Draw();

    // 5. Alt Bilgi
    std::string footerText = "Gelismis Termal Fizik, Gercek Zamanli Enerji Sebekesi ve Kripto Piyasasi Simulatöru";
    float fW = UIFrame::MeasureTextCustom(footerText, 14.0f, false);
    UIFrame::DrawTextCustom(footerText, (screenW - fW) * 0.5f, screenH - 35.0f, 14.0f, Color{90, 110, 140, 255}, false);

    // 6. Sıfırlama Onay Diyaloğu (Açıksa)
    if (m_showConfirmNewGame) {
        DrawRectangle(0, 0, static_cast<int>(screenW), static_cast<int>(screenH), Color{0, 0, 0, 180});

        float modalW = 540.0f;
        float modalH = 240.0f;
        float modalX = (screenW - modalW) * 0.5f;
        float modalY = (screenH - modalH) * 0.5f;

        UIFrame::DrawCard(Rectangle{modalX, modalY, modalW, modalH}, "! DIKKAT: MEVCUT KAYIT SILINECEK !", Color{255, 60, 60, 255});

        std::string warnText1 = "Zaten kayitli bir sirketiniz ve tesisleriniz bulunuyor:";
        std::string warnText2 = "Sirket: " + m_saveMeta.companyName + " (" + m_saveMeta.activeFacilityName + ")";
        std::string warnText3 = "Yeni oyuna baslarsaniz mevcut ilerlemeniz tamamen sifirlanacaktir!";

        UIFrame::DrawTextCustom(warnText1, modalX + 30.0f, modalY + 60.0f, 16.0f, WHITE, false);
        UIFrame::DrawTextCustom(warnText2, modalX + 30.0f, modalY + 88.0f, 18.0f, Color{255, 210, 50, 255}, true);
        UIFrame::DrawTextCustom(warnText3, modalX + 30.0f, modalY + 118.0f, 15.0f, Color{255, 120, 120, 255}, false);

        m_btnConfirmReset.Draw();
        m_btnCancelReset.Draw();
    }
}

} // namespace Render
