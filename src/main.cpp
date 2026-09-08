#include "raylib.h"
#include "core/GPU.hpp"
#include "core/MiningRig.hpp"
#include "core/Warehouse.hpp"
#include "core/ThermalModel.hpp"
#include "core/PowerGrid.hpp"
#include "core/EconomyManager.hpp"
#include "core/UserProfile.hpp"
#include "core/CoolingManager.hpp"
#include "core/Localization.hpp"
#include "render/ShaderManager.hpp"
#include "render/RigRenderer.hpp"
#include "render/TextureManager.hpp"
#include "render/UIButton.hpp"
#include "render/UIFrame.hpp"
#include "render/LoginScreen.hpp"
#include "render/SettingsModal.hpp"
#include "render/GPUInspectionModal.hpp"
#include "core/MarketCatalog.hpp"
#include "render/MarketModal.hpp"

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
    SetExitKey(KEY_NULL); // ESC tuşunun oyunu aniden kapatmasını engelle

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
    enum class WarehouseViewMode {
        RIG_DETAIL,
        OVERVIEW_GRID
    };
    WarehouseViewMode currentViewMode = WarehouseViewMode::RIG_DETAIL;

    // 3. Profil, Giriş ve Modal Pencereleri
    Core::UserProfile userProfile;
    Render::LoginScreen loginScreen;
    Render::SettingsModal settingsModal;
    Render::GPUInspectionModal gpuInspectionModal;
    Core::MarketCatalog marketCatalog;
    Render::MarketModal marketModal;

    // 4. Çekirdek Simülasyon Nesneleri (Clean Code / SRP)
    Core::EconomyManager economy(1000.0, 3100.0, "TEX");
    Core::PowerGrid powerGrid(3600.0, 0.15); // Standart pano (3.6 kW) ile başlar, marketten yükseltilebilir
    Core::ThermalModel thermalModel(22.0);    // 22°C Oda sıcaklığı
    Core::CoolingManager coolingManager;
    thermalModel.SetCoolingPowerWatts(coolingManager.CalculateTotalCoolingWatts());

    // Depo (Warehouse) - Çoklu Rig Yönetimi
    Core::Warehouse warehouse("Mega Madencilik Tesisi - Hangar 01");
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
    Render::UIButton btnToggleRigPower(Rectangle{}, "RIG'I KAPAT", "", Color{60, 25, 25, 255}, Color{255, 70, 70, 255});
    Render::UIButton btnSellRig(Rectangle{}, "RIG'I SAT", "+$1,200", Color{50, 40, 20, 255}, Color{255, 160, 0, 255});

    Render::UIButton btnTabRigDetail(Rectangle{}, "DETAYLI RIG", "", Color{30, 50, 75, 255}, Color{0, 220, 255, 255});
    Render::UIButton btnTabOverview(Rectangle{}, "DEPO GENEL BAKIS", "", Color{25, 30, 42, 255}, Color{0, 220, 255, 255});

    Render::UIButton btnSellCrypto(Rectangle{}, "KRIPTO PARALARI SAT", "Anlik Deger: $0.00",
                                  Color{45, 40, 20, 255}, Color{255, 200, 0, 255});

    Render::UIButton btnUpgradeCooling(Rectangle{}, "SOGUTMAYI YUKSELT", "",
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

        // ESC Tuşu: Modal açıksa kapat, değilse tam ekrandan küçük pencereli moda dön!
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (marketModal.IsOpen()) {
                marketModal.Close();
            } else if (gpuInspectionModal.IsOpen()) {
                gpuInspectionModal.Close();
            } else if (settingsModal.IsOpen()) {
                settingsModal.Close();
            } else if (IsWindowFullscreen()) {
                ToggleFullscreen();
            }
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
        auto* activeRig = warehouse.GetActiveRig();

        // GPU İnceleme Modalı Açıksa Güncelle (Tamir ve Hurdaya Satma)
        if (gpuInspectionModal.IsOpen()) {
            double repairCost = 0.0;
            bool scrapRequested = false;
            gpuInspectionModal.Update(economy.GetFiatBalance(), repairCost, scrapRequested);
            if (repairCost > 0.0) {
                economy.DeductFiat(repairCost);
            }
            if (scrapRequested && activeRig) {
                size_t slot = gpuInspectionModal.GetSlotIndex();
                auto removed = activeRig->RemoveGPU(slot);
                if (removed) {
                    economy.AddFiat(75.0); // Hurda metal ve parça geri dönüşüm geliri
                }
            }
        }

        // Donanım Marketi Modalı Açıksa Güncelle ve Satın Almaları İşle
        if (marketModal.IsOpen()) {
            auto action = marketModal.Update(economy, warehouse, coolingManager, marketCatalog, powerGrid);
            if (action.type == Render::MarketPurchaseAction::ActionType::BUY_GPU) {
                const auto* model = marketCatalog.GetGPUModel(action.itemIndex);
                if (model && activeRig && activeRig->GetGPUCount() < activeRig->GetMaxCapacity()) {
                    if (economy.DeductFiat(model->priceUSD)) {
                        activeRig->InstallGPU(std::make_unique<Core::GPU>(model->name, model->hashrate, model->powerWatts, 1.0));
                    }
                }
            } else if (action.type == Render::MarketPurchaseAction::ActionType::BUY_POWER) {
                const auto& powerUpgrades = marketCatalog.GetPowerUpgrades();
                if (action.itemIndex < powerUpgrades.size()) {
                    const auto& upg = powerUpgrades[action.itemIndex];
                    if (!upg.isInstalled && economy.DeductFiat(upg.priceUSD)) {
                        marketCatalog.PurchasePowerUpgrade(action.itemIndex);
                        powerGrid.SetMaxCapacityWatts(upg.capacityWatts);
                    }
                }
            } else if (action.type == Render::MarketPurchaseAction::ActionType::BUY_COOLING) {
                double cost = coolingManager.UpgradeTier(action.itemIndex);
                if (cost > 0.0) {
                    economy.DeductFiat(cost);
                    thermalModel.SetCoolingPowerWatts(coolingManager.CalculateTotalCoolingWatts());
                }
            } else if (action.type == Render::MarketPurchaseAction::ActionType::BUY_FACILITY) {
                const auto& facilities = marketCatalog.GetFacilityUpgrades();
                if (action.itemIndex < facilities.size()) {
                    const auto& fac = facilities[action.itemIndex];
                    if (!fac.isInstalled && economy.DeductFiat(fac.priceUSD)) {
                        marketCatalog.PurchaseFacilityUpgrade(action.itemIndex);
                        if (fac.addedGreenWatts > 0.0) {
                            powerGrid.AddProducerWatts(fac.addedGreenWatts);
                        }
                    }
                }
            }
        }

        // Ayarlar Modalı Açıksa Güncelle
        if (settingsModal.IsOpen()) {
            settingsModal.Update(economy);
        }

        // --- DİNAMİK RESPONSIVE DÜZEN HESAPLAMALARI ---
        const float pad = 16.0f;
        const float headerH = 76.0f;
        const float footerH = 42.0f;

        // Üst Rozetlerin Dinamik Genişliği
        const float totalBadgesW = screenW - (pad * 2.0f);
        const float badgeGap = 10.0f;
        const float settingsBtnW = 120.0f;
        const float remainingW = totalBadgesW - settingsBtnW - (badgeGap * 5.0f);
        const float badgeW = remainingW / 5.0f;
        const float badgeH = 54.0f;
        const float badgeY = (headerH - badgeH) / 2.0f;

        btnOpenSettings.SetBounds(Rectangle{screenW - pad - settingsBtnW, badgeY, settingsBtnW, badgeH});
        if (!settingsModal.IsOpen() && !gpuInspectionModal.IsOpen() && btnOpenSettings.UpdateAndCheckClick()) {
            settingsModal.Open();
        }

        // Ana İçerik Alanı
        const float contentY = headerH + 12.0f;
        const float contentH = screenH - contentY - footerH - 12.0f;

        // Sol Viewport ve Sağ Kontrol Paneli Genişlikleri
        const float leftW = (screenW - (pad * 2.0f) - 14.0f) * 0.66f;
        const float rightW = (screenW - (pad * 2.0f) - 14.0f) * 0.34f;
        const float rightX = pad + leftW + 14.0f;

        bool isTR = (Core::LocalizationManager::Get().GetLanguage() == Core::Language::TURKISH);

        // Sekme Butonları (Rig Detayı vs Depo Kuşbakışı Genel Bakış)
        btnTabRigDetail.SetTitle(Core::LocalizationManager::Tr("TAB_RIG_DETAIL"));
        btnTabOverview.SetTitle(Core::LocalizationManager::Tr("TAB_OVERVIEW"));
        btnTabRigDetail.SetBounds(Rectangle{pad + 16.0f, contentY + 10.0f, 130.0f, 32.0f});
        btnTabOverview.SetBounds(Rectangle{pad + 152.0f, contentY + 10.0f, 160.0f, 32.0f});

        if (!settingsModal.IsOpen() && !gpuInspectionModal.IsOpen()) {
            if (btnTabRigDetail.UpdateAndCheckClick()) currentViewMode = WarehouseViewMode::RIG_DETAIL;
            if (btnTabOverview.UpdateAndCheckClick()) currentViewMode = WarehouseViewMode::OVERVIEW_GRID;
        }

        // Rig Gezinme ve Yönetim Butonları (Sol Panel Üstü)
        btnToggleRigPower.SetBounds(Rectangle{pad + leftW - 460.0f, contentY + 10.0f, 110.0f, 32.0f});
        btnSellRig.SetBounds(Rectangle{pad + leftW - 345.0f, contentY + 10.0f, 110.0f, 32.0f});
        btnPrevRig.SetBounds(Rectangle{pad + leftW - 230.0f, contentY + 10.0f, 105.0f, 32.0f});
        btnNextRig.SetBounds(Rectangle{pad + leftW - 120.0f, contentY + 10.0f, 105.0f, 32.0f});

        btnPrevRig.SetTitle(Core::LocalizationManager::Tr("RIG_PREV"));
        btnNextRig.SetTitle(Core::LocalizationManager::Tr("RIG_NEXT"));

        if (activeRig) {
            btnToggleRigPower.SetTitle(activeRig->IsPoweredOn() ? Core::LocalizationManager::Tr("RIG_POWER_OFF") : Core::LocalizationManager::Tr("RIG_POWER_ON"));
            btnToggleRigPower.SetSubtitle(activeRig->IsPoweredOn() ? Core::LocalizationManager::Tr("RIG_POWER_OFF_SUB") : Core::LocalizationManager::Tr("RIG_POWER_ON_SUB"));
        }
        btnSellRig.SetTitle(Core::LocalizationManager::Tr("RIG_SELL"));
        btnSellRig.SetSubtitle(std::string("+") + economy.FormatFiat(1200.0) + (isTR ? " Hurda" : " Scrap"));
        btnSellRig.SetDisabled(warehouse.GetRigCount() <= 1);

        if (!settingsModal.IsOpen() && !gpuInspectionModal.IsOpen()) {
            if (activeRig && btnToggleRigPower.UpdateAndCheckClick()) {
                activeRig->TogglePower();
            }
            if (btnSellRig.UpdateAndCheckClick() && warehouse.GetRigCount() > 1) {
                if (warehouse.RemoveRig(warehouse.GetActiveRigIndex())) {
                    economy.AddFiat(1200.0);
                    activeRig = warehouse.GetActiveRig();
                }
            }
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
        btnUpgradeCooling.SetBounds(Rectangle{btnX, startBtnY + (3 * (btnH + btnGap)), btnW, btnH});
        btnOverclock.SetBounds(Rectangle{btnX, startBtnY + (4 * (btnH + btnGap)), btnW, btnH});
        btnUndervolt.SetBounds(Rectangle{btnX, startBtnY + (5 * (btnH + btnGap)), btnW, btnH});
        btnThermalToggle.SetBounds(Rectangle{btnX, startBtnY + (6 * (btnH + btnGap)), btnW, btnH});
        btnResetBreaker.SetBounds(Rectangle{btnX, startBtnY + (7 * (btnH + btnGap)), btnW, btnH});

        // Kart Tıklama Tespiti (Viewport içerisindeki GPU'ya tıklandı mı?)
        constexpr float rigBaseW = 720.0f;
        const float rigX = pad + (leftW - rigBaseW) / 2.0f;
        const float rigY = contentY + 60.0f;

        if (!settingsModal.IsOpen() && !gpuInspectionModal.IsOpen() && !marketModal.IsOpen() && activeRig && currentViewMode == WarehouseViewMode::RIG_DETAIL) {
            Vector2 mouse = GetMousePosition();
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                int clickedIndex = rigRenderer.GetClickedGPUIndex(static_cast<int>(rigX), static_cast<int>(rigY), activeRig->GetGPUCount(), mouse);
                if (clickedIndex >= 0 && clickedIndex < static_cast<int>(activeRig->GetGPUCount())) {
                    auto* targetCard = activeRig->GetGPU(static_cast<size_t>(clickedIndex));
                    double cardTemp = activeRig->IsPoweredOn() ? thermalModel.CalculateGPUTemperature(targetCard->GetEffectivePowerWatts(), 0.85) : thermalModel.GetAmbientTemperature();
                    gpuInspectionModal.Open(targetCard, cardTemp, static_cast<size_t>(clickedIndex));
                }
            }
        }

        if (!settingsModal.IsOpen() && !gpuInspectionModal.IsOpen() && !marketModal.IsOpen()) {
            // 1. Donanım ve Tesis Marketi
            btnBuyGPU.SetTitle(isTR ? "🛒 DONANIM MARKETI" : "🛒 HARDWARE STORE");
            std::string gpuSub = isTR ? "Farkli Modeller, Trafo ve Tesis" : "Different Models, Power & Facilities";
            btnBuyGPU.SetSubtitle(gpuSub);
            btnBuyGPU.SetDisabled(false);
            if (btnBuyGPU.UpdateAndCheckClick()) {
                marketModal.Open();
            }

            // 2. Yeni Rig Satın Al ($2,500)
            btnBuyRig.SetTitle(Core::LocalizationManager::Tr("BTN_BUY_RIG"));
            btnBuyRig.SetDisabled(economy.GetFiatBalance() < 2500.0);
            std::string rigSub = (isTR ? "Maliyet: " : "Cost: ") + economy.FormatFiat(2500.0) + (isTR ? " | Toplam: " : " | Total: ") + std::to_string(warehouse.GetRigCount());
            btnBuyRig.SetSubtitle(rigSub);
            if (btnBuyRig.UpdateAndCheckClick()) {
                if (economy.DeductFiat(2500.0)) {
                    std::string newRigName = std::string("Rig ") + (warehouse.GetRigCount() < 9 ? "0" : "") + std::to_string(warehouse.GetRigCount() + 1) + " - Frame";
                    warehouse.AddNewRig(newRigName, 6);
                    warehouse.SetActiveRigIndex(warehouse.GetRigCount() - 1);
                }
            }

            // 3. Kripto Sat
            btnSellCrypto.SetTitle(Core::LocalizationManager::Tr("BTN_SELL_CRYPTO"));
            const double cryptoValue = economy.GetCryptoBalance() * economy.GetCryptoPrice();
            std::string cryptoSub = (isTR ? "Bozdurulacak: " : "To Liquidate: ") + economy.FormatFiat(cryptoValue);
            btnSellCrypto.SetSubtitle(cryptoSub);
            btnSellCrypto.SetDisabled(economy.GetCryptoBalance() <= 0.0001);
            if (btnSellCrypto.UpdateAndCheckClick()) {
                economy.SellCrypto(economy.GetCryptoBalance());
            }

            // 4. Soğutmayı Kademeli Yükselt
            size_t nextCoolingTier = 0;
            const auto& coolingTiers = coolingManager.GetTiers();
            for (size_t i = 1; i < coolingTiers.size(); ++i) {
                if (!coolingTiers[i].isInstalled) {
                    nextCoolingTier = i;
                    break;
                }
            }

            if (nextCoolingTier > 0) {
                const auto& nextTierInfo = coolingTiers[nextCoolingTier];
                btnUpgradeCooling.SetTitle(nextTierInfo.name);
                std::string coolSub = (isTR ? "Maliyet: " : "Cost: ") + economy.FormatFiat(nextTierInfo.cost) + " (+" + std::to_string(static_cast<int>(nextTierInfo.addedCoolingWatts)) + "W)";
                btnUpgradeCooling.SetSubtitle(coolSub);
                btnUpgradeCooling.SetDisabled(economy.GetFiatBalance() < nextTierInfo.cost);

                if (btnUpgradeCooling.UpdateAndCheckClick()) {
                    if (economy.DeductFiat(nextTierInfo.cost)) {
                        coolingManager.UpgradeTier(nextCoolingTier);
                        thermalModel.SetCoolingPowerWatts(coolingManager.CalculateTotalCoolingWatts());
                    }
                }
            } else {
                btnUpgradeCooling.SetTitle(isTR ? "DALDIRMA SIVI SOGUTMA AKTIF" : "IMMERSION COOLING ACTIVE");
                btnUpgradeCooling.SetSubtitle(isTR ? "Maksimum Sogutma (6,000W) - Kartlar Asla Yanmaz" : "Max Cooling (6,000W) - Overheat Immune");
                btnUpgradeCooling.SetDisabled(true);
            }

            // 5. Overclock
            btnOverclock.SetTitle(Core::LocalizationManager::Tr("BTN_OVERCLOCK"));
            btnOverclock.SetSubtitle(Core::LocalizationManager::Tr("BTN_OVERCLOCK_SUB"));
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
            btnUndervolt.SetTitle(Core::LocalizationManager::Tr("BTN_UNDERVOLT"));
            btnUndervolt.SetSubtitle(Core::LocalizationManager::Tr("BTN_UNDERVOLT_SUB"));
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
            btnThermalToggle.SetTitle(Core::LocalizationManager::Tr("BTN_THERMAL"));
            btnThermalToggle.SetSubtitle(shaderManager.IsThermalActive() ? Core::LocalizationManager::Tr("BTN_THERMAL_ON") : Core::LocalizationManager::Tr("BTN_THERMAL_OFF"));
            if (btnThermalToggle.UpdateAndCheckClick() || IsKeyPressed(KEY_TAB)) {
                shaderManager.ToggleThermal();
            }

            // 8. Sigorta Şalteri
            btnResetBreaker.SetTitle(Core::LocalizationManager::Tr("BTN_BREAKER_RESET"));
            btnResetBreaker.SetDisabled(!powerGrid.IsBreakerTripped());
            btnResetBreaker.SetSubtitle(powerGrid.IsBreakerTripped() ? Core::LocalizationManager::Tr("BTN_BREAKER_TRIPPED") : Core::LocalizationManager::Tr("BTN_BREAKER_OK"));
            if (btnResetBreaker.UpdateAndCheckClick() || (powerGrid.IsBreakerTripped() && IsKeyPressed(KEY_R))) {
                powerGrid.ResetBreaker();
            }
        }

        // --- SİMÜLASYON MOTORU HESAPLAMALARI VE YANMA/HASAR KONTROLÜ ---
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
                    bool rPowered = r->IsPoweredOn();
                    for (const auto& gpu : r->GetGPUs()) {
                        if (gpu) {
                            double cardTemp = rPowered ? thermalModel.CalculateGPUTemperature(gpu->GetEffectivePowerWatts(), gpu->GetFanSpeedPercent() / 100.0)
                                                       : thermalModel.GetAmbientTemperature();
                            if (cardTemp > maxCardTemp) maxCardTemp = cardTemp;

                            if (rPowered) {
                                // 85°C üzeri: Thermal Throttling
                                gpu->SetThrottled(Core::ThermalModel::IsOverheating(cardTemp));

                                // 105°C üzeri: Kart sağlığı erir (Damage)
                                if (cardTemp >= 105.0 && !coolingManager.IsImmersionCoolingActive()) {
                                    gpu->TakeDamage(dt * 5.0);
                                }

                                // 140°C üzeri: KART AŞIRI SICAKLIKTAN YANAR (BURNT)!
                                if (cardTemp >= 140.0 && !coolingManager.IsImmersionCoolingActive()) {
                                    if (marketCatalog.HasAutoFireSuppression()) {
                                        r->SetPoweredOn(false); // Otomatik yangın söndürücü sistemi rig'i güvenle kapatır
                                    } else {
                                        gpu->SetBurnt(true);
                                    }
                                }
                            } else {
                                gpu->SetThrottled(false);
                            }
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

        // Arka Plan
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

        std::ostringstream ssCrypto, ssHash, ssTemp;
        ssCrypto << std::fixed << std::setprecision(4) << economy.GetCryptoBalance() << " " << economy.GetCoinSymbol();
        ssHash << std::fixed << std::setprecision(1) << warehouse.CalculateTotalHashrate() << " MH/s";
        ssTemp << std::fixed << std::setprecision(1) << thermalModel.GetAmbientTemperature() << " C";

        Render::UIFrame::DrawStatBadge(pad + (0 * (badgeW + badgeGap)), badgeY, badgeW, badgeH, currentAvatar, Core::LocalizationManager::Tr("BADGE_COMPANY"), userProfile.GetCompanyName(), Color{0, 230, 255, 255});
        Render::UIFrame::DrawStatBadge(pad + (1 * (badgeW + badgeGap)), badgeY, badgeW, badgeH, "[CASH]", Core::LocalizationManager::Tr("BADGE_CASH"), economy.FormatFiat(economy.GetFiatBalance()), Color{0, 255, 150, 255});
        Render::UIFrame::DrawStatBadge(pad + (2 * (badgeW + badgeGap)), badgeY, badgeW, badgeH, "[WALLET]", Core::LocalizationManager::Tr("BADGE_CRYPTO"), ssCrypto.str(), Color{255, 210, 50, 255});
        Render::UIFrame::DrawStatBadge(pad + (3 * (badgeW + badgeGap)), badgeY, badgeW, badgeH, "[MARKET]", std::string("1 ") + economy.GetCoinSymbol(), economy.FormatPrice(economy.GetCryptoPrice()), Color{170, 200, 255, 255});
        Render::UIFrame::DrawStatBadge(pad + (4 * (badgeW + badgeGap)), badgeY, badgeW, badgeH, "[SPEED]", Core::LocalizationManager::Tr("BADGE_SPEED"), ssHash.str(), Color{100, 230, 255, 255});

        btnOpenSettings.Draw();

        // 2. SOL PANEL: VIEWPORT ÇERÇEVESİ (SEÇİLİ RİG DETAYI VEYA DEPO KUŞBAKIŞI)
        const Rectangle viewportRect{pad, contentY, leftW, contentH};
        std::string viewportTitle = (currentViewMode == WarehouseViewMode::RIG_DETAIL)
            ? (warehouse.GetFacilityName() + " (" + std::to_string(warehouse.GetActiveRigIndex() + 1) + "/" + std::to_string(warehouse.GetRigCount()) + " RIG) [KARTA TIKLA: 360 INCELE]")
            : ("DEPO GENEL BAKIS - " + warehouse.GetFacilityName() + " (" + std::to_string(warehouse.GetRigCount()) + " RIG)");
        Render::UIFrame::DrawCard(viewportRect, viewportTitle, Color{0, 220, 255, 255});

        // Sekme Butonlarını Çiz
        btnTabRigDetail.Draw();
        btnTabOverview.Draw();

        if (currentViewMode == WarehouseViewMode::RIG_DETAIL) {
            btnToggleRigPower.Draw();
            btnSellRig.Draw();
            btnPrevRig.Draw();
            btnNextRig.Draw();

            if (activeRig) {
                shaderManager.BeginShader();
                rigRenderer.DrawRig(*activeRig, thermalModel, static_cast<int>(rigX), static_cast<int>(rigY), animTime, &textureManager);
                shaderManager.EndShader();
            }

            // Güç Gösterge Barı
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
        } else {
            // Kuşbakışı Genel Bakış Görünümü
            Vector2 mouse = GetMousePosition();
            int selectedRigIdx = -1;
            int toggledRigIdx = -1;
            rigRenderer.DrawWarehouseOverviewGrid(warehouse, thermalModel,
                                                 Rectangle{pad + 10.0f, contentY + 50.0f, leftW - 20.0f, contentH - 60.0f},
                                                 animTime, mouse, selectedRigIdx, toggledRigIdx);
            if (toggledRigIdx >= 0) {
                if (auto* r = warehouse.GetRig(static_cast<size_t>(toggledRigIdx))) {
                    r->TogglePower();
                }
            }
            if (selectedRigIdx >= 0) {
                warehouse.SetActiveRigIndex(static_cast<size_t>(selectedRigIdx));
                currentViewMode = WarehouseViewMode::RIG_DETAIL;
            }
        }

        // 3. SAĞ PANEL: KONTROL VE YÜKSELTME MERKEZİ
        const Rectangle managementRect{rightX, contentY, rightW, contentH};
        Render::UIFrame::DrawCard(managementRect, "DEPO VE OPERASYON MERKEZI", Color{255, 200, 0, 255});

        btnBuyGPU.Draw();
        btnBuyRig.Draw();
        btnSellCrypto.Draw();
        btnUpgradeCooling.Draw();
        btnOverclock.Draw();
        btnUndervolt.Draw();
        btnThermalToggle.Draw();
        btnResetBreaker.Draw();

        // 4. ALT BİLGİ VE KONTROL ŞERİDİ
        DrawRectangle(0, static_cast<int>(screenH - footerH), static_cast<int>(screenW), static_cast<int>(footerH), Color{14, 17, 23, 250});
        DrawLine(0, static_cast<int>(screenH - footerH), static_cast<int>(screenW), static_cast<int>(screenH - footerH), Color{35, 42, 56, 255});

        Render::UIFrame::DrawTextCustom(Core::LocalizationManager::Tr("TIP_FOOTER"),
                                       pad + 10.0f, screenH - footerH + 12.0f, 14.0f, Color{150, 165, 190, 255}, false);

        std::string verTag = "GameOfTex v1.6 [Hardware Market & Facility Store]";
        float verW = Render::UIFrame::MeasureTextCustom(verTag, 14.0f, false);
        Render::UIFrame::DrawTextCustom(verTag, screenW - verW - pad - 10.0f, screenH - footerH + 12.0f, 14.0f, Color{100, 120, 150, 255}, false);

        // 5. GPU 360 İNCELEME MODALI (AÇIKSA EN ÜSTTE ÇİZİLİR)
        if (gpuInspectionModal.IsOpen()) {
            gpuInspectionModal.Draw(animTime, &textureManager);
        }

        // 6. AYARLAR MODAL PENCERESİ
        if (settingsModal.IsOpen()) {
            settingsModal.Draw(economy);
        }

        // 7. DONANIM VE TESİS MARKETİ MODAL PENCERESİ
        if (marketModal.IsOpen()) {
            marketModal.Draw(economy, warehouse, coolingManager, marketCatalog, powerGrid);
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
