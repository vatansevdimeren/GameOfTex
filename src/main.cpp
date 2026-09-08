#include "raylib.h"
#include "core/GPU.hpp"
#include "core/MiningRig.hpp"
#include "core/Warehouse.hpp"
#include "core/ThermalModel.hpp"
#include "core/PowerGrid.hpp"
#include "core/EconomyManager.hpp"
#include "core/UserProfile.hpp"
#include "render/ShaderManager.hpp"
#include "render/RigRenderer.hpp"
#include "render/TextureManager.hpp"
#include "render/UIButton.hpp"
#include "render/UIFrame.hpp"
#include "render/LoginScreen.hpp"
#include "render/SettingsModal.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
#include <algorithm>

enum class GameState {
    LOGIN,
    GAMEPLAY
};

int main() {
    // 1. Pencere Yapılandırması (Resize ve F11 Tam Ekran desteği)
    constexpr int initialWidth = 1280;
    constexpr int initialHeight = 720;
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(initialWidth, initialHeight, "GameOfTex - Crypto Mining & Energy Tycoon (Warehouse Edition)");
    SetWindowMinSize(1100, 650);
    SetTargetFPS(60);

    // 2. Yüksek Çözünürlüklü Vektör Fontlarını Yükle (Segoe UI / Arial)
    Font fontRegular = LoadFontEx("C:/Windows/Fonts/segoeui.ttf", 36, nullptr, 0);
    Font fontBold = LoadFontEx("C:/Windows/Fonts/segoeuib.ttf", 40, nullptr, 0);

    if (fontRegular.texture.id == 0) {
        fontRegular = LoadFontEx("C:/Windows/Fonts/arial.ttf", 36, nullptr, 0);
        fontBold = LoadFontEx("C:/Windows/Fonts/arialbd.ttf", 40, nullptr, 0);
    }

    if (fontRegular.texture.id > 0) {
        SetTextureFilter(fontRegular.texture, TEXTURE_FILTER_BILINEAR);
        SetTextureFilter(fontBold.texture, TEXTURE_FILTER_BILINEAR);
        Render::UIFrame::InitTheme(fontRegular, fontBold);
    }

    GameState currentState = GameState::LOGIN;

    // 3. Profil ve Giriş Sistemi
    Core::UserProfile userProfile;
    Render::LoginScreen loginScreen;
    Render::SettingsModal settingsModal;

    // 4. Çekirdek Simülasyon Nesneleri (Clean Code / SRP)
    Core::EconomyManager economy(1000.0, 3100.0, "TEX");
    Core::PowerGrid powerGrid(8000.0, 0.15); // Çoklu rig için 8.0 kW sigorta limiti
    Core::ThermalModel thermalModel(22.0);   // 22°C Oda sıcaklığı
    thermalModel.SetCoolingPowerWatts(500.0);

    // Depo (Warehouse) - Çoklu Rig Yönetimi
    Core::Warehouse warehouse("Mega Madencilik Tesisi - Hangardır 01");
    if (auto* firstRig = warehouse.GetActiveRig()) {
        firstRig->InstallGPU(std::make_unique<Core::GPU>("RTX 3080", 100.0, 220.0, 1.05));
        firstRig->InstallGPU(std::make_unique<Core::GPU>("RTX 3070", 62.0, 130.0, 0.98));
    }

    // 5. Render, Texture ve Shader Sistemi
    Render::ShaderManager shaderManager;
    shaderManager.LoadThermalShader("src/shaders/thermal_vision.fs");

    Render::TextureManager textureManager;
    textureManager.LoadTextures("assets/textures");

    Render::RigRenderer rigRenderer;
    double animTime = 0.0;

    // 6. İnteraktif Butonlar
    Render::UIButton btnBuyGPU(Rectangle{}, "GPU SATIN AL", "Maliyet: $600",
                              Color{25, 45, 60, 255}, Color{0, 200, 255, 255});

    Render::UIButton btnBuyRig(Rectangle{}, "YENI RIG SATIN AL", "Maliyet: $2,500 (6 Slotlu Raf)",
                              Color{30, 50, 40, 255}, Color{0, 255, 140, 255});

    Render::UIButton btnPrevRig(Rectangle{}, "< ONCEKI", "", Color{30, 35, 45, 255}, Color{0, 200, 255, 255});
    Render::UIButton btnNextRig(Rectangle{}, "SONRAKI >", "", Color{30, 35, 45, 255}, Color{0, 200, 255, 255});

    Render::UIButton btnSellCrypto(Rectangle{}, "KRIPTO PARALARI SAT", "Anlik Deger: $0.00",
                                  Color{45, 40, 20, 255}, Color{255, 200, 0, 255});

    Render::UIButton btnAddCooler(Rectangle{}, "SANAYI KLIMASI EKLE", "Maliyet: $150 (+250W)",
                                 Color{20, 45, 50, 255}, Color{0, 240, 200, 255});

    Render::UIButton btnOverclock(Rectangle{}, "HIZ ASIRTMA (+5% OC)", "Daha Fazla MH/s",
                                 Color{45, 30, 25, 255}, Color{255, 120, 0, 255});

    Render::UIButton btnUndervolt(Rectangle{}, "VOLTAJ DUSUR (-5% UV)", "Daha Az Guc ve Isi",
                                 Color{25, 35, 45, 255}, Color{100, 180, 255, 255});

    Render::UIButton btnThermalToggle(Rectangle{}, "TERMAL VIZYON MODU", "FLIR Isı Haritasi Shader",
                                     Color{35, 25, 45, 255}, Color{200, 0, 255, 255});

    Render::UIButton btnResetBreaker(Rectangle{}, "SIGORTA SALTERINI AC", "Sistem Normal",
                                    Color{45, 20, 25, 255}, Color{255, 50, 50, 255});

    Render::UIButton btnOpenSettings(Rectangle{}, "AYARLAR", "", Color{35, 42, 56, 255}, Color{0, 220, 255, 255});

    // 7. Ana Oyun Döngüsü
    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();
        animTime += dt;

        // F11: Tam Ekran Geçişi
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        const float screenW = static_cast<float>(GetScreenWidth());
        const float screenH = static_cast<float>(GetScreenHeight());

        // --- GİRİŞ EKRANI DURUMU ---
        if (currentState == GameState::LOGIN) {
            if (loginScreen.Update(userProfile)) {
                if (!userProfile.HasClaimedBonus()) {
                    economy.AddFiat(1000.0);
                    userProfile.SetClaimedBonus(true);
                }
                currentState = GameState::GAMEPLAY;
            }

            BeginDrawing();
            loginScreen.Draw();
            EndDrawing();
            continue;
        }

        // ==================== OYUN İÇİ (GAMEPLAY) ====================

        // Ayarlar Modalı Açıksa Güncelle
        if (settingsModal.IsOpen()) {
            settingsModal.Update();
        }

        // --- DİNAMİK RESPONSIVE DÜZEN HESAPLAMALARI ---
        const float pad = 16.0f;
        const float headerH = 76.0f;
        const float footerH = 42.0f;

        // Üst Rozetlerin Dinamik Genişliği (5 Rozet + 1 Ayarlar Butonu)
        const float totalBadgesW = screenW - (pad * 2.0f);
        const float badgeGap = 10.0f;
        const float settingsBtnW = 120.0f;
        const float remainingW = totalBadgesW - settingsBtnW - (badgeGap * 5.0f);
        const float badgeW = remainingW / 5.0f;
        const float badgeH = 54.0f;
        const float badgeY = (headerH - badgeH) / 2.0f;

        btnOpenSettings.SetBounds(Rectangle{screenW - pad - settingsBtnW, badgeY, settingsBtnW, badgeH});
        if (!settingsModal.IsOpen() && btnOpenSettings.UpdateAndCheckClick()) {
            settingsModal.Open();
        }

        // Ana İçerik Alanı
        const float contentY = headerH + 12.0f;
        const float contentH = screenH - contentY - footerH - 12.0f;

        // Sol Viewport ve Sağ Kontrol Paneli Genişlikleri
        const float leftW = (screenW - (pad * 2.0f) - 14.0f) * 0.66f;
        const float rightW = (screenW - (pad * 2.0f) - 14.0f) * 0.34f;
        const float rightX = pad + leftW + 14.0f;

        // Rig Gezinme Butonları (Sol Panel Üstü)
        btnPrevRig.SetBounds(Rectangle{pad + leftW - 230.0f, contentY + 10.0f, 105.0f, 32.0f});
        btnNextRig.SetBounds(Rectangle{pad + leftW - 120.0f, contentY + 10.0f, 105.0f, 32.0f});

        if (!settingsModal.IsOpen()) {
            if (btnPrevRig.UpdateAndCheckClick()) warehouse.PreviousRig();
            if (btnNextRig.UpdateAndCheckClick()) warehouse.NextRig();
        }

        // Sağ Paneldeki Butonların Dinamik Konumları (8 Buton)
        const float btnW = rightW - 32.0f;
        const float btnH = std::clamp((contentH - 85.0f) / 8.2f, 48.0f, 64.0f);
        const float btnGap = 6.0f;
        const float startBtnY = contentY + 50.0f;
        const float btnX = rightX + 16.0f;

        btnBuyGPU.SetBounds(Rectangle{btnX, startBtnY + (0 * (btnH + btnGap)), btnW, btnH});
        btnBuyRig.SetBounds(Rectangle{btnX, startBtnY + (1 * (btnH + btnGap)), btnW, btnH});
        btnSellCrypto.SetBounds(Rectangle{btnX, startBtnY + (2 * (btnH + btnGap)), btnW, btnH});
        btnAddCooler.SetBounds(Rectangle{btnX, startBtnY + (3 * (btnH + btnGap)), btnW, btnH});
        btnOverclock.SetBounds(Rectangle{btnX, startBtnY + (4 * (btnH + btnGap)), btnW, btnH});
        btnUndervolt.SetBounds(Rectangle{btnX, startBtnY + (5 * (btnH + btnGap)), btnW, btnH});
        btnThermalToggle.SetBounds(Rectangle{btnX, startBtnY + (6 * (btnH + btnGap)), btnW, btnH});
        btnResetBreaker.SetBounds(Rectangle{btnX, startBtnY + (7 * (btnH + btnGap)), btnW, btnH});

        // --- BUTON TIKLAMA VE AKSİYON KONTROLLERİ ---
        auto* activeRig = warehouse.GetActiveRig();

        if (!settingsModal.IsOpen()) {
            // 1. GPU Satın Al
            if (activeRig) {
                std::string gpuSub = "Maliyet: $600 | Secili Rig: " + std::to_string(activeRig->GetGPUCount()) + "/" + std::to_string(activeRig->GetMaxCapacity());
                btnBuyGPU.SetSubtitle(gpuSub);
                btnBuyGPU.SetDisabled(economy.GetFiatBalance() < 600.0 || activeRig->GetGPUCount() >= activeRig->GetMaxCapacity());
                if (btnBuyGPU.UpdateAndCheckClick()) {
                    if (economy.DeductFiat(600.0)) {
                        activeRig->InstallGPU(std::make_unique<Core::GPU>("RTX 3070 Ti", 80.0, 180.0, 1.0));
                    }
                }
            }

            // 2. Yeni Rig Satın Al ($2,500)
            btnBuyRig.SetDisabled(economy.GetFiatBalance() < 2500.0);
            std::string rigSub = "Maliyet: $2,500 | Toplam Rig: " + std::to_string(warehouse.GetRigCount());
            btnBuyRig.SetSubtitle(rigSub);
            if (btnBuyRig.UpdateAndCheckClick()) {
                if (economy.DeductFiat(2500.0)) {
                    std::string newRigName = std::string("Rig ") + (warehouse.GetRigCount() < 9 ? "0" : "") + std::to_string(warehouse.GetRigCount() + 1) + " - Ek Depo";
                    warehouse.AddNewRig(newRigName, 6);
                    warehouse.SetActiveRigIndex(warehouse.GetRigCount() - 1);
                }
            }

            // 3. Kripto Sat
            const double cryptoValue = economy.GetCryptoBalance() * economy.GetCryptoPrice();
            std::ostringstream ssCryptoVal;
            ssCryptoVal << "Bozdurulacak: $" << std::fixed << std::setprecision(2) << cryptoValue;
            btnSellCrypto.SetSubtitle(ssCryptoVal.str());
            btnSellCrypto.SetDisabled(economy.GetCryptoBalance() <= 0.0001);
            if (btnSellCrypto.UpdateAndCheckClick()) {
                economy.SellCrypto(economy.GetCryptoBalance());
            }

            // 4. Klima Ekle
            std::string coolSub = "Maliyet: $150 | Toplam: " + std::to_string(static_cast<int>(thermalModel.GetCoolingPowerWatts())) + "W";
            btnAddCooler.SetSubtitle(coolSub);
            btnAddCooler.SetDisabled(economy.GetFiatBalance() < 150.0);
            if (btnAddCooler.UpdateAndCheckClick()) {
                if (economy.DeductFiat(150.0)) {
                    thermalModel.SetCoolingPowerWatts(thermalModel.GetCoolingPowerWatts() + 250.0);
                }
            }

            // 5. Overclock
            if (btnOverclock.UpdateAndCheckClick()) {
                for (const auto& r : warehouse.GetAllRigs()) {
                    if (r) {
                        for (const auto& gpu : r->GetGPUs()) {
                            if (gpu) gpu->SetOverclockMultiplier(gpu->GetOverclockMultiplier() + 0.05);
                        }
                    }
                }
            }

            // 6. Undervolt
            if (btnUndervolt.UpdateAndCheckClick()) {
                for (const auto& r : warehouse.GetAllRigs()) {
                    if (r) {
                        for (const auto& gpu : r->GetGPUs()) {
                            if (gpu) gpu->SetOverclockMultiplier(gpu->GetOverclockMultiplier() - 0.05);
                        }
                    }
                }
            }

            // 7. Termal Vizyon
            if (btnThermalToggle.UpdateAndCheckClick() || IsKeyPressed(KEY_TAB)) {
                shaderManager.ToggleThermal();
            }
            btnThermalToggle.SetSubtitle(shaderManager.IsThermalActive() ? "[AKTIF - RENKLI ISI HARITASI]" : "[KAPALI - NORMAL GORUNUM]");

            // 8. Sigorta Şalteri
            btnResetBreaker.SetDisabled(!powerGrid.IsBreakerTripped());
            btnResetBreaker.SetSubtitle(powerGrid.IsBreakerTripped() ? "! SALTER ATTI - TIKLA !" : "Sebeke Normal");
            if (btnResetBreaker.UpdateAndCheckClick() || (powerGrid.IsBreakerTripped() && IsKeyPressed(KEY_R))) {
                powerGrid.ResetBreaker();
            }
        }

        // --- SİMÜLASYON MOTORU HESAPLAMALARI (TÜM DEPO İÇİN) ---
        powerGrid.ResetStep();

        if (!powerGrid.IsBreakerTripped()) {
            const double warehousePower = warehouse.CalculateTotalPowerWatts();
            powerGrid.AddConsumerWatts(warehousePower);

            const double electricityCost = powerGrid.CalculateCostForDuration(dt);
            economy.DeductFiat(electricityCost);

            thermalModel.Update(warehousePower, dt);

            double maxCardTemp = 0.0;
            for (const auto& r : warehouse.GetAllRigs()) {
                if (r) {
                    for (const auto& gpu : r->GetGPUs()) {
                        if (gpu) {
                            double cardTemp = thermalModel.CalculateGPUTemperature(gpu->GetEffectivePowerWatts(), 0.85);
                            if (cardTemp > maxCardTemp) maxCardTemp = cardTemp;
                            gpu->SetThrottled(Core::ThermalModel::IsOverheating(cardTemp));
                        }
                    }
                }
            }

            float normTemp = static_cast<float>((maxCardTemp - 20.0) / 70.0);
            shaderManager.SetThermalState(shaderManager.IsThermalActive(), normTemp);

            economy.MineCoins(warehouse.CalculateTotalHashrate(), dt);
        }

        economy.UpdateMarket(dt);

        // --- ÇİZİM AŞAMASI ---
        BeginDrawing();
        ClearBackground(Color{10, 12, 16, 255});

        // Varsa ultra-res arka plan, yoksa ızgara
        if (textureManager.HasWarehouseBackground()) {
            textureManager.DrawBackground(Rectangle{0, 0, screenW, screenH});
        } else {
            for (int x = 0; x < static_cast<int>(screenW); x += 48) DrawLine(x, 0, x, static_cast<int>(screenH), Color{16, 19, 26, 255});
            for (int y = 0; y < static_cast<int>(screenH); y += 48) DrawLine(0, y, static_cast<int>(screenW), y, Color{16, 19, 26, 255});
        }

        // 1. ÜST HUD PANELİ
        DrawRectangle(0, 0, static_cast<int>(screenW), static_cast<int>(headerH), Color{15, 18, 24, 245});
        DrawLine(0, static_cast<int>(headerH), static_cast<int>(screenW), static_cast<int>(headerH), Color{38, 46, 62, 255});

        const char* avatarIcons[] = {"[SIBER]", "[SANAYI]", "[UZAY]"};
        const char* currentAvatar = avatarIcons[userProfile.GetAvatarIndex() % 3];

        std::ostringstream ssFiat, ssCrypto, ssPrice, ssHash, ssTemp;
        ssFiat << "$" << std::fixed << std::setprecision(2) << economy.GetFiatBalance();
        ssCrypto << std::fixed << std::setprecision(4) << economy.GetCryptoBalance() << " " << economy.GetCoinSymbol();
        ssPrice << "$" << std::fixed << std::setprecision(0) << economy.GetCryptoPrice();
        ssHash << std::fixed << std::setprecision(1) << warehouse.CalculateTotalHashrate() << " MH/s";
        ssTemp << std::fixed << std::setprecision(1) << thermalModel.GetAmbientTemperature() << " C";

        Render::UIFrame::DrawStatBadge(pad + (0 * (badgeW + badgeGap)), badgeY, badgeW, badgeH, currentAvatar, "SIRKET", userProfile.GetCompanyName(), Color{0, 230, 255, 255});
        Render::UIFrame::DrawStatBadge(pad + (1 * (badgeW + badgeGap)), badgeY, badgeW, badgeH, "[CASH]", "NAKIT", ssFiat.str(), Color{0, 255, 150, 255});
        Render::UIFrame::DrawStatBadge(pad + (2 * (badgeW + badgeGap)), badgeY, badgeW, badgeH, "[WALLET]", "KRIPTO", ssCrypto.str(), Color{255, 210, 50, 255});
        Render::UIFrame::DrawStatBadge(pad + (3 * (badgeW + badgeGap)), badgeY, badgeW, badgeH, "[MARKET]", "1 TEX", ssPrice.str(), Color{170, 200, 255, 255});
        Render::UIFrame::DrawStatBadge(pad + (4 * (badgeW + badgeGap)), badgeY, badgeW, badgeH, "[SPEED]", "TOPLAM HASH", ssHash.str(), Color{100, 230, 255, 255});

        btnOpenSettings.Draw();

        // 2. SOL PANEL: VIEWPORT ÇERÇEVESİ (SEÇİLİ RİG VE DEPO BİLGİSİ)
        const Rectangle viewportRect{pad, contentY, leftW, contentH};
        std::string viewportTitle = warehouse.GetFacilityName() + " (" + std::to_string(warehouse.GetActiveRigIndex() + 1) + "/" + std::to_string(warehouse.GetRigCount()) + " RIG)";
        Render::UIFrame::DrawCard(viewportRect, viewportTitle, Color{0, 220, 255, 255});

        // Gezinme butonları
        btnPrevRig.Draw();
        btnNextRig.Draw();

        // Rig'i sol kart içerisinde ortala
        constexpr float rigBaseW = 720.0f;
        const float rigX = pad + (leftW - rigBaseW) / 2.0f;
        const float rigY = contentY + 60.0f;

        if (activeRig) {
            shaderManager.BeginShader();
            rigRenderer.DrawRig(*activeRig, thermalModel, static_cast<int>(rigX), static_cast<int>(rigY), animTime, &textureManager);
            shaderManager.EndShader();
        }

        // Güç Gösterge Barı (Ortalanmış)
        const float powerRatio = static_cast<float>(powerGrid.GetTotalConsumptionWatts() / powerGrid.GetMaxCapacityWatts());
        const Color powerColor = (powerRatio > 0.85f) ? Color{255, 50, 50, 255} : ((powerRatio > 0.60f) ? Color{255, 180, 0, 255} : Color{0, 230, 130, 255});
        const std::string powerText = "DEPO SEBEKE YUKU: " + std::to_string(static_cast<int>(powerGrid.GetTotalConsumptionWatts())) +
                                      "W / " + std::to_string(static_cast<int>(powerGrid.GetMaxCapacityWatts())) + "W";

        const float barW = std::min(rigBaseW, leftW - 40.0f);
        const float barX = pad + (leftW - barW) / 2.0f;
        const float barY = rigY + 340.0f;

        Render::UIFrame::DrawProgressBar(Rectangle{barX, barY, barW, 28.0f}, powerRatio, powerColor, powerText);

        // Termal veya Sigorta Bildirim Kutusu
        if (powerGrid.IsBreakerTripped()) {
            DrawRectangleRounded(Rectangle{barX, barY + 38.0f, barW, 40.0f}, 0.2f, 4, Color{190, 20, 20, 240});
            Render::UIFrame::DrawTextCustom("! SEBEKE ASIRI YUKLENDI - SIGORTA ATTI ! SAG PANELDEN SALTERI ACIN",
                                           barX + 24.0f, barY + 48.0f, 16.0f, WHITE, true);
        } else if (shaderManager.IsThermalActive()) {
            DrawRectangleRounded(Rectangle{barX, barY + 38.0f, barW, 40.0f}, 0.2f, 4, Color{32, 16, 52, 230});
            DrawRectangleRoundedLines(Rectangle{barX, barY + 38.0f, barW, 40.0f}, 0.2f, 4, 1.4f, Color{220, 0, 255, 255});
            Render::UIFrame::DrawTextCustom("[CANLI TERMAL FLIR VIZYONU AKTIF] - Isi dagilimi fragment shader ile renklendiriliyor",
                                           barX + 24.0f, barY + 48.0f, 15.0f, Color{230, 130, 255, 255}, true);
        }

        // 3. SAĞ PANEL: KONTROL VE YÜKSELTME MERKEZİ
        const Rectangle managementRect{rightX, contentY, rightW, contentH};
        Render::UIFrame::DrawCard(managementRect, "DEPO VE OPERASYON MERKEZI", Color{255, 200, 0, 255});

        btnBuyGPU.Draw();
        btnBuyRig.Draw();
        btnSellCrypto.Draw();
        btnAddCooler.Draw();
        btnOverclock.Draw();
        btnUndervolt.Draw();
        btnThermalToggle.Draw();
        btnResetBreaker.Draw();

        // 4. ALT BİLGİ VE KONTROL ŞERİDİ
        DrawRectangle(0, static_cast<int>(screenH - footerH), static_cast<int>(screenW), static_cast<int>(footerH), Color{14, 17, 23, 250});
        DrawLine(0, static_cast<int>(screenH - footerH), static_cast<int>(screenW), static_cast<int>(screenH - footerH), Color{35, 42, 56, 255});

        Render::UIFrame::DrawTextCustom("[F11] Tam Ekran  |  [AYARLAR] ile font ve buton boyutunu istediginiz gibi buyutebilirsiniz!",
                                       pad + 10.0f, screenH - footerH + 12.0f, 14.0f, Color{150, 165, 190, 255}, false);

        std::string verTag = "GameOfTex v1.3 [Warehouse & Multi-Rig]";
        float verW = Render::UIFrame::MeasureTextCustom(verTag, 14.0f, false);
        Render::UIFrame::DrawTextCustom(verTag, screenW - verW - pad - 10.0f, screenH - footerH + 12.0f, 14.0f, Color{100, 120, 150, 255}, false);

        // 5. AYARLAR MODAL PENCERESİ (AÇIKSA EN ÜSTTE ÇİZİLİR)
        if (settingsModal.IsOpen()) {
            settingsModal.Draw();
        }

        EndDrawing();
    }

    // 8. Temizlik
    textureManager.Unload();
    if (fontRegular.texture.id > 0) {
        UnloadFont(fontRegular);
        UnloadFont(fontBold);
    }
    CloseWindow();
    return 0;
}
