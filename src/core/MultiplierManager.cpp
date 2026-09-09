#include "MultiplierManager.hpp"
#include "MiningRig.hpp"
#include "Warehouse.hpp"
#include "ThermalModel.hpp"
#include "EconomyManager.hpp"
#include "MarketCatalog.hpp"
#include "Localization.hpp"
#include "../render/UIFrame.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace Render {

MultiplierManager::MultiplierManager()
    : m_spawnCooldown(45.0f)
{
}

void MultiplierManager::SetSpawnArea(Rectangle area) {
    m_spawnArea = area;
}

void MultiplierManager::SpawnGoldenNonce() {
    float pad = 40.0f;
    float minX = m_spawnArea.x + pad;
    float maxX = std::max(minX + 50.0f, m_spawnArea.x + m_spawnArea.width - pad);
    float minY = m_spawnArea.y + pad;
    float maxY = std::max(minY + 50.0f, m_spawnArea.y + m_spawnArea.height - pad);

    float rx = minX + static_cast<float>(std::rand() % static_cast<int>(std::max(1.0f, maxX - minX)));
    float ry = minY + static_cast<float>(std::rand() % static_cast<int>(std::max(1.0f, maxY - minY)));

    m_noncePos = Vector2{rx, ry};
    m_nonceActive = true;
    m_nonceTimer = 18.0f; // 18 seconds on screen before disappearing
}

void MultiplierManager::SpawnParticles(Vector2 center, int count) {
    for (int i = 0; i < count; ++i) {
        GoldenParticle p;
        p.pos = center;
        float angle = static_cast<float>(std::rand() % 360) * (3.14159265f / 180.0f);
        float speed = 40.0f + static_cast<float>(std::rand() % 160);
        p.vel = Vector2{std::cos(angle) * speed, std::sin(angle) * speed};
        p.size = 2.5f + static_cast<float>(std::rand() % 4);
        p.alpha = 1.0f;
        int col = std::rand() % 3;
        if (col == 0) p.color = Color{255, 215, 0, 255};
        else if (col == 1) p.color = Color{255, 165, 0, 255};
        else p.color = Color{0, 240, 255, 255};
        m_particles.push_back(p);
    }
}

void MultiplierManager::Update(double dt, Core::EconomyManager& economy, Core::Warehouse& warehouse,
                               const Core::ThermalModel& thermalModel) {
    (void)warehouse;
    (void)thermalModel;
    float fdt = static_cast<float>(dt);

    // 1. Parçacık simülasyonu
    for (auto it = m_particles.begin(); it != m_particles.end();) {
        it->pos.x += it->vel.x * fdt;
        it->pos.y += it->vel.y * fdt;
        it->vel.x *= 0.94f;
        it->vel.y *= 0.94f;
        it->alpha -= fdt * 1.2f;
        if (it->alpha <= 0.0f) {
            it = m_particles.erase(it);
        } else {
            ++it;
        }
    }

    // 2. Altın Nonce zamanlayıcısı
    if (m_nonceActive) {
        m_nonceTimer -= fdt;
        if (m_nonceTimer <= 0.0f) {
            m_nonceActive = false;
            m_spawnCooldown = 50.0f + static_cast<float>(std::rand() % 40);
        }
    } else {
        m_spawnCooldown -= fdt;
        if (m_spawnCooldown <= 0.0f) {
            SpawnGoldenNonce();
        }
    }

    // 3. Aktif Frenzy geri sayımı
    if (m_activeFrenzy != FrenzyType::NONE) {
        m_frenzyRemaining -= fdt;
        if (m_frenzyRemaining <= 0.0f) {
            m_activeFrenzy = FrenzyType::NONE;
            m_frenzyRemaining = 0.0f;
            m_frenzyBanner.clear();
        }
    }

    if (m_toastTimer > 0.0f) {
        m_toastTimer -= fdt;
    }
}

