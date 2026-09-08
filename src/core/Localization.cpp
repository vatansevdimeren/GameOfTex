#include "Localization.hpp"

namespace Core {

LocalizationManager& LocalizationManager::Get() {
    static LocalizationManager instance;
    return instance;
}

LocalizationManager::LocalizationManager()
    : m_currentLanguage(Language::TURKISH)
{
    InitTranslations();
}

void LocalizationManager::SetLanguage(Language lang) {
    m_currentLanguage = lang;
}

Language LocalizationManager::GetLanguage() const {
    return m_currentLanguage;
}

void LocalizationManager::ToggleLanguage() {
    m_currentLanguage = (m_currentLanguage == Language::TURKISH) ? Language::ENGLISH : Language::TURKISH;
}

const std::string& LocalizationManager::GetLanguageName() const {
    static const std::string tr = "TURKCE 🇹🇷";
    static const std::string en = "ENGLISH 🇺🇸";
    return (m_currentLanguage == Language::TURKISH) ? tr : en;
}

const char* LocalizationManager::Get(const std::string& key) const {
    const auto& map = (m_currentLanguage == Language::TURKISH) ? m_translationsTR : m_translationsEN;
    auto it = map.find(key);
    if (it != map.end()) {
        return it->second.c_str();
    }
    return key.c_str();
}

const char* LocalizationManager::Tr(const std::string& key) {
    return Get().Get(key);
}

void LocalizationManager::InitTranslations() {
    // ==================== TÜRKÇE SÖZLÜK ====================
    m_translationsTR = {
        {"BADGE_COMPANY", "SIRKET"},
        {"BADGE_CASH", "NAKIT"},
        {"BADGE_CRYPTO", "KRIPTO"},
        {"BADGE_MARKET", "PIYASA"},
        {"BADGE_SPEED", "TOPLAM HASH"},
        
        {"TAB_RIG_DETAIL", "DETAYLI RIG"},
        {"TAB_OVERVIEW", "DEPO GENEL BAKIS"},
        
        {"RIG_POWER_OFF", "RIG'I KAPAT"},
        {"RIG_POWER_ON", "RIG'I AC"},
        {"RIG_POWER_OFF_SUB", "Gucu Kes"},
        {"RIG_POWER_ON_SUB", "Calistir"},
        {"RIG_SELL", "RIG'I SAT"},
        {"RIG_SELL_SUB", "+$1,200 Hurda"},
        {"RIG_PREV", "< ONCEKI"},
        {"RIG_NEXT", "SONRAKI >"},
        {"RIG_ACTIVE", "AKTIF / CALISIYOR"},
        {"RIG_STOPPED", "KAPALI / DEVRE DISI"},
        
        {"BTN_BUY_GPU", "GPU SATIN AL"},
        {"BTN_BUY_RIG", "YENI RIG SATIN AL"},
        {"BTN_SELL_CRYPTO", "KRIPTO PARALARI SAT"},
        {"BTN_UPGRADE_COOLING", "SOGUTMAYI YUKSELT"},
        {"BTN_OVERCLOCK", "HIZ ASIRTMA (+5% OC)"},
        {"BTN_OVERCLOCK_SUB", "Daha Fazla MH/s"},
        {"BTN_UNDERVOLT", "VOLTAJ DUSUR (-5% UV)"},
        {"BTN_UNDERVOLT_SUB", "Daha Az Guc ve Isi"},
        {"BTN_THERMAL", "TERMAL VIZYON MODU"},
        {"BTN_THERMAL_ON", "[AKTIF - RENKLI ISI HARITASI]"},
        {"BTN_THERMAL_OFF", "[KAPALI - NORMAL GORUNUM]"},
        {"BTN_BREAKER_RESET", "SIGORTA SALTERINI AC"},
        {"BTN_BREAKER_TRIPPED", "! SALTER ATTI - TIKLA !"},
        {"BTN_BREAKER_OK", "Sebeke Normal"},
        {"BTN_SETTINGS", "AYARLAR"},
        
        {"INSPECT_TITLE", "360 DERECE GPU DONANIM & OC INCELEME"},
        {"INSPECT_SILICON", "Silikon Kalitesi"},
        {"INSPECT_GOLDEN", " (Altin Silikon!)"},
        {"INSPECT_STANDARD", " (Standart)"},
        {"INSPECT_TEMP", "Anlik Cekirdek Isisi"},
        {"INSPECT_HASH", "Etkin Kazim Gucu"},
        {"INSPECT_WATTS", "Guc Tuketimi"},
        {"INSPECT_CLOCK", "Cekirdek Saat Farki"},
        {"INSPECT_POWER_LIMIT", "Guc Limiti"},
        {"INSPECT_FAN_SPEED", "Manuel Fan Devri"},
        {"INSPECT_REPAIR", "KARTI TAMIR ET"},
        {"INSPECT_REPAIR_SUB", "Maliyet: $400"},
        {"INSPECT_SCRAP", "KARTI HURDAYA SAT / CIKAR"},
        {"INSPECT_SCRAP_SUB", "+$75 Hurda Parasi"},
        {"INSPECT_CLOSE", "INCELEMEYI KAPAT"},
        {"CARD_BURNT", "YANDI!"},
        {"CARD_INSPECT", "INCELE"},
        
        {"SETTINGS_TITLE", "SISTEM VE EKRAN AYARLARI"},
        {"SETTINGS_SCALE_DESC", "Kullanici Arayuzu Olcegi (Font ve Buton Boyutu):"},
        {"SETTINGS_FULLSCREEN", "TAM EKRAN AC (F11)"},
        {"SETTINGS_WINDOWED", "PENCERE MODUNA DON (F11 / ESC)"},
        {"SETTINGS_LANGUAGE", "DIL SECIMI: "},
        {"SETTINGS_CURRENCY", "PARA BIRIMI: "},
        {"SETTINGS_CLOSE", "KAPAT VE DEVAM ET"},
        
        {"GRID_MINE", "Kazim: "},
        {"GRID_POWER", "Guc: "},
        {"GRID_MAX_TEMP", "Max Isi: "},
        {"GRID_INSPECT", "INCELE"},
        
        {"TIP_FOOTER", "[IPUCU] Kartlara tiklayarak 360 inceleyebilir, ESC ile pencereyi kucultebilirsiniz!"}
    };

    // ==================== ENGLISH DICTIONARY ====================
    m_translationsEN = {
        {"BADGE_COMPANY", "COMPANY"},
        {"BADGE_CASH", "CASH"},
        {"BADGE_CRYPTO", "CRYPTO"},
        {"BADGE_MARKET", "MARKET"},
        {"BADGE_SPEED", "TOTAL HASH"},
        
        {"TAB_RIG_DETAIL", "RIG DETAIL"},
        {"TAB_OVERVIEW", "WAREHOUSE OVERVIEW"},
        
        {"RIG_POWER_OFF", "SHUTDOWN RIG"},
        {"RIG_POWER_ON", "START RIG"},
        {"RIG_POWER_OFF_SUB", "Cut Power"},
        {"RIG_POWER_ON_SUB", "Power On"},
        {"RIG_SELL", "SELL RIG"},
        {"RIG_SELL_SUB", "+$1,200 Scrap"},
        {"RIG_PREV", "< PREVIOUS"},
        {"RIG_NEXT", "NEXT >"},
        {"RIG_ACTIVE", "ACTIVE / MINING"},
        {"RIG_STOPPED", "OFFLINE / STANDBY"},
        
        {"BTN_BUY_GPU", "BUY GPU"},
        {"BTN_BUY_RIG", "BUY NEW RIG"},
        {"BTN_SELL_CRYPTO", "SELL CRYPTO"},
        {"BTN_UPGRADE_COOLING", "UPGRADE COOLING"},
        {"BTN_OVERCLOCK", "OVERCLOCK (+5% OC)"},
        {"BTN_OVERCLOCK_SUB", "Higher MH/s"},
        {"BTN_UNDERVOLT", "UNDERVOLT (-5% UV)"},
        {"BTN_UNDERVOLT_SUB", "Lower Power & Heat"},
        {"BTN_THERMAL", "THERMAL VISION MODE"},
        {"BTN_THERMAL_ON", "[ACTIVE - COLOR HEATMAP]"},
        {"BTN_THERMAL_OFF", "[OFF - NORMAL VIEW]"},
        {"BTN_BREAKER_RESET", "RESET BREAKER SWITCH"},
        {"BTN_BREAKER_TRIPPED", "! BREAKER TRIPPED - CLICK !"},
        {"BTN_BREAKER_OK", "Grid Normal"},
        {"BTN_SETTINGS", "SETTINGS"},
        
        {"INSPECT_TITLE", "360 DEGREE GPU HARDWARE & OC INSPECTION"},
        {"INSPECT_SILICON", "Silicon Quality"},
        {"INSPECT_GOLDEN", " (Golden Silicon!)"},
        {"INSPECT_STANDARD", " (Standard)"},
        {"INSPECT_TEMP", "Core Temperature"},
        {"INSPECT_HASH", "Effective Hashrate"},
        {"INSPECT_WATTS", "Power Draw"},
        {"INSPECT_CLOCK", "Core Clock Offset"},
        {"INSPECT_POWER_LIMIT", "Power Limit"},
        {"INSPECT_FAN_SPEED", "Manual Fan Duty"},
        {"INSPECT_REPAIR", "REPAIR GRAPHICS CARD"},
        {"INSPECT_REPAIR_SUB", "Cost: $400"},
        {"INSPECT_SCRAP", "SCRAP / RECYCLE CARD"},
        {"INSPECT_SCRAP_SUB", "+$75 Scrap Cash"},
        {"INSPECT_CLOSE", "CLOSE INSPECTION"},
        {"CARD_BURNT", "BURNT!"},
        {"CARD_INSPECT", "INSPECT"},
        
        {"SETTINGS_TITLE", "SYSTEM & DISPLAY SETTINGS"},
        {"SETTINGS_SCALE_DESC", "UI Text & Component Scaling:"},
        {"SETTINGS_FULLSCREEN", "ENTER FULLSCREEN (F11)"},
        {"SETTINGS_WINDOWED", "EXIT TO WINDOWED (F11 / ESC)"},
        {"SETTINGS_LANGUAGE", "LANGUAGE: "},
        {"SETTINGS_CURRENCY", "CURRENCY: "},
        {"SETTINGS_CLOSE", "CLOSE & RESUME"},
        
        {"GRID_MINE", "Mining: "},
        {"GRID_POWER", "Power: "},
        {"GRID_MAX_TEMP", "Max Temp: "},
        {"GRID_INSPECT", "INSPECT"},
        
        {"TIP_FOOTER", "[TIP] Click cards for 360 inspection, press ESC to exit fullscreen or modals!"}
    };
}

} // namespace Core
