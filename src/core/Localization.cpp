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
        {"TIP_FOOTER", "[IPUCU] Kartlara tiklayarak 360 inceleyebilir, ESC ile pencereyi kucultebilirsiniz!"},
        
        {"MARKET_TITLE", "DONANIM VE TESIS MARKETI"},
        {"MARKET_TAB_GPUS", "EKRAN KARTLARI"},
        {"MARKET_TAB_POWER", "ELEKTRIK & TRAFO"},
        {"MARKET_TAB_COOLING", "SOGUTMA SISTEMLERI"},
        {"MARKET_TAB_FACILITIES", "TESIS & DEPO"},
        {"MARKET_ACTIVE_RIG", "Hedef: "},
        {"MARKET_RIG_FULL", "RIG DOLU!"},
        {"MARKET_NO_MONEY", "YETERSIZ BAKIYE"},
        {"MARKET_BUY", "SATIN AL"},
        {"MARKET_INSTALLED", "KURULDU (AKTIF)"},
        {"MARKET_HASH_BAR", "Kazim"},
        {"MARKET_POWER_BAR", "Guc"},
        {"MARKET_EFF_BAR", "Verim"},
        {"TIER_ENTRY", "Giris Seviye"},
        {"TIER_MID", "F/P Canavari"},
        {"TIER_HIGH", "Yuksek Performans"},
        {"TIER_ULTRA", "Amiral Gemisi"},
        {"TIER_ASIC", "Endustriyel ASIC"},
        {"POWER_TIER_1", "Standart Pano (3,600W)"},
        {"POWER_TIER_2", "Sanayi Tipi Trifaze (7,500W)"},
        {"POWER_TIER_3", "Ozel Trafo Istasyonu (15,000W)"},
        {"POWER_TIER_4", "Yuksek Gerilim Hatti (30,000W)"},
        {"FACILITY_SOLAR_1", "Gunes Paneli Paketi-1"},
        {"FACILITY_SOLAR_1_DESC", "+1,000W Surekli Yesil Uretim"},
        {"FACILITY_SOLAR_2", "Gunes Paneli Paketi-2"},
        {"FACILITY_SOLAR_2_DESC", "+2,500W Sanayi Gunes Tarlasi"},
        {"FACILITY_FIRE_EXT", "Otomatik Yangin Sondurucu"},
        {"FACILITY_FIRE_EXT_DESC", "140C uzerinde kartlarin yanmasini onler"},
        
        {"BTN_TASKS", "GOREVLER"},
        {"TASK_MODAL_TITLE", "GOREV & HEDEF MERKEZI"},
        {"TASK_READY_BADGE", "ODUL HAZIR"},
        {"TASK_REWARD_PREFIX", "Odul:"},
        {"TASK_BTN_CLAIM", "ODULU AL"},
        {"TASK_BTN_CLAIMED", "ALINDI"},
        {"TASK_BTN_IN_PROGRESS", "SURUYOR"},
        
        {"TASK_TITLE_INSPECT", "Donanim Uzmani"},
        {"TASK_DESC_INSPECT", "Bir karta tiklayarak 360 Inceleme panelini ac"},
        {"TASK_TITLE_OVERCLOCK", "Hiz Asirtma"},
        {"TASK_DESC_OVERCLOCK", "Herhangi bir karta Overclock uygula"},
        {"TASK_TITLE_BUY_GPU", "Pazar Alisverisi"},
        {"TASK_DESC_BUY_GPU", "Marketten en az 1 GPU satin al"},
        {"TASK_TITLE_HASHRATE", "Madenci Gucu"},
        {"TASK_DESC_HASHRATE", "Toplam kazim hizini 150 MH/s uzerine cikar"},
        {"TASK_TITLE_SELL_CRYPTO", "Kripto Tuccari"},
        {"TASK_DESC_SELL_CRYPTO", "Borsada en az $500 degerinde kripto sat"},
        {"TASK_TITLE_MULTI_RIG", "Buyuk Tesis"},
        {"TASK_DESC_MULTI_RIG", "Depodaki rig kasasi sayisini 2 veya uzerine cikar"},
        {"TASK_TITLE_COOLING", "Termal Kontrol"},
        {"TASK_DESC_COOLING", "Tesis sogutma gucunu 3,000W uzerine cikar"},
        {"TASK_TITLE_POWER", "Sanayi Trafosu"},
        {"TASK_DESC_POWER", "Sebeke panosunu 7,500W veya uzerine yukselt"},
        {"TASK_TITLE_SOLAR", "Yesil Enerji"},
        {"TASK_DESC_SOLAR", "Tesisine en az 1,000W gunes paneli paketi kur"},
        
        {"BTN_WORLD_MAP", "HARITA"},
        {"WORLD_MAP_TITLE", "KURESEL MADENCILIK TESISLERI & DUNYA HARITASI"},
        {"FAC_OWNED", "SAHIPSINIZ"},
        {"FAC_ALREADY_PURCHASED", "SATIN ALINDI"},
        {"FAC_BADGE_ACTIVE", "AKTIF TESIS"},
        {"FAC_BADGE_OWNED", "SATIN ALINDI"},
        {"FAC_BADGE_FOR_SALE", "SATILIK"},
        {"FAC_BTN_ACTIVE", "BURADASINIZ"},
        {"FAC_BTN_SWITCH", "TESISE GECIS YAP"},
        {"FAC_BTN_BUY", "TESISI SATIN AL"},
        {"FAC_PARAM_CLIMATE", "Ortam Iklim Isisi:"},
        {"FAC_PARAM_CAPACITY", "Max Rig Kapasitesi:"},
        {"FAC_PARAM_GRID", "Sebeke Trafosu:"},
        {"FAC_PARAM_PRICE", "Elektrik Birim Fiyati:"},
        {"FAC_PARAM_COST", "Tesis Bedeli:"},
        
        {"FAC_TEXAS_NAME", "Teksas Madencilik Hangari"},
        {"FAC_TEXAS_COUNTRY", "Austin, ABD"},
        {"FAC_TEXAS_DESC", "Giris seviye kisisel sanayi hangari. Sicak iklim."},
        {"FAC_ICELAND_NAME", "Reykjavik Jeotermal Vadisi"},
        {"FAC_ICELAND_COUNTRY", "Reykjavik, Izlanda"},
        {"FAC_ICELAND_DESC", "Volkanik jeotermal santral yaninda dogal buzul sogutmasi."},
        {"FAC_NORWAY_NAME", "Tromso Fiyort Sualti Tesisi"},
        {"FAC_NORWAY_COUNTRY", "Tromso, Norvec"},
        {"FAC_NORWAY_DESC", "Kutup cemberinde fiyort suyu ile dogal sirkulasyon."},
        {"FAC_GERMANY_NAME", "Frankfurt Endustriyel Veri Parki"},
        {"FAC_GERMANY_COUNTRY", "Frankfurt, Almanya"},
        {"FAC_GERMANY_DESC", "Avrupa finans merkezinde yuksek guvenlikli sanayi trafosu."},
        {"FAC_SIBERIA_NAME", "Sibirya Kutup Madencilik Ussu"},
        {"FAC_SIBERIA_COUNTRY", "Novosibirsk, Rusya"},
        {"FAC_SIBERIA_DESC", "Dondurucu kutup ikliminde devasa hidroelektrik hatti."},
        
        {"BTN_UPGRADE_PSU", "PSU'YU YUKSELT"},
        {"PSU_MAX_REACHED", "PSU MAKSIMUMDA"},
        {"BADGE_ROOM_TEMP", "ODA ISISI"},
        {"BTN_SAVE", "KAYDET"},
        {"BTN_SAVE_SUB", "F5 / Hizli Kayit"}
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
        {"TIP_FOOTER", "[TIP] Click cards for 360 inspection, press ESC to exit fullscreen or modals!"},
        
        {"MARKET_TITLE", "HARDWARE & FACILITY MARKET"},
        {"MARKET_TAB_GPUS", "GRAPHICS CARDS"},
        {"MARKET_TAB_POWER", "POWER & GRID"},
        {"MARKET_TAB_COOLING", "COOLING SYSTEMS"},
        {"MARKET_TAB_FACILITIES", "FACILITIES"},
        {"MARKET_ACTIVE_RIG", "Target: "},
        {"MARKET_RIG_FULL", "RIG FULL!"},
        {"MARKET_NO_MONEY", "INSUFFICIENT FUNDS"},
        {"MARKET_BUY", "BUY"},
        {"MARKET_INSTALLED", "INSTALLED (ACTIVE)"},
        {"MARKET_HASH_BAR", "Hash"},
        {"MARKET_POWER_BAR", "Power"},
        {"MARKET_EFF_BAR", "Eff"},
        {"TIER_ENTRY", "Entry Tier"},
        {"TIER_MID", "Mid-Range Value"},
        {"TIER_HIGH", "High Performance"},
        {"TIER_ULTRA", "Flagship Ultra"},
        {"TIER_ASIC", "Industrial ASIC"},
        {"POWER_TIER_1", "Standard Panel (3,600W)"},
        {"POWER_TIER_2", "Industrial 3-Phase (7,500W)"},
        {"POWER_TIER_3", "Dedicated Substation (15,000W)"},
        {"POWER_TIER_4", "High-Voltage Line (30,000W)"},
        {"FACILITY_SOLAR_1", "Solar Array Package 1"},
        {"FACILITY_SOLAR_1_DESC", "+1,000W Continuous Green Power"},
        {"FACILITY_SOLAR_2", "Solar Array Package 2"},
        {"FACILITY_SOLAR_2_DESC", "+2,500W Industrial Solar Farm"},
        {"FACILITY_FIRE_EXT", "Automated Fire Extinguisher"},
        {"FACILITY_FIRE_EXT_DESC", "Safeguards cards against 140C burnout"},
        
        {"BTN_TASKS", "MISSIONS"},
        {"TASK_MODAL_TITLE", "MISSION & TASK HUB"},
        {"TASK_READY_BADGE", "REWARDS READY"},
        {"TASK_REWARD_PREFIX", "Reward:"},
        {"TASK_BTN_CLAIM", "CLAIM"},
        {"TASK_BTN_CLAIMED", "CLAIMED"},
        {"TASK_BTN_IN_PROGRESS", "IN PROGRESS"},
        
        {"TASK_TITLE_INSPECT", "Hardware Specialist"},
        {"TASK_DESC_INSPECT", "Click any GPU to open 360 Inspection panel"},
        {"TASK_TITLE_OVERCLOCK", "Overclocking"},
        {"TASK_DESC_OVERCLOCK", "Apply Overclock to any graphics card"},
        {"TASK_TITLE_BUY_GPU", "Hardware Shopping"},
        {"TASK_DESC_BUY_GPU", "Purchase at least 1 GPU from the market"},
        {"TASK_TITLE_HASHRATE", "Mining Power"},
        {"TASK_DESC_HASHRATE", "Reach total hashrate above 150 MH/s"},
        {"TASK_TITLE_SELL_CRYPTO", "Crypto Trader"},
        {"TASK_DESC_SELL_CRYPTO", "Sell at least $500 worth of crypto on the exchange"},
        {"TASK_TITLE_MULTI_RIG", "Warehouse Expansion"},
        {"TASK_DESC_MULTI_RIG", "Expand warehouse to have 2 or more mining rigs"},
        {"TASK_TITLE_COOLING", "Thermal Control"},
        {"TASK_DESC_COOLING", "Upgrade facility cooling capacity to 3,000W or higher"},
        {"TASK_TITLE_POWER", "Industrial Power"},
        {"TASK_DESC_POWER", "Upgrade power panel capacity to 7,500W or higher"},
        {"TASK_TITLE_SOLAR", "Green Energy"},
        {"TASK_DESC_SOLAR", "Install at least 1,000W solar panel array"},
        
        {"BTN_WORLD_MAP", "WORLD MAP"},
        {"WORLD_MAP_TITLE", "GLOBAL MINING FACILITIES & WORLD MAP"},
        {"FAC_OWNED", "OWNED"},
        {"FAC_ALREADY_PURCHASED", "PURCHASED"},
        {"FAC_BADGE_ACTIVE", "ACTIVE SITE"},
        {"FAC_BADGE_OWNED", "PURCHASED"},
        {"FAC_BADGE_FOR_SALE", "FOR SALE"},
        {"FAC_BTN_ACTIVE", "CURRENT LOCATION"},
        {"FAC_BTN_SWITCH", "SWITCH TO SITE"},
        {"FAC_BTN_BUY", "BUY FACILITY"},
        {"FAC_PARAM_CLIMATE", "Climate Temp:"},
        {"FAC_PARAM_CAPACITY", "Max Rig Capacity:"},
        {"FAC_PARAM_GRID", "Power Substation:"},
        {"FAC_PARAM_PRICE", "Electricity Rate:"},
        {"FAC_PARAM_COST", "Facility Cost:"},
        
        {"FAC_TEXAS_NAME", "Texas Mining Hangar"},
        {"FAC_TEXAS_COUNTRY", "Austin, USA"},
        {"FAC_TEXAS_DESC", "Entry industrial hangar in warm climate."},
        {"FAC_ICELAND_NAME", "Reykjavik Geothermal Valley"},
        {"FAC_ICELAND_COUNTRY", "Reykjavik, Iceland"},
        {"FAC_ICELAND_DESC", "Natural glacier cooling next to geothermal plant."},
        {"FAC_NORWAY_NAME", "Tromso Fjord Underwater Site"},
        {"FAC_NORWAY_COUNTRY", "Tromso, Norway"},
        {"FAC_NORWAY_DESC", "Arctic circle natural fjord cooling circulation."},
        {"FAC_GERMANY_NAME", "Frankfurt Industrial Data Park"},
        {"FAC_GERMANY_COUNTRY", "Frankfurt, Germany"},
        {"FAC_GERMANY_DESC", "High-security industrial grid in European hub."},
        {"FAC_SIBERIA_NAME", "Siberian Arctic Mining Base"},
        {"FAC_SIBERIA_COUNTRY", "Novosibirsk, Russia"},
        {"FAC_SIBERIA_DESC", "Freezing arctic climate with massive hydro grid."},
        
        {"BTN_UPGRADE_PSU", "UPGRADE PSU"},
        {"PSU_MAX_REACHED", "MAX PSU REACHED"},
        {"BADGE_ROOM_TEMP", "ROOM TEMP"},
        {"BTN_SAVE", "SAVE"},
        {"BTN_SAVE_SUB", "F5 / Quick Save"}
    };
}

} // namespace Core