void MultiplierManager::TriggerFrenzy(FrenzyType type, Core::EconomyManager& economy,
                                      Core::Warehouse& warehouse, const Core::MarketCatalog& catalog) {
    bool isTR = (Core::LocalizationManager::Get().GetLanguage() == Core::Language::TURKISH);
    m_activeFrenzy = type;

    switch (type) {
        case FrenzyType::TURBO_HASH:
            m_frenzyDuration = 30.0f;
            m_frenzyRemaining = 30.0f;
            m_frenzyBanner = isTR ? "[ALTIN NONCE] TURBO MADENCILIK FRENZY! x5.0 KAZIM HIZI CARPANI AKTIF!"
                                  : "[GOLDEN NONCE] TURBO MINING FRENZY! x5.0 HASHRATE MULTIPLIER ACTIVE!";
            break;

        case FrenzyType::BULL_RALLY:
            m_frenzyDuration = 25.0f;
            m_frenzyRemaining = 25.0f;
            m_frenzyBanner = isTR ? "[BOGA RALLISI] KRIPTO PIYASASI PATLADI! +%80 DEGER VE SATIS CARPANI!"
                                  : "[BULL RALLY] CRYPTO MARKET SURGE! +80% VALUE & SELL MULTIPLIER!";
            break;

        case FrenzyType::SUPERCONDUCTOR:
            m_frenzyDuration = 40.0f;
            m_frenzyRemaining = 40.0f;
            m_frenzyBanner = isTR ? "[SUPERILETKEN PULSE] -%50 GUC VE +%30 VERIMLILIK! SIFIR ISINMA!"
                                  : "[SUPERCONDUCTOR PULSE] -50% POWER & +30% EFFICIENCY! ZERO OVERHEAT!";
            break;

        case FrenzyType::AIRDROP: {
            m_activeFrenzy = FrenzyType::NONE;
            m_frenzyRemaining = 0.0f;
            double reward = 1500.0 + (std::rand() % 2500);
            economy.AddFiat(reward);
            m_toastTimer = 5.0f;
            m_frenzyToast = isTR ? ("[AIRDROP] GIRISIM FONUNDAN +" + economy.FormatFiat(reward) + " HEDIYE NAKIT DUSTU!")
                                 : ("[AIRDROP] VENTURE FUND GRANTED +" + economy.FormatFiat(reward) + " FREE CASH!");
            break;
        }

        default:
            break;
    }
}

bool MultiplierManager::CheckClick(Vector2 mousePos, Core::EconomyManager& economy,
                                   Core::Warehouse& warehouse, const Core::MarketCatalog& catalog) {
    if (!m_nonceActive) return false;

    if (CheckCollisionPointCircle(mousePos, m_noncePos, m_nonceRadius + 10.0f)) {
        // Altın patlama parçacıkları
        SpawnParticles(m_noncePos, 45);

        // Rastgele Frenzy seçimi
        int roll = std::rand() % 100;
        FrenzyType selectedType;
        if (roll < 45) {
            selectedType = FrenzyType::TURBO_HASH;     // 45% şansla x5 Hashrate
        } else if (roll < 70) {
            selectedType = FrenzyType::BULL_RALLY;     // 25% şansla Boğa Rallisi
        } else if (roll < 85) {
            selectedType = FrenzyType::SUPERCONDUCTOR; // 15% şansla Süperiletken
        } else {
            selectedType = FrenzyType::AIRDROP;        // 15% şansla Anında Nakit
        }

        TriggerFrenzy(selectedType, economy, warehouse, catalog);

        m_nonceActive = false;
        m_spawnCooldown = 60.0f + static_cast<float>(std::rand() % 45);
        return true;
    }

    return false;
}

