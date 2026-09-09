#ifdef _WIN32
extern "C" __declspec(dllimport) int __stdcall SetProcessDPIAware(void);
#endif

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
#include "core/TaskManager.hpp"
#include "render/TaskModal.hpp"
#include "core/FacilityManager.hpp"
#include "render/WorldMapModal.hpp"
#include "core/SaveManager.hpp"
#include "render/MainMenuScreen.hpp"
#include "render/CryptoExchangeModal.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
#include <algorithm>

enum class GameState {
    MAIN_MENU,
    LOGIN,
    GAMEPLAY
};

int main() {
#ifdef _WIN32
    // Windows DWM bitmap ölçekleme bulanıklığını tamamen devre dışı bırak
    SetProcessDPIAware();
#endif

    // 1. Pencere Yapılandırması (Resize ve F11 Tam Ekran desteği)
    constexpr int initialWidth = 1280;
    constexpr int initialHeight = 720;
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
    InitWindow(initialWidth, initialHeight, "GameOfTex - Crypto Mining & Energy Tycoon (Warehouse Edition)");
    SetWindowMinSize(1024, 600);
    SetTargetFPS(60);
    SetExitKey(KEY_NULL); // ESC tuşunun oyunu aniden kapatmasını engelle

    // 2. Yüksek Çözünürlüklü Vektör Fontlarını Yükle (Türkçe Genişletilmiş Kod Noktaları ile)
    std::vector<int> codepoints;
    // ASCII Basic (32 - 126)
    for (int i = 32; i <= 126; ++i) codepoints.push_back(i);
    // Latin-1 Supplement (160 - 255): ç, Ç, ö, Ö, ü, Ü, °, vb.
    for (int i = 160; i <= 255; ++i) codepoints.push_back(i);
    // Latin Extended-A (0x0100 - 0x017F): ğ, Ğ, ı, İ, ş, Ş, vb.
    for (int i = 0x0100; i <= 0x017F; ++i) codepoints.push_back(i);

    // Öncelik 1: Segoe UI (Pürüzsüz, modern yuvarlak hatlar, sıfır pikselleşme ve kusursuz Türkçe karakter desteği)
    Font fontRegular = LoadFontEx("assets/fonts/SegoeUI-Regular.ttf", 32, codepoints.data(), static_cast<int>(codepoints.size()));
    Font fontBold = LoadFontEx("assets/fonts/SegoeUI-Bold.ttf", 36, codepoints.data(), static_cast<int>(codepoints.size()));

    // Yedek 1: Windows Sistem Segoe UI Fontu
    if (fontRegular.texture.id == 0) {
        fontRegular = LoadFontEx("C:/Windows/Fonts/segoeui.ttf", 32, codepoints.data(), static_cast<int>(codepoints.size()));
        fontBold = LoadFontEx("C:/Windows/Fonts/segoeuib.ttf", 36, codepoints.data(), static_cast<int>(codepoints.size()));
    }

    // Yedek 2: Google Inter Fontu
    if (fontRegular.texture.id == 0) {
        fontRegular = LoadFontEx("assets/fonts/Inter.ttf", 32, codepoints.data(), static_cast<int>(codepoints.size()));
        fontBold = LoadFontEx("assets/fonts/Inter.ttf", 36, codepoints.data(), static_cast<int>(codepoints.size()));
    }

    // Yedek 3: Windows Arial
    if (fontRegular.texture.id == 0) {
        fontRegular = LoadFontEx("C:/Windows/Fonts/arial.ttf", 32, codepoints.data(), static_cast<int>(codepoints.size()));
        fontBold = LoadFontEx("C:/Windows/Fonts/arialbd.ttf", 36, codepoints.data(), static_cast<int>(codepoints.size()));
    }

    // Yedek 4: Rajdhani
    if (fontRegular.texture.id == 0) {
        fontRegular = LoadFontEx("assets/fonts/Rajdhani-Medium.ttf", 32, codepoints.data(), static_cast<int>(codepoints.size()));
        fontBold = LoadFontEx("assets/fonts/Rajdhani-Bold.ttf", 36, codepoints.data(), static_cast<int>(codepoints.size()));
    }

    if (fontRegular.texture.id > 0) {
        GenTextureMipmaps(&fontRegular.texture);
        GenTextureMipmaps(&fontBold.texture);
        SetTextureFilter(fontRegular.texture, TEXTURE_FILTER_TRILINEAR);
        SetTextureFilter(fontBold.texture, TEXTURE_FILTER_TRILINEAR);
        Render::UIFrame::InitTheme(fontRegular, fontBold);
    }

    GameState currentState = GameState::MAIN_MENU;
    enum class WarehouseViewMode {
        RIG_DETAIL,
        OVERVIEW_GRID
    };
    WarehouseViewMode currentViewMode = WarehouseViewMode::RIG_DETAIL;
    float overviewScrollY = 0.0f;
    float maxOverviewScrollY = 0.0f;

    // 3. Profil, Giriş, Ana Menü ve Modal Pencereleri
    Core::UserProfile userProfile;
    Render::MainMenuScreen mainMenuScreen;
    Render::LoginScreen loginScreen;
    Render::SettingsModal settingsModal;
    Render::GPUInspectionModal gpuInspectionModal;
    Core::MarketCatalog marketCatalog;
    Render::MarketModal marketModal;
    Core::TaskManager taskManager;
    Render::TaskModal taskModal;
    Core::FacilityManager facilityManager;
    Render::WorldMapModal worldMapModal;
    Render::CryptoExchangeModal cryptoExchangeModal;

    // 4. Çekirdek Simülasyon Nesneleri (Clean Code / SRP)
    Core::EconomyManager economy(1500.0, 2.40, "TEX");

    // Kayıt Bildirimi ve Otomatik Kayıt Durumu
    float saveToastTimer = 0.0f;
    std::string saveToastMessage = "";
    float autoSaveTimer = 0.0f;
    constexpr float AUTO_SAVE_INTERVAL = 45.0f;

    auto triggerSave = [&](const std::string& customMsg = "") {
        bool isTR = (Core::LocalizationManager::Get().GetLanguage() == Core::Language::TURKISH);
        if (Core::SaveManager::SaveGame(Core::SaveManager::DEFAULT_SAVE_PATH,
                                        userProfile, economy, facilityManager, taskManager, marketCatalog)) {
            saveToastMessage = customMsg.empty() ? (isTR ? "[OK] OYUN BASARIYLA KAYDEDILDI" : "[OK] GAME SAVED SUCCESSFULLY") : customMsg;
            saveToastTimer = 2.5f;
            mainMenuScreen.RefreshSaveState();
        }
    };

    auto triggerSaveToast = [&](const std::string& msg) {
        saveToastMessage = msg;
        saveToastTimer = 2.5f;
    };

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
    Render::UIButton btnQuickSave(Rectangle{}, "KAYDET", "F5", Color{20, 50, 36, 255}, Color{0, 255, 140, 255});
    Render::UIButton btnOpenTasks(Rectangle{}, "GOREVLER", "", Color{30, 40, 58, 255}, Color{255, 200, 40, 255});
    Render::UIButton btnOpenWorldMap(Rectangle{}, "HARITA", "", Color{25, 45, 65, 255}, Color{0, 220, 255, 255});
    Render::UIButton btnUpgradePSU(Rectangle{}, "PSU YUKSELT", "", Color{40, 32, 58, 255}, Color{200, 100, 255, 255});

    // 7. Ana Oyun Döngüsü
    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();
        animTime += dt;

        auto* activeFacility = facilityManager.GetActiveFacility();
        auto& warehouse = *activeFacility->warehouse;
        auto& thermalModel = *activeFacility->thermalModel;
        auto& powerGrid = *activeFacility->powerGrid;
        auto& coolingManager = *activeFacility->coolingManager;

        auto toggleFullscreenNative = [&]() {
            int monitor = GetCurrentMonitor();
            if (IsWindowFullscreen()) {
                ToggleFullscreen();
                SetWindowSize(initialWidth, initialHeight);
                int monW = GetMonitorWidth(monitor);
                int monH = GetMonitorHeight(monitor);
                SetWindowPosition((monW - initialWidth) / 2, (monH - initialHeight) / 2);
            } else {
                int monW = GetMonitorWidth(monitor);
                int monH = GetMonitorHeight(monitor);
                SetWindowSize(monW, monH);
                ToggleFullscreen();
            }
        };

        // F11: Tam Ekran Geçişi (Native Çözünürlük Geçişi - Sıfır Bulanıklık)
        if (IsKeyPressed(KEY_F11)) {
            toggleFullscreenNative();
        }

        // ESC Tuşu: Modal açıksa kapat, değilse tam ekrandan küçük pencereli moda dön!
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (cryptoExchangeModal.IsOpen()) {
                cryptoExchangeModal.Close();
            } else if (worldMapModal.IsOpen()) {
                worldMapModal.Close();
            } else if (taskModal.IsOpen()) {
                taskModal.Close();
            } else if (marketModal.IsOpen()) {
                marketModal.Close();
            } else if (gpuInspectionModal.IsOpen()) {
                gpuInspectionModal.Close();
            } else if (settingsModal.IsOpen()) {
                settingsModal.Close();
            } else if (IsWindowFullscreen()) {
                toggleFullscreenNative();
            }
        }

        const float screenW = static_cast<float>(GetScreenWidth());
        const float screenH = static_cast<float>(GetScreenHeight());

        // --- 1. ANA MENÜ / GİRİŞ EKRANI DURUMU ---
        if (currentState == GameState::MAIN_MENU) {
            if (settingsModal.IsOpen()) {
                auto sAct = settingsModal.Update(economy);
                if (sAct == Render::SettingsAction::SAVE_GAME) {
                    triggerSave();
                }
            } else {
                auto action = mainMenuScreen.Update(dt);
                if (action == Render::MainMenuAction::CONTINUE_GAME) {
                    bool ok = Core::SaveManager::LoadGame(Core::SaveManager::DEFAULT_SAVE_PATH,
                                                          userProfile, economy, facilityManager, taskManager, marketCatalog);
                    if (ok) {
                        bool isTR = (Core::LocalizationManager::Get().GetLanguage() == Core::Language::TURKISH);
                        triggerSaveToast(isTR ? "[OK] OYUN BASARIYLA YUKLENDI" : "[OK] GAME LOADED SUCCESSFULLY");
                        currentState = GameState::GAMEPLAY;
                    }
                } else if (action == Render::MainMenuAction::START_NEW_GAME) {
                    currentState = GameState::LOGIN;
                } else if (action == Render::MainMenuAction::OPEN_SETTINGS) {
                    settingsModal.Open();
                } else if (action == Render::MainMenuAction::QUIT_GAME) {
                    break;
                }
            }

            BeginDrawing();
            mainMenuScreen.Draw();
            if (settingsModal.IsOpen()) {
                settingsModal.Draw(economy);
            }
            EndDrawing();
            continue;
        }

        // --- 2. PROFİL VE HESAP OLUŞTURMA EKRANI DURUMU ---
        if (currentState == GameState::LOGIN) {
            auto lAct = loginScreen.Update(userProfile);
            if (lAct == Render::LoginAction::SUBMIT) {
                if (!userProfile.HasClaimedBonus()) {
                    economy.AddFiat(1000.0);
                    userProfile.SetClaimedBonus(true);
                }
                triggerSave();
                bool isTR = (Core::LocalizationManager::Get().GetLanguage() == Core::Language::TURKISH);
                triggerSaveToast(isTR ? "[OK] YENI SIRKET BASLATILDI" : "[OK] NEW COMPANY INITIALIZED");
                currentState = GameState::GAMEPLAY;
            } else if (lAct == Render::LoginAction::CANCEL) {
                currentState = GameState::MAIN_MENU;
                mainMenuScreen.RefreshSaveState();
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
            gpuInspectionModal.Update(economy.GetFiatBalance(), &thermalModel, repairCost, scrapRequested);
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
                        taskManager.NotifyGpuPurchased();
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

        // Görevler Modalı Açıksa Güncelle
        if (taskModal.IsOpen()) {
            taskModal.Update(taskManager, economy);
        }

        // Dünya Haritası Modalı Açıksa Güncelle
        if (worldMapModal.IsOpen()) {
            worldMapModal.Update(facilityManager, economy);
        }

        // Kripto Borsası ve Grafik Modalı Açıksa Güncelle
        if (cryptoExchangeModal.IsOpen()) {
            cryptoExchangeModal.Update(economy, taskManager);
        }

        // Ayarlar Modalı Açıksa Güncelle
        if (settingsModal.IsOpen()) {
            auto sAct = settingsModal.Update(economy);
            if (sAct == Render::SettingsAction::SAVE_GAME) {
                triggerSave();
            } else if (sAct == Render::SettingsAction::RETURN_TO_MAIN_MENU) {
                bool isTR = (Core::LocalizationManager::Get().GetLanguage() == Core::Language::TURKISH);
                triggerSave(isTR ? "[OK] OYUN KAYDEDILDI - MENÜYE DONULDU" : "[OK] GAME SAVED - RETURNED TO MENU");
                currentState = GameState::MAIN_MENU;
                mainMenuScreen.RefreshSaveState();
            }
        }

        // --- DİNAMİK RESPONSIVE DÜZEN HESAPLAMALARI ---
        const float pad = 16.0f;
        const float headerH = 76.0f;
        const float footerH = 42.0f;

        // Üst Rozetlerin Dinamik Genişliği
        const float badgeGap = 6.0f;
        const float settingsBtnW = 86.0f;
        const float saveBtnW = 82.0f;
        const float taskBtnW = 118.0f;
        const float worldMapBtnW = 110.0f;
        const float totalBtnsW = settingsBtnW + saveBtnW + taskBtnW + worldMapBtnW + (badgeGap * 3.0f);
        const float availBadgeSpace = screenW - (pad * 2.0f) - totalBtnsW - (badgeGap * 5.0f);
        const float badgeW = std::clamp(availBadgeSpace / 5.0f, 80.0f, 215.0f);
        const float badgeH = 56.0f;
        const float badgeY = (headerH - badgeH) / 2.0f;

        btnOpenSettings.SetBounds(Rectangle{screenW - pad - settingsBtnW, badgeY, settingsBtnW, badgeH});
        btnQuickSave.SetBounds(Rectangle{screenW - pad - settingsBtnW - badgeGap - saveBtnW, badgeY, saveBtnW, badgeH});
        btnOpenTasks.SetBounds(Rectangle{screenW - pad - settingsBtnW - badgeGap - saveBtnW - badgeGap - taskBtnW, badgeY, taskBtnW, badgeH});
        btnOpenWorldMap.SetBounds(Rectangle{screenW - pad - settingsBtnW - badgeGap - saveBtnW - badgeGap - taskBtnW - badgeGap - worldMapBtnW, badgeY, worldMapBtnW, badgeH});
        btnOpenWorldMap.SetTitle(std::string("[M] ") + Core::LocalizationManager::Tr("BTN_WORLD_MAP"));
        btnQuickSave.SetTitle(std::string("[S] ") + Core::LocalizationManager::Tr("BTN_SAVE"));
        btnQuickSave.SetSubtitle("F5");

        size_t unclaimedCount = taskManager.GetUnclaimedCompletedCount();
        if (unclaimedCount > 0) {
            btnOpenTasks.SetTitle(std::string("[*] ") + Core::LocalizationManager::Tr("BTN_TASKS") + " (" + std::to_string(unclaimedCount) + ")");
            btnOpenTasks.SetAccentColor(Color{255, 215, 0, 255});
        } else {
            btnOpenTasks.SetTitle(std::string("[+] ") + Core::LocalizationManager::Tr("BTN_TASKS"));
            btnOpenTasks.SetAccentColor(Color{60, 160, 240, 255});
        }

        if (!settingsModal.IsOpen() && !gpuInspectionModal.IsOpen() && !marketModal.IsOpen() && !taskModal.IsOpen() && !worldMapModal.IsOpen()) {
            if (btnOpenSettings.UpdateAndCheckClick()) {
                settingsModal.Open();
            }
            if (btnQuickSave.UpdateAndCheckClick() || IsKeyPressed(KEY_F5) || ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_S))) {
                triggerSave();
            }
            if (btnOpenTasks.UpdateAndCheckClick()) {
                taskModal.Open();
            }
            if (btnOpenWorldMap.UpdateAndCheckClick()) {
                worldMapModal.Open();
            }
        }

        // Ana İçerik Alanı
        const float contentY = headerH + 12.0f;
        const float contentH = screenH - contentY - footerH - 12.0f;

        // Sol Viewport ve Sağ Kontrol Paneli Genişlikleri
        const float leftW = (screenW - (pad * 2.0f) - 14.0f) * 0.66f;
        const float rightW = (screenW - (pad * 2.0f) - 14.0f) * 0.34f;
        const float rightX = pad + leftW + 14.0f;

        bool isTR = (Core::LocalizationManager::Get().GetLanguage() == Core::Language::TURKISH);

        // Sekme Butonları (Rig Detayı vs Depo Kuşbakışı Genel Bakış - Başlığın sağında yer alır)
        btnTabRigDetail.SetTitle(Core::LocalizationManager::Tr("TAB_RIG_DETAIL"));
        btnTabOverview.SetTitle(Core::LocalizationManager::Tr("TAB_OVERVIEW"));
        btnTabRigDetail.SetBounds(Rectangle{pad + leftW - 246.0f, contentY + 8.0f, 116.0f, 28.0f});
        btnTabOverview.SetBounds(Rectangle{pad + leftW - 124.0f, contentY + 8.0f, 116.0f, 28.0f});

        if (!settingsModal.IsOpen() && !gpuInspectionModal.IsOpen() && !marketModal.IsOpen() && !taskModal.IsOpen() && !worldMapModal.IsOpen()) {
            if (btnTabRigDetail.UpdateAndCheckClick()) currentViewMode = WarehouseViewMode::RIG_DETAIL;
            if (btnTabOverview.UpdateAndCheckClick()) currentViewMode = WarehouseViewMode::OVERVIEW_GRID;

            // Depo Genel Bakış Ekranında Fare Tekerleği ile Akıcı Aşağı/Yukarı Kaydırma
            if (currentViewMode == WarehouseViewMode::OVERVIEW_GRID) {
                Vector2 mouse = GetMousePosition();
                Rectangle overviewArea{pad, contentY + 46.0f, leftW, contentH - 54.0f};
                if (CheckCollisionPointRec(mouse, overviewArea)) {
                    float wheel = GetMouseWheelMove();
                    if (wheel != 0.0f) {
                        overviewScrollY -= wheel * 60.0f;
                        overviewScrollY = std::clamp(overviewScrollY, 0.0f, maxOverviewScrollY);
                    }
                }
            }
        }

        // Rig Gezinme ve Yönetim Butonları (Yalnızca RIG_DETAIL modunda, 2. satırda konumlandırılır, asla başlık veya sekmelerle çakışmaz)
        if (currentViewMode == WarehouseViewMode::RIG_DETAIL) {
            btnPrevRig.SetBounds(Rectangle{pad + 16.0f, contentY + 45.0f, 85.0f, 32.0f});
            btnNextRig.SetBounds(Rectangle{pad + 107.0f, contentY + 45.0f, 85.0f, 32.0f});
            btnToggleRigPower.SetBounds(Rectangle{pad + leftW - 224.0f, contentY + 45.0f, 112.0f, 32.0f});
            btnSellRig.SetBounds(Rectangle{pad + leftW - 106.0f, contentY + 45.0f, 94.0f, 32.0f});

            btnPrevRig.SetTitle(Core::LocalizationManager::Tr("RIG_PREV"));
            btnNextRig.SetTitle(Core::LocalizationManager::Tr("RIG_NEXT"));

            if (activeRig) {
                btnToggleRigPower.SetTitle(activeRig->IsPoweredOn() ? Core::LocalizationManager::Tr("RIG_POWER_OFF") : Core::LocalizationManager::Tr("RIG_POWER_ON"));
                btnToggleRigPower.SetSubtitle(activeRig->IsPoweredOn() ? Core::LocalizationManager::Tr("RIG_POWER_OFF_SUB") : Core::LocalizationManager::Tr("RIG_POWER_ON_SUB"));
            }
            btnSellRig.SetTitle(Core::LocalizationManager::Tr("RIG_SELL"));
            btnSellRig.SetSubtitle(std::string("+") + economy.FormatFiat(1200.0) + (isTR ? " Hurda" : " Scrap"));
            btnSellRig.SetDisabled(warehouse.GetRigCount() <= 1);

            if (!settingsModal.IsOpen() && !gpuInspectionModal.IsOpen() && !marketModal.IsOpen() && !taskModal.IsOpen() && !worldMapModal.IsOpen()) {
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
        }

        // Sağ Paneldeki Butonların Dinamik Konumları (9 Buton)
        const float btnW = rightW - 32.0f;
        const float btnH = std::clamp((contentH - 95.0f) / 9.2f, 44.0f, 58.0f);
        const float btnGap = 5.0f;
        const float startBtnY = contentY + 46.0f;
        const float btnX = rightX + 16.0f;

        btnBuyGPU.SetBounds(Rectangle{btnX, startBtnY + (0 * (btnH + btnGap)), btnW, btnH});
        btnBuyRig.SetBounds(Rectangle{btnX, startBtnY + (1 * (btnH + btnGap)), btnW, btnH});
        btnUpgradePSU.SetBounds(Rectangle{btnX, startBtnY + (2 * (btnH + btnGap)), btnW, btnH});
        btnSellCrypto.SetBounds(Rectangle{btnX, startBtnY + (3 * (btnH + btnGap)), btnW, btnH});
        btnUpgradeCooling.SetBounds(Rectangle{btnX, startBtnY + (4 * (btnH + btnGap)), btnW, btnH});
        btnOverclock.SetBounds(Rectangle{btnX, startBtnY + (5 * (btnH + btnGap)), btnW, btnH});
        btnUndervolt.SetBounds(Rectangle{btnX, startBtnY + (6 * (btnH + btnGap)), btnW, btnH});
        btnThermalToggle.SetBounds(Rectangle{btnX, startBtnY + (7 * (btnH + btnGap)), btnW, btnH});
        btnResetBreaker.SetBounds(Rectangle{btnX, startBtnY + (8 * (btnH + btnGap)), btnW, btnH});

        // Kart Tıklama Tespiti (Viewport içerisindeki GPU'ya tıklandı mı?)
        constexpr float rigBaseW = 720.0f;
        const float rigX = pad + (leftW - rigBaseW) / 2.0f;
        const float rigY = contentY + 84.0f;

        if (!settingsModal.IsOpen() && !gpuInspectionModal.IsOpen() && !marketModal.IsOpen() && !taskModal.IsOpen() && !worldMapModal.IsOpen() && !cryptoExchangeModal.IsOpen() && activeRig && currentViewMode == WarehouseViewMode::RIG_DETAIL) {
            Vector2 mouse = GetMousePosition();
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                int clickedIndex = rigRenderer.GetClickedGPUIndex(static_cast<int>(rigX), static_cast<int>(rigY), activeRig->GetGPUCount(), mouse);
                if (clickedIndex >= 0 && clickedIndex < static_cast<int>(activeRig->GetGPUCount())) {
                    auto* targetCard = activeRig->GetGPU(static_cast<size_t>(clickedIndex));
                    double cardTemp = activeRig->IsPoweredOn() ? thermalModel.CalculateGPUTemperature(targetCard->GetEffectivePowerWatts(), 0.85) : thermalModel.GetAmbientTemperature();
                    gpuInspectionModal.Open(targetCard, cardTemp, static_cast<size_t>(clickedIndex));
                    taskManager.NotifyCardInspected();
                }
            }
        }

        if (!settingsModal.IsOpen() && !gpuInspectionModal.IsOpen() && !marketModal.IsOpen() && !taskModal.IsOpen() && !worldMapModal.IsOpen() && !cryptoExchangeModal.IsOpen()) {
            // [B] veya [E] kısayolu ile Borsayı aç
            if (IsKeyPressed(KEY_B) || IsKeyPressed(KEY_E)) {
                cryptoExchangeModal.Open();
            }

            // 1. Donanım ve Tesis Marketi
            btnBuyGPU.SetTitle(isTR ? "[MARKET] DONANIM VE TESIS" : "[STORE] HARDWARE & SITES");
            std::string gpuSub = isTR ? "Farkli Modeller, Trafo ve Tesis" : "Different Models, Power & Facilities";
            btnBuyGPU.SetSubtitle(gpuSub);
            btnBuyGPU.SetDisabled(false);
            if (btnBuyGPU.UpdateAndCheckClick()) {
                marketModal.Open();
            }

            // 2. Yeni Rig Satın Al ($2,500)
            btnBuyRig.SetTitle(Core::LocalizationManager::Tr("BTN_BUY_RIG"));
            bool isRigCapFull = (warehouse.GetRigCount() >= warehouse.GetMaxRigCapacity());
            btnBuyRig.SetDisabled(economy.GetFiatBalance() < 2500.0 || isRigCapFull);
            std::string rigSub;
            if (isRigCapFull) {
                rigSub = isTR ? ("KAPASITE DOLU (" + std::to_string(warehouse.GetRigCount()) + "/" + std::to_string(warehouse.GetMaxRigCapacity()) + ") - HARITADAN YENI DEPO ALIN")
                              : ("CAPACITY FULL (" + std::to_string(warehouse.GetRigCount()) + "/" + std::to_string(warehouse.GetMaxRigCapacity()) + ") - BUY NEW SITE");
            } else {
                rigSub = (isTR ? "Maliyet: " : "Cost: ") + economy.FormatFiat(2500.0) + (isTR ? " | Kapasite: " : " | Capacity: ") + std::to_string(warehouse.GetRigCount()) + "/" + std::to_string(warehouse.GetMaxRigCapacity());
            }
            btnBuyRig.SetSubtitle(rigSub);
            if (btnBuyRig.UpdateAndCheckClick() && !isRigCapFull) {
                if (economy.DeductFiat(2500.0)) {
                    std::string newRigName = std::string("Rig ") + (warehouse.GetRigCount() < 9 ? "0" : "") + std::to_string(warehouse.GetRigCount() + 1) + " - Frame";
                    warehouse.AddNewRig(newRigName, 6);
                    warehouse.SetActiveRigIndex(warehouse.GetRigCount() - 1);
                }
            }

            // 3. Rig İçi PSU Yükseltme
            if (activeRig) {
                if (activeRig->CanUpgradePSU()) {
                    btnUpgradePSU.SetTitle(Core::LocalizationManager::Tr("BTN_UPGRADE_PSU"));
                    double psuCost = activeRig->GetNextPSUCost();
                    std::string psuSub = activeRig->GetNextPSUName() + " (" + economy.FormatFiat(psuCost) + ")";
                    btnUpgradePSU.SetSubtitle(psuSub);
                    btnUpgradePSU.SetDisabled(economy.GetFiatBalance() < psuCost);
                    if (btnUpgradePSU.UpdateAndCheckClick()) {
                        if (economy.DeductFiat(psuCost)) {
                            activeRig->UpgradePSU();
                        }
                    }
                } else {
                    btnUpgradePSU.SetTitle(Core::LocalizationManager::Tr("PSU_MAX_REACHED"));
                    btnUpgradePSU.SetSubtitle("2600W Server Dual PSU");
                    btnUpgradePSU.SetDisabled(true);
                }
            }

            // 4. Kripto Borsa & Canlı Grafik Masası
            btnSellCrypto.SetTitle(isTR ? "[BORSA] KRIPTO AL / SAT" : "[EXCHANGE] TRADE & CHARTS");
            const double totalPortfolioUSD = economy.GetTotalPortfolioValueUSD();
            std::string cryptoSub = (isTR ? "Portfoy: " : "Portfolio: ") + economy.FormatFiat(totalPortfolioUSD) + (isTR ? " (5 Coin)" : " (5 Coins)");
            btnSellCrypto.SetSubtitle(cryptoSub);
            btnSellCrypto.SetDisabled(false);
            if (btnSellCrypto.UpdateAndCheckClick()) {
                cryptoExchangeModal.Open();
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
                taskManager.NotifyCardOverclocked();
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

        // --- SİMÜLASYON MOTORU: TÜM SATIN ALINAN TESİSLER EŞZAMANLI ÇALIŞIR ---
        double activeMaxCardTemp = 0.0;
        double companyTotalMinedHashrate = 0.0;

        for (size_t fIdx = 0; fIdx < facilityManager.GetFacilityCount(); ++fIdx) {
            auto* fac = facilityManager.GetFacility(fIdx);
            if (!fac || !fac->isPurchased || !fac->warehouse || !fac->powerGrid || !fac->thermalModel || !fac->coolingManager) continue;

            auto& fWarehouse = *fac->warehouse;
            auto& fPowerGrid = *fac->powerGrid;
            auto& fThermal = *fac->thermalModel;
            auto& fCooling = *fac->coolingManager;
            bool isActiveFacility = (fIdx == facilityManager.GetActiveFacilityIndex());

            fPowerGrid.Update(dt);
            fPowerGrid.ResetStep();

            for (const auto& r : fWarehouse.GetAllRigs()) {
                if (r) r->Update(dt);
            }

            if (!fPowerGrid.IsBreakerTripped()) {
                const double rawPower = fWarehouse.CalculateTotalPowerWatts();
                const double warehousePower = rawPower * fPowerGrid.GetPowerSurgeMultiplier();
                fPowerGrid.AddConsumerWatts(warehousePower);

                const double electricityCost = fPowerGrid.CalculateCostForDuration(dt);
                economy.DeductFiat(electricityCost);

                const double thermalLoad = warehousePower * (fPowerGrid.IsGridStrained() ? 1.15 : 1.0);
                fThermal.Update(thermalLoad, dt);

                for (const auto& r : fWarehouse.GetAllRigs()) {
                    if (r) {
                        bool rPowered = r->IsPoweredOn();
                        for (const auto& gpu : r->GetGPUs()) {
                            if (gpu) {
                                double cardTemp = rPowered ? fThermal.CalculateGPUTemperature(gpu->GetEffectivePowerWatts(), gpu->GetFanSpeedPercent() / 100.0)
                                                           : fThermal.GetAmbientTemperature();
                                if (isActiveFacility && cardTemp > activeMaxCardTemp) {
                                    activeMaxCardTemp = cardTemp;
                                }

                                if (rPowered) {
                                    gpu->SetThrottled(Core::ThermalModel::IsOverheating(cardTemp));

                                    if (cardTemp >= 105.0 && !fCooling.IsImmersionCoolingActive()) {
                                        gpu->TakeDamage(dt * 5.0);
                                    }

                                    if (cardTemp >= 140.0 && !fCooling.IsImmersionCoolingActive()) {
                                        if (marketCatalog.HasAutoFireSuppression()) {
                                            r->SetPoweredOn(false);
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

                companyTotalMinedHashrate += fWarehouse.CalculateTotalHashrate() * fPowerGrid.GetHashrateSurgeMultiplier();
            }
        }

        // Aktif tesis için termal FLIR gölgelendirici durumunu güncelle
        float normTemp = static_cast<float>((activeMaxCardTemp - 20.0) / 70.0);
        shaderManager.SetThermalState(shaderManager.IsThermalActive(), normTemp);

        // Tüm aktif tesislerden toplanan net kazım gücü ile coin madenciliği
        if (companyTotalMinedHashrate > 0.0) {
            economy.MineCoins(companyTotalMinedHashrate, dt);
        }

        economy.UpdateMarket(dt);
        taskManager.UpdateProgress(warehouse, economy, coolingManager, powerGrid);

        // Otomatik Kayıt Zamanlayıcısı (Her 45 saniyede bir kaydet)
        autoSaveTimer += dt;
        if (autoSaveTimer >= AUTO_SAVE_INTERVAL) {
            autoSaveTimer = 0.0f;
            triggerSave(isTR ? "[OK] OTOMATIK KAYIT TAMAMLANDI" : "[OK] AUTO-SAVED");
        }

        if (saveToastTimer > 0.0f) {
            saveToastTimer -= dt;
        }

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

        // Wallet veya Market rozetine tıklayarak doğrudan Borsayı açma
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !settingsModal.IsOpen() && !marketModal.IsOpen() && !taskModal.IsOpen() && !worldMapModal.IsOpen()) {
            Rectangle walletBadgeRec{pad + (2 * (badgeW + badgeGap)), badgeY, badgeW, badgeH};
            Rectangle marketBadgeRec{pad + (3 * (badgeW + badgeGap)), badgeY, badgeW, badgeH};
            Vector2 mPos = GetMousePosition();
            if (CheckCollisionPointRec(mPos, walletBadgeRec) || CheckCollisionPointRec(mPos, marketBadgeRec)) {
                cryptoExchangeModal.Open();
            }
        }

        btnOpenSettings.Draw();
        btnQuickSave.Draw();
        btnOpenTasks.Draw();
        btnOpenWorldMap.Draw();

        // Kaydedildi / Yüklendi Bildirim Rozeti (Save Toast)
        if (saveToastTimer > 0.0f) {
            float toastW = Render::UIFrame::MeasureTextCustom(saveToastMessage, 15.0f, true) + 36.0f;
            float toastX = (screenW - toastW) * 0.5f;
            float toastY = headerH + 16.0f;
            float toastAlpha = std::clamp(saveToastTimer / 0.5f, 0.0f, 1.0f);
            Color bgColor{16, 42, 28, static_cast<unsigned char>(toastAlpha * 240.0f)};
            Color borderCol{0, 255, 140, static_cast<unsigned char>(toastAlpha * 255.0f)};
            Color textCol{220, 255, 235, static_cast<unsigned char>(toastAlpha * 255.0f)};

            Rectangle toastRec{toastX, toastY, toastW, 36.0f};
            DrawRectangleRounded(toastRec, 0.3f, 4, bgColor);
            DrawRectangleRoundedLines(toastRec, 0.3f, 4, 1.5f, borderCol);
            Render::UIFrame::DrawTextCustom(saveToastMessage, toastX + 18.0f, toastY + 9.0f, 15.0f, textCol, true);
        }

        // Küresel Ağ Zorluk ve Güç Sıçraması (Mining Spike) Canlı Uyarısı
        if (powerGrid.IsNetworkSpikeActive()) {
            std::string spikeText = isTR ? "[ALARM] [KURESEL AG GUC ZIRVESI! +25% ANLIK WATT CEKISI & +40% KAZIM ODULU!] "
                                         : "[ALARM] [GLOBAL NETWORK MINING SPIKE! +25% POWER DRAW & +40% HASH REWARD!] ";
            spikeText += std::to_string(static_cast<int>(powerGrid.GetNetworkSpikeRemainingSeconds()) + 1) + "s";
            DrawRectangle(0, static_cast<int>(headerH - 2), static_cast<int>(screenW), 20, Color{235, 145, 20, 240});
            float tW = Render::UIFrame::MeasureTextCustom(spikeText, 12.0f, true);
            Render::UIFrame::DrawTextCustom(spikeText, (screenW - tW) * 0.5f, headerH + 1.0f, 12.0f, WHITE, true);
        }

        // 2. SOL PANEL: VIEWPORT ÇERÇEVESİ (SEÇİLİ RİG DETAYI VEYA DEPO KUŞBAKIŞI)
        const Rectangle viewportRect{pad, contentY, leftW, contentH};
        double roomTemp = thermalModel.GetAmbientTemperature();
        std::string tempTag = (roomTemp <= 0.0) ? "[SOGUK] " : ((roomTemp >= 28.0) ? "[SICAK] " : "[ILIK] ");
        std::string viewportTitle = (currentViewMode == WarehouseViewMode::RIG_DETAIL)
            ? (warehouse.GetFacilityName() + " (" + std::to_string(warehouse.GetActiveRigIndex() + 1) + "/" + std::to_string(warehouse.GetRigCount()) + " RIG | " + tempTag + "Oda: " + std::to_string(static_cast<int>(roomTemp)) + "C)")
            : ("DEPO GENEL BAKIS - " + warehouse.GetFacilityName() + " (" + std::to_string(warehouse.GetRigCount()) + "/" + std::to_string(warehouse.GetMaxRigCapacity()) + " RIG | " + tempTag + "Oda: " + std::to_string(static_cast<int>(roomTemp)) + "C)");
        Render::UIFrame::DrawCard(viewportRect, "", Color{0, 220, 255, 255});

        // Üst Başlık ve Ayrım Çizgisi (Sekmelerle asla üst üste binmez)
        DrawRectangle(static_cast<int>(viewportRect.x + 16), static_cast<int>(viewportRect.y + 12), 4, 20, Color{0, 220, 255, 255});
        float maxTitleW = std::max(100.0f, leftW - 275.0f);
        float titleSize = 16.0f;
        float vTitleW = Render::UIFrame::MeasureTextCustom(viewportTitle, titleSize, true);
        if (vTitleW > maxTitleW && vTitleW > 0.0f) {
            titleSize = std::max(10.5f, titleSize * (maxTitleW / vTitleW));
        }
        Render::UIFrame::DrawTextCustom(viewportTitle, viewportRect.x + 26.0f, viewportRect.y + 13.0f, titleSize, RAYWHITE, true);
        DrawLine(static_cast<int>(viewportRect.x + 16), static_cast<int>(viewportRect.y + 40),
                 static_cast<int>(viewportRect.x + viewportRect.width - 16), static_cast<int>(viewportRect.y + 40),
                 Color{38, 46, 62, 255});

        // Sekme Butonlarını Çiz
        btnTabRigDetail.Draw();
        btnTabOverview.Draw();

        if (currentViewMode == WarehouseViewMode::RIG_DETAIL) {
            btnPrevRig.Draw();
            btnNextRig.Draw();
            btnToggleRigPower.Draw();
            btnSellRig.Draw();

            if (activeRig) {
                shaderManager.BeginShader();
                rigRenderer.DrawRig(*activeRig, thermalModel, static_cast<int>(rigX), static_cast<int>(rigY), animTime, &textureManager, powerGrid.IsBreakerTripped());
                shaderManager.EndShader();
            }

            // Güç Gösterge Barı
            const float powerRatio = static_cast<float>(powerGrid.GetTotalConsumptionWatts() / powerGrid.GetMaxCapacityWatts());
            const Color powerColor = (powerRatio > 0.85f) ? Color{255, 50, 50, 255} : ((powerRatio > 0.60f) ? Color{255, 180, 0, 255} : Color{0, 230, 130, 255});
            const std::string powerText = "DEPO SEBEKE YUKU: " + std::to_string(static_cast<int>(powerGrid.GetTotalConsumptionWatts())) +
                                          "W / " + std::to_string(static_cast<int>(powerGrid.GetMaxCapacityWatts())) + "W";

            const float barW = std::min(rigBaseW, leftW - 40.0f);
            const float barX = pad + (leftW - barW) / 2.0f;
            const float barY = rigY + 332.0f;

            Render::UIFrame::DrawProgressBar(Rectangle{barX, barY, barW, 26.0f}, powerRatio, powerColor, powerText);

            // Termal, Demeraj veya Sigorta Bildirim Kutusu
            if (powerGrid.IsBreakerTripped()) {
                DrawRectangleRounded(Rectangle{barX, barY + 32.0f, barW, 36.0f}, 0.2f, 4, Color{190, 20, 20, 240});
                Render::UIFrame::DrawTextCustom("! SEBEKE ASIRI YUKLENDI - SIGORTA ATTI ! SAG PANELDEN SALTERI ACIN",
                                               barX + 24.0f, barY + 42.0f, 15.0f, WHITE, true);
            } else if (activeRig && activeRig->IsInStartupSurge()) {
                DrawRectangleRounded(Rectangle{barX, barY + 32.0f, barW, 36.0f}, 0.2f, 4, Color{180, 110, 15, 230});
                Render::UIFrame::DrawTextCustom("[DEMERAJ] KALKIS AKIMI AKTIF: +30% GUC CEKISI - FANLAR VE KAPASITORLER",
                                               barX + 24.0f, barY + 42.0f, 14.0f, WHITE, true);
            } else if (shaderManager.IsThermalActive()) {
                DrawRectangleRounded(Rectangle{barX, barY + 32.0f, barW, 36.0f}, 0.2f, 4, Color{32, 16, 52, 230});
                DrawRectangleRoundedLines(Rectangle{barX, barY + 32.0f, barW, 36.0f}, 0.2f, 4, 1.4f, Color{220, 0, 255, 255});
                Render::UIFrame::DrawTextCustom("[CANLI TERMAL FLIR VIZYONU AKTIF] - Isi dagilimi fragment shader ile renklendiriliyor",
                                               barX + 24.0f, barY + 42.0f, 14.0f, Color{230, 130, 255, 255}, true);
            }

            // Hızlı Rig Seçici Şeridi (Quick Rig Selector)
            Vector2 mouse = GetMousePosition();
            int quickRigIdx = -1;
            Rectangle selectorBounds{pad + 20.0f, contentY + contentH - 40.0f, leftW - 40.0f, 30.0f};
            rigRenderer.DrawQuickRigSelector(warehouse, thermalModel, selectorBounds, mouse, quickRigIdx);
            if (quickRigIdx >= 0) {
                warehouse.SetActiveRigIndex(static_cast<size_t>(quickRigIdx));
            }
        } else {
            // Kuşbakışı Genel Bakış Görünümü (Akıcı Fare Tekerleği Kaydırması)
            Vector2 mouse = GetMousePosition();
            int selectedRigIdx = -1;
            int toggledRigIdx = -1;
            rigRenderer.DrawWarehouseOverviewGrid(warehouse, thermalModel,
                                                 Rectangle{pad + 10.0f, contentY + 46.0f, leftW - 20.0f, contentH - 56.0f},
                                                 animTime, mouse, overviewScrollY, maxOverviewScrollY,
                                                 selectedRigIdx, toggledRigIdx);
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
        btnUpgradePSU.Draw();
        btnSellCrypto.Draw();
        btnUpgradeCooling.Draw();
        btnOverclock.Draw();
        btnUndervolt.Draw();
        btnThermalToggle.Draw();
        btnResetBreaker.Draw();

        // 4. ALT BİLGİ VE KONTROL ŞERİDİ
        DrawRectangle(0, static_cast<int>(screenH - footerH), static_cast<int>(screenW), static_cast<int>(footerH), Color{14, 17, 23, 250});
        DrawLine(0, static_cast<int>(screenH - footerH), static_cast<int>(screenW), static_cast<int>(screenH - footerH), Color{35, 42, 56, 255});

        const float footerTextY = screenH - footerH + ((footerH - 14.0f) / 2.0f);
        Render::UIFrame::DrawTextCustom(Core::LocalizationManager::Tr("TIP_FOOTER"),
                                       pad + 10.0f, footerTextY, 13.0f, Color{150, 165, 190, 255}, false);

        std::string verTag = "GameOfTex v1.9.2 [Segoe UI Native F11]";
        float verW = Render::UIFrame::MeasureTextCustom(verTag, 13.0f, false);
        Render::UIFrame::DrawTextCustom(verTag, screenW - verW - pad - 10.0f, footerTextY, 13.0f, Color{100, 120, 150, 255}, false);

        // 5. GPU 360 İNCELEME MODALI (AÇIKSA EN ÜSTTE ÇİZİLİR)
        if (gpuInspectionModal.IsOpen()) {
            gpuInspectionModal.Draw(animTime, &textureManager, &economy, activeFacility->gridPricePerKwh);
        }

        // 6. AYARLAR MODAL PENCERESİ
        if (settingsModal.IsOpen()) {
            settingsModal.Draw(economy);
        }

        // 7. DONANIM VE TESİS MARKETİ MODAL PENCERESİ
        if (marketModal.IsOpen()) {
            marketModal.Draw(economy, warehouse, coolingManager, marketCatalog, powerGrid);
        }

        // 8. GOREV & HEDEF MERKEZI MODAL PENCERESI
        if (taskModal.IsOpen()) {
            taskModal.Draw(taskManager, economy);
        }

        // 9. DUNYA HARITASI MODAL PENCERESI
        if (worldMapModal.IsOpen()) {
            worldMapModal.Draw(facilityManager, economy);
        }

        // 10. KRIPTO BORSA VE CANLI GRAFIK MODAL PENCERESI
        if (cryptoExchangeModal.IsOpen()) {
            cryptoExchangeModal.Draw(economy);
        }

        EndDrawing();
    }

    // 8. Cikista Otomatik Kayit ve Temizlik
    Core::SaveManager::SaveGame(Core::SaveManager::DEFAULT_SAVE_PATH, userProfile, economy, facilityManager, taskManager, marketCatalog);
    textureManager.Unload();
    if (fontRegular.texture.id > 0) {
        UnloadFont(fontRegular);
        UnloadFont(fontBold);
    }
    CloseWindow();
    return 0;
}