void MultiplierManager::Draw(float animTime) const {
    // 1. Altın Kıvılcım Parçacıkları
    for (const auto& p : m_particles) {
        Color c = p.color;
        c.a = static_cast<unsigned char>(std::clamp(p.alpha * 255.0f, 0.0f, 255.0f));
        DrawCircle(static_cast<int>(p.pos.x), static_cast<int>(p.pos.y), p.size, c);
    }

    // 2. Altın Nonce Küresi
    if (m_nonceActive) {
        float floatY = m_noncePos.y + std::sin(animTime * 3.5f) * 6.0f;
        float pulse = (std::sin(animTime * 5.0f) + 1.0f) * 0.5f;
        float currentRadius = m_nonceRadius + pulse * 4.0f;

        // Dış neon halkalar
        DrawCircle(static_cast<int>(m_noncePos.x), static_cast<int>(floatY), currentRadius + 14.0f, Color{255, 200, 0, static_cast<unsigned char>(35 + pulse * 45)});
        DrawCircle(static_cast<int>(m_noncePos.x), static_cast<int>(floatY), currentRadius + 7.0f, Color{255, 230, 80, static_cast<unsigned char>(70 + pulse * 60)});
        DrawCircleLines(static_cast<int>(m_noncePos.x), static_cast<int>(floatY), currentRadius + 12.0f, Color{255, 215, 0, 180});

        // Altın Küre Gövdesi
        DrawCircle(static_cast<int>(m_noncePos.x), static_cast<int>(floatY), currentRadius, Color{255, 195, 20, 245});
        DrawCircleGradient(static_cast<int>(m_noncePos.x), static_cast<int>(floatY), currentRadius * 0.85f, Color{255, 255, 160, 255}, Color{230, 150, 0, 255});
        DrawCircleLines(static_cast<int>(m_noncePos.x), static_cast<int>(floatY), currentRadius, WHITE);

        // İç Logo / Yıldız
        UIFrame::DrawTextCustom("[*]", m_noncePos.x - 10.0f, floatY - 11.0f, 18.0f, BLACK, true);

        // Etiket
        std::string tag = "ALTIN NONCE!";
        float tagW = UIFrame::MeasureTextCustom(tag, 13.0f, true);
        Rectangle tagRec{m_noncePos.x - (tagW / 2.0f) - 6.0f, floatY + currentRadius + 6.0f, tagW + 12.0f, 20.0f};
        DrawRectangleRounded(tagRec, 0.3f, 4, Color{20, 20, 25, 230});
        DrawRectangleRoundedLines(tagRec, 0.3f, 4, 1.2f, Color{255, 215, 0, 255});
        UIFrame::DrawTextCustom(tag, tagRec.x + 6.0f, tagRec.y + 3.0f, 13.0f, Color{255, 225, 60, 255}, true);
    }
}

void MultiplierManager::DrawHUDNotification(float screenW, float headerH) const {
    if (m_activeFrenzy != FrenzyType::NONE) {
        float ratio = std::clamp(m_frenzyRemaining / std::max(1.0f, m_frenzyDuration), 0.0f, 1.0f);
        float bannerH = 26.0f;
        float bannerY = headerH + 2.0f;

        // Banner Arka Planı
        DrawRectangle(0, static_cast<int>(bannerY), static_cast<int>(screenW), static_cast<int>(bannerH), Color{28, 22, 10, 240});
        DrawLine(0, static_cast<int>(bannerY + bannerH), static_cast<int>(screenW), static_cast<int>(bannerY + bannerH), Color{255, 200, 0, 255});

        // Kalan Süre İlerleme Barı
        DrawRectangle(0, static_cast<int>(bannerY + bannerH - 4.0f), static_cast<int>(screenW * ratio), 4, Color{255, 215, 0, 255});

        // Başlık ve Süre
        std::string text = m_frenzyBanner + " (" + std::to_string(static_cast<int>(m_frenzyRemaining) + 1) + "s)";
        float tW = UIFrame::MeasureTextCustom(text, 14.0f, true);
        UIFrame::DrawTextCustom(text, (screenW - tW) * 0.5f, bannerY + 5.0f, 14.0f, Color{255, 235, 120, 255}, true);
    } else if (m_toastTimer > 0.0f) {
        float toastW = UIFrame::MeasureTextCustom(m_frenzyToast, 15.0f, true) + 36.0f;
        float toastX = (screenW - toastW) * 0.5f;
        float toastY = headerH + 16.0f;
        float alpha = std::clamp(m_toastTimer / 0.8f, 0.0f, 1.0f);

        Rectangle rec{toastX, toastY, toastW, 36.0f};
        DrawRectangleRounded(rec, 0.3f, 4, Color{45, 36, 12, static_cast<unsigned char>(alpha * 240.0f)});
        DrawRectangleRoundedLines(rec, 0.3f, 4, 1.5f, Color{255, 215, 0, static_cast<unsigned char>(alpha * 255.0f)});
        UIFrame::DrawTextCustom(m_frenzyToast, toastX + 18.0f, toastY + 9.0f, 15.0f, Color{255, 235, 150, static_cast<unsigned char>(alpha * 255.0f)}, true);
    }
}

SynergyInfo MultiplierManager::CalculateRigSynergy(const Core::MiningRig& rig, const Core::ThermalModel& thermalModel) const {
    SynergyInfo info;
    info.totalMultiplier = 1.0;

    const auto& gpus = rig.GetGPUs();
    if (gpus.empty()) return info;

    // 1. 6x Tam Set Kombo (Full Set Synergy): Kasadaki 6 kartın tümü takılı ve aynı model ise +%25
    if (gpus.size() >= 6 && rig.GetGPUCount() >= 6) {
        bool allSame = true;
        const std::string& firstModel = gpus[0]->GetName();
        for (size_t i = 1; i < 6; ++i) {
            if (!gpus[i] || gpus[i]->GetName() != firstModel) {
                allSame = false;
                break;
            }
        }
        if (allSame) {
            info.isFullSet = true;
            info.totalMultiplier *= 1.25;
            info.synergyLabel += "[6x KOMBO +%25] ";
        }
    }

    // 2. Kriyojenik Soğukluk Bonusu (Cryo Synergy): Ortalama sıcaklık 60°C altındaysa +%15
    if (rig.IsPoweredOn()) {
        double avgTemp = rig.CalculateAverageTemperature(thermalModel);
        if (avgTemp < 60.0 && avgTemp > 5.0) {
            info.isCryoCold = true;
            info.totalMultiplier *= 1.15;
            info.synergyLabel += "[KRIYO +%15] ";
        }
    }

    // 3. Ekolojik Güç Bonusu (Eco Power): PSU yükü %70 altındaysa +%10
    if (rig.IsPoweredOn()) {
        double currentW = rig.CalculateTotalPowerWatts();
        double maxPsu = rig.GetPSUMaxWatts();
        if (maxPsu > 0 && currentW < maxPsu * 0.70) {
            info.isEcoPower = true;
            info.totalMultiplier *= 1.10;
            info.synergyLabel += "[EKO GUC +%10] ";
        }
    }

    return info;
}

double MultiplierManager::GetGlobalHashMultiplier() const {
    double mult = 1.0;
    if (m_activeFrenzy == FrenzyType::TURBO_HASH) {
        mult *= 5.0; // 5 Kat Hashrate!
    } else if (m_activeFrenzy == FrenzyType::SUPERCONDUCTOR) {
        mult *= 1.30; // %30 Süperiletken Verim Artışı
    }
    return mult;
}

double MultiplierManager::GetPriceMultiplier() const {
    if (m_activeFrenzy == FrenzyType::BULL_RALLY) {
        return 1.80; // %80 Boğa Rallisi Fiyat Artışı!
    }
    return 1.0;
}

bool MultiplierManager::IsSuperconductorActive() const {
    return (m_activeFrenzy == FrenzyType::SUPERCONDUCTOR);
}

bool MultiplierManager::IsFrenzyActive() const {
    return (m_activeFrenzy != FrenzyType::NONE);
}

FrenzyType MultiplierManager::GetActiveFrenzyType() const {
    return m_activeFrenzy;
}

float MultiplierManager::GetFrenzyRemainingSeconds() const {
    return m_frenzyRemaining;
}

const std::string& MultiplierManager::GetFrenzyBannerText() const {
    return m_frenzyBanner;
}

} // namespace Render
