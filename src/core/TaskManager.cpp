#include "TaskManager.hpp"
#include "Warehouse.hpp"
#include "EconomyManager.hpp"
#include "CoolingManager.hpp"
#include "PowerGrid.hpp"
#include "FacilityManager.hpp"
#include <algorithm>

namespace Core {

TaskManager::TaskManager() {
    // 75+ Dengeli ve Kapsamlı Görev Havuzu (Multi-Tier Balanced Quests)
    m_tasks = {
        // =========================================================================
        // 1. GELİR & NAKİT SERVET KADEMELERİ (CASH MILESTONES)
        // =========================================================================
        {
            "TASK_CASH_1000", "Ilk Bin Dolar", "Kasadaki nakit bakiyeni $1,000 seviyesine cikar.",
            TaskCategory::CASH, "", 1000.0, 0.0, 100.0, "", 0.0, false, false, true
        },
        {
            "TASK_CASH_2500", "Birikimci Madenci", "Kasadaki nakit bakiyeni $2,500 seviyesine cikar.",
            TaskCategory::CASH, "", 2500.0, 0.0, 250.0, "", 0.0, false, false, true
        },
        {
            "TASK_CASH_5000", "Sermaye Genislemesi", "Kasadaki nakit bakiyeni $5,000 seviyesine ulastir.",
            TaskCategory::CASH, "", 5000.0, 0.0, 500.0, "", 0.0, false, false, true
        },
        {
            "TASK_CASH_10000", "On Binler Kulubu", "Kasadaki nakit bakiyeni $10,000 seviyesine ulastir.",
            TaskCategory::CASH, "", 10000.0, 0.0, 1000.0, "", 0.0, false, false, true
        },
        {
            "TASK_CASH_25000", "Kripto Yatirimcisi", "Kasadaki nakit bakiyeni $25,000 seviyesine ulastir.",
            TaskCategory::CASH, "", 25000.0, 0.0, 2500.0, "", 0.0, false, false, true
        },
        {
            "TASK_CASH_50000", "Agirlikli Sermaye", "Kasadaki nakit bakiyeni $50,000 seviyesine ulastir.",
            TaskCategory::CASH, "", 50000.0, 0.0, 5000.0, "", 0.0, false, false, true
        },
        {
            "TASK_CASH_100000", "Yuz Bin Dolar Hazine", "Kasadaki nakit bakiyeni $100,000 seviyesine ulastir.",
            TaskCategory::CASH, "", 100000.0, 0.0, 10000.0, "", 0.0, false, false, true
        },
        {
            "TASK_CASH_250000", "Ceyrek Milyon", "Kasadaki nakit bakiyeni $250,000 seviyesine ulastir.",
            TaskCategory::CASH, "", 250000.0, 0.0, 25000.0, "", 0.0, false, false, true
        },
        {
            "TASK_CASH_500000", "Yarim Milyoner", "Kasadaki nakit bakiyeni $500,000 seviyesine ulastir.",
            TaskCategory::CASH, "", 500000.0, 0.0, 50000.0, "", 0.0, false, false, true
        },
        {
            "TASK_CASH_1000000", "Milyoner Madenci", "Kasadaki nakit bakiyeni tam $1,000,000 seviyesine ulastir!",
            TaskCategory::CASH, "", 1000000.0, 0.0, 100000.0, "BTC", 1.0, false, false, true
        },

        // =========================================================================
        // 2. KRİPTO MADENCİLİK & ÇAPRAZ COIN ÖDÜLLERİ (CRYPTO CROSS-REWARDS)
        // =========================================================================
        {
            "TASK_MINE_TEX_100", "Yerel Cevher (TEX)", "Cuzdaninda en az 100 TEX biriktir.",
            TaskCategory::CRYPTO, "", 100.0, 0.0, 500.0, "", 0.0, false, false, true
        },
        {
            "TASK_MINE_TEX_500", "TexCoin Koleksiyoneri", "Cuzdaninda en az 500 TEX biriktir.",
            TaskCategory::CRYPTO, "", 500.0, 0.0, 250.0, "RVN", 10.0, false, false, true
        },
        {
            "TASK_MINE_TEX_2500", "TexCoin Deposu", "Cuzdaninda en az 2,500 TEX biriktir.",
            TaskCategory::CRYPTO, "", 2500.0, 0.0, 200.0, "RVN", 100.0, false, false, true
        },
        {
            "TASK_MINE_TEX_10000", "TexCoin Hazinesi", "Cuzdaninda en az 10,000 TEX biriktir.",
            TaskCategory::CRYPTO, "", 10000.0, 0.0, 500.0, "ETC", 1.0, false, false, true
        },
        {
            "TASK_MINE_TEX_50000", "TexCoin Imparatoru", "Cuzdaninda en az 50,000 TEX biriktir.",
            TaskCategory::CRYPTO, "", 50000.0, 0.0, 2000.0, "ETC", 5.0, false, false, true
        },
        {
            "TASK_MINE_RVN_1000", "Kuzgun Madenciligi", "Cuzdaninda en az 1,000 RVN biriktir.",
            TaskCategory::CRYPTO, "", 1000.0, 0.0, 650.0, "", 0.0, false, false, true
        },
        {
            "TASK_MINE_RVN_5000", "Ravencoin Surusu", "Cuzdaninda en az 5,000 RVN biriktir.",
            TaskCategory::CRYPTO, "", 5000.0, 0.0, 300.0, "TEX", 50.0, false, false, true
        },
        {
            "TASK_MINE_RVN_25000", "Ravencoin Lordu", "Cuzdaninda en az 25,000 RVN biriktir.",
            TaskCategory::CRYPTO, "", 25000.0, 0.0, 600.0, "ETC", 2.0, false, false, true
        },
        {
            "TASK_MINE_ETC_1", "Klasik Vizyon (ETC)", "Cuzdaninda en az 1 ETC biriktir.",
            TaskCategory::CRYPTO, "", 1.0, 0.0, 600.0, "", 0.0, false, false, true
        },
        {
            "TASK_MINE_ETC_5", "Ethereum Classic Ustasi", "Cuzdaninda en az 5 ETC biriktir.",
            TaskCategory::CRYPTO, "", 5.0, 0.0, 400.0, "ETHW", 1.0, false, false, true
        },
        {
            "TASK_MINE_ETC_20", "Ethereum Classic Balinasi", "Cuzdaninda en az 20 ETC biriktir.",
            TaskCategory::CRYPTO, "", 20.0, 0.0, 1000.0, "ETHW", 5.0, false, false, true
        },
        {
            "TASK_MINE_ETHW_1", "Is Kaniti Ussu (ETHW)", "Cuzdaninda en az 1 ETHW biriktir.",
            TaskCategory::CRYPTO, "", 1.0, 0.0, 2500.0, "", 0.0, false, false, true
        },
        {
            "TASK_MINE_ETHW_5", "ETHW Rezervi", "Cuzdaninda en az 5 ETHW biriktir.",
            TaskCategory::CRYPTO, "", 5.0, 0.0, 1500.0, "SOL", 5.0, false, false, true
        },
        {
            "TASK_MINE_ETHW_25", "ETHW Devleti", "Cuzdaninda en az 25 ETHW biriktir.",
            TaskCategory::CRYPTO, "", 25.0, 0.0, 5000.0, "BTC", 0.25, false, false, true
        },
        {
            "TASK_MINE_XMR_1", "CPU Gizlilik (Monero)", "Islemcinle kazarak cuzdaninda 1 XMR biriktir.",
            TaskCategory::CRYPTO, "", 1.0, 0.0, 150.0, "", 0.0, false, false, true
        },
        {
            "TASK_MINE_XMR_5", "Monero RandomX Gucu", "Cuzdaninda en az 5 XMR biriktir.",
            TaskCategory::CRYPTO, "", 5.0, 0.0, 350.0, "SOL", 2.0, false, false, true
        },
        {
            "TASK_MINE_XMR_25", "Karanlik Ag Madencisi", "Cuzdaninda en az 25 XMR biriktir.",
            TaskCategory::CRYPTO, "", 25.0, 0.0, 1200.0, "SOL", 10.0, false, false, true
        },
        {
            "TASK_MINE_XMR_100", "Monero Karteli", "Cuzdaninda en az 100 XMR biriktir.",
            TaskCategory::CRYPTO, "", 100.0, 0.0, 5000.0, "BTC", 0.5, false, false, true
        },
        {
            "TASK_MINE_RTM_1000", "GhostRider L3 Kazimi", "Islemcinle en az 1,000 RTM kaz.",
            TaskCategory::CRYPTO, "", 1000.0, 0.0, 100.0, "", 0.0, false, false, true
        },
        {
            "TASK_MINE_RTM_10000", "Raptoreum Koleksiyonu", "Cuzdaninda en az 10,000 RTM biriktir.",
            TaskCategory::CRYPTO, "", 10000.0, 0.0, 200.0, "TEX", 100.0, false, false, true
        },
        {
            "TASK_MINE_RTM_50000", "Raptoreum Fabrikasi", "Cuzdaninda en az 50,000 RTM biriktir.",
            TaskCategory::CRYPTO, "", 50000.0, 0.0, 500.0, "ETC", 1.0, false, false, true
        },
        {
            "TASK_MINE_BTC_01", "Satoshi Mirasi (0.1 BTC)", "Cuzdaninda en az 0.1 BTC biriktir.",
            TaskCategory::CRYPTO, "", 0.1, 0.0, 500.0, "SOL", 2.0, false, false, true
        },
        {
            "TASK_MINE_BTC_05", "Yarim Bitcoin", "Cuzdaninda en az 0.5 BTC biriktir.",
            TaskCategory::CRYPTO, "", 0.5, 0.0, 2000.0, "SOL", 5.0, false, false, true
        },
        {
            "TASK_MINE_BTC_1", "1 Tam Bitcoin & 10 Solana!", "Cuzdaninda tam 1.0 BTC biriktir; aninda 10 SOL kazan!",
            TaskCategory::CRYPTO, "", 1.0, 0.0, 1000.0, "SOL", 10.0, false, false, true
        },
        {
            "TASK_MINE_BTC_2", "Bitcoin Ciftligi", "Cuzdaninda en az 2.0 BTC biriktir.",
            TaskCategory::CRYPTO, "", 2.0, 0.0, 5000.0, "SOL", 25.0, false, false, true
        },
        {
            "TASK_HOLD_SOL_10", "Solana Hizi (10 SOL)", "Cuzdaninda en az 10 SOL biriktir.",
            TaskCategory::CRYPTO, "", 10.0, 0.0, 300.0, "BTC", 0.05, false, false, true
        },
        {
            "TASK_HOLD_SOL_50", "Solana Ekosistemi (50 SOL)", "Cuzdaninda en az 50 SOL biriktir.",
            TaskCategory::CRYPTO, "", 50.0, 0.0, 1500.0, "BTC", 0.25, false, false, true
        },

        // =========================================================================
        // 3. FABRİKA & TESİS TABANLI DİNAMİK GÖREVLER (FACILITY OPERATIONS)
        // =========================================================================
        {
            "TASK_TEXAS_RIGS_3", "Teksas Genislemesi", "Teksas hangarinda en az 3 rig calistir.",
            TaskCategory::FACILITY, "", 3.0, 0.0, 600.0, "TEX", 30.0, false, false, true
        },
        {
            "TASK_TEXAS_CAP_FULL", "Teksas Tam Kapasite", "Teksas hangarindaki tum 5 rig yuvasini doldur.",
            TaskCategory::FACILITY, "", 5.0, 0.0, 1500.0, "RVN", 250.0, false, false, true
        },
        {
            "TASK_BUY_ICELAND", "Izlanda Fabrikasini Satin Al", "Dunya haritasindan Reykjavik Jeotermal Tesisi'ni satin al.",
            TaskCategory::FACILITY, "", 1.0, 0.0, 1500.0, "XMR", 2.0, false, false, true
        },
        {
            "TASK_ICELAND_RIGS_3", "Izlanda Jeotermal Ussu", "Izlanda tesisinde en az 3 aktif rig kur.",
            TaskCategory::FACILITY, "FAC_ICELAND", 3.0, 0.0, 1200.0, "XMR", 3.0, false, false, false
        },
        {
            "TASK_ICELAND_HASH_400", "Buz Vadisi Gucu", "Izlanda tesisinde 400 MH/s kazim gucune ulas.",
            TaskCategory::FACILITY, "FAC_ICELAND", 400.0, 0.0, 2000.0, "SOL", 4.0, false, false, false
        },
        {
            "TASK_ICELAND_CAP_FULL", "Reykjavik Jeotermal Zirvesi", "Izlanda tesisinin tum 8 rig yuvasini doldur.",
            TaskCategory::FACILITY, "FAC_ICELAND", 8.0, 0.0, 4500.0, "SOL", 8.0, false, false, false
        },
        {
            "TASK_BUY_NORWAY", "Norvec Fabrikasini Satin Al", "Tromso Fiyort Sualti Veri Parki'ni satin al.",
            TaskCategory::FACILITY, "", 1.0, 0.0, 3000.0, "SOL", 5.0, false, false, true
        },
        {
            "TASK_NORWAY_RIGS_5", "Fiyort Sualti Filosu", "Norvec tesisinde en az 5 aktif rig kur.",
            TaskCategory::FACILITY, "FAC_NORWAY", 5.0, 0.0, 2500.0, "SOL", 5.0, false, false, false
        },
        {
            "TASK_NORWAY_HASH_800", "Kuzey Isiklari Madenciligi", "Norvec tesisinde 800 MH/s kazim gucune ulas.",
            TaskCategory::FACILITY, "FAC_NORWAY", 800.0, 0.0, 4000.0, "ETHW", 2.0, false, false, false
        },
        {
            "TASK_NORWAY_CAP_FULL", "Tromso Kutup Kalesi", "Norvec tesisinin tum 14 rig yuvasini doldur.",
            TaskCategory::FACILITY, "FAC_NORWAY", 14.0, 0.0, 8000.0, "BTC", 0.15, false, false, false
        },
        {
            "TASK_BUY_GERMANY", "Almanya Fabrikasini Satin Al", "Frankfurt Endustriyel Veri Parki'ni satin al.",
            TaskCategory::FACILITY, "", 1.0, 0.0, 6000.0, "BTC", 0.1, false, false, true
        },
        {
            "TASK_GERMANY_RIGS_10", "Alman Endustri Standarti", "Almanya tesisinde en az 10 aktif rig kur.",
            TaskCategory::FACILITY, "FAC_GERMANY", 10.0, 0.0, 5000.0, "BTC", 0.1, false, false, false
        },
        {
            "TASK_GERMANY_HASH_1500", "Frankfurt Veri Hatti", "Almanya tesisinde 1,500 MH/s kazim gucune ulas.",
            TaskCategory::FACILITY, "FAC_GERMANY", 1500.0, 0.0, 9000.0, "SOL", 15.0, false, false, false
        },
        {
            "TASK_GERMANY_CAP_FULL", "Avrupa Hub Krali", "Almanya tesisinin tum 20 rig yuvasini doldur.",
            TaskCategory::FACILITY, "FAC_GERMANY", 20.0, 0.0, 15000.0, "BTC", 0.3, false, false, false
        },
        {
            "TASK_BUY_SIBERIA", "Sibirya Fabrikasini Satin Al", "Sibirya Kutup Madencilik Ussu'nu satin al.",
            TaskCategory::FACILITY, "", 1.0, 0.0, 15000.0, "SOL", 25.0, false, false, true
        },
        {
            "TASK_SIBERIA_RIGS_15", "Kutup Sondurucu Filo", "Sibirya tesisinde en az 15 aktif rig kur.",
            TaskCategory::FACILITY, "FAC_SIBERIA", 15.0, 0.0, 12000.0, "BTC", 0.2, false, false, false
        },
        {
            "TASK_SIBERIA_HASH_3000", "Sibirya Tipi Gigahash", "Sibirya tesisinde 3,000 MH/s (3 GH/s) gucune ulas.",
            TaskCategory::FACILITY, "FAC_SIBERIA", 3000.0, 0.0, 20000.0, "BTC", 0.35, false, false, false
        },
        {
            "TASK_SIBERIA_CAP_FULL", "Kutup Zirvesi (30 Rig)", "Sibirya tesisinin tum 30 rig yuvasini doldur!",
            TaskCategory::FACILITY, "FAC_SIBERIA", 30.0, 0.0, 35000.0, "BTC", 0.6, false, false, false
        },
        {
            "TASK_ALL_FACILITIES", "Kuresel Madencilik Imparatoru", "Dunya haritasindaki tum 5 tesisi satin al!",
            TaskCategory::FACILITY, "", 5.0, 0.0, 50000.0, "BTC", 1.0, false, false, true
        },

        // =========================================================================
        // 4. DONANIM, RİG ŞASİSİ, CPU VE GPU (HARDWARE)
        // =========================================================================
        {
            "TASK_INSPECT", "Donanim Uzmani", "Kasa icerisindeki herhangi bir GPU'ya tiklayarak detayli incele.",
            TaskCategory::HARDWARE, "", 1.0, 0.0, 200.0, "", 0.0, false, false, true
        },
        {
            "TASK_OVERCLOCK", "Hiz Asirtma (+%5 OC)", "Sag panelden rig kartlarina hiz asirtma uygula.",
            TaskCategory::HARDWARE, "", 1.0, 0.0, 350.0, "", 0.0, false, false, true
        },
        {
            "TASK_BUY_GPU", "Pazar Alisverisi", "Markete gidip ilk yeni ekran kartini satin al.",
            TaskCategory::HARDWARE, "", 1.0, 0.0, 400.0, "", 0.0, false, false, true
        },
        {
            "TASK_BUY_GPU_5", "Ekran Karti Koleksiyoneri", "Toplamda en az 5 adet GPU satin al.",
            TaskCategory::HARDWARE, "", 5.0, 0.0, 1000.0, "TEX", 50.0, false, false, true
        },
        {
            "TASK_BUY_GPU_10", "Donanim Fabrikatori", "Toplamda en az 10 adet GPU satin al.",
            TaskCategory::HARDWARE, "", 10.0, 0.0, 2500.0, "RVN", 200.0, false, false, true
        },
        {
            "TASK_BUY_GPU_25", "Donanim Deposu (25 GPU)", "Toplamda en az 25 adet GPU satin al.",
            TaskCategory::HARDWARE, "", 25.0, 0.0, 6000.0, "ETC", 2.0, false, false, true
        },
        {
            "TASK_BUY_GPU_50", "Endustriyel GPU Ciftligi (50)", "Toplamda en az 50 adet GPU satin al.",
            TaskCategory::HARDWARE, "", 50.0, 0.0, 15000.0, "SOL", 10.0, false, false, true
        },
        {
            "TASK_FRAME_TIER2", "Aluminyum Kasa (Tier 2)", "Kasa sasini Tier 2 Aluminyum Acik Kasaya (4 Slot) yukselt.",
            TaskCategory::HARDWARE, "", 2.0, 0.0, 150.0, "TEX", 25.0, false, false, true
        },
        {
            "TASK_FRAME_TIER3", "Celik Pro Miner (Tier 3)", "Kasa sasini Tier 3 Celik Pro Miner'a (6 Slot) yukselt.",
            TaskCategory::HARDWARE, "", 3.0, 0.0, 350.0, "TEX", 50.0, false, false, true
        },
        {
            "TASK_FRAME_TIER4", "Server Kabineti (Tier 4)", "Kasa sasini Tier 4 4U Server Kabinetine (8 Slot) yukselt.",
            TaskCategory::HARDWARE, "", 4.0, 0.0, 800.0, "ETC", 1.0, false, false, true
        },
        {
            "TASK_FRAME_TIER5", "Daldırma Tanki (Tier 5)", "Kasa sasini Tier 5 Kriyojenik Daldırma Tankina (10 Slot) yukselt!",
            TaskCategory::HARDWARE, "", 5.0, 0.0, 1500.0, "ETHW", 1.0, false, false, true
        },
        {
            "TASK_INSTALL_CPU", "Islemci Montaji", "Marketten yeni bir CPU satin al ve kasana tak.",
            TaskCategory::HARDWARE, "", 1.0, 0.0, 200.0, "XMR", 1.0, false, false, true
        },
        {
            "TASK_CPU_POWER", "Pro Islemci Gucu", "En az 25 KH/s gucunde bir CPU (Ryzen 9 veya ustu) tak.",
            TaskCategory::HARDWARE, "", 25.0, 0.0, 600.0, "XMR", 5.0, false, false, true
        },
        {
            "TASK_CPU_SERVER", "Sunucu Sinifi Islemci", "En az 80 KH/s gucunde bir CPU (Threadripper / EPYC) tak.",
            TaskCategory::HARDWARE, "", 80.0, 0.0, 2500.0, "XMR", 15.0, false, false, true
        },
        {
            "TASK_HASHRATE_50", "Cirak Madenci (50 MH/s)", "Tesisinde toplam 50 MH/s kazim gucune ulas.",
            TaskCategory::HARDWARE, "", 50.0, 0.0, 350.0, "", 0.0, false, false, true
        },
        {
            "TASK_HASHRATE_150", "Kalfa Madenci (150 MH/s)", "Tesisinde toplam 150 MH/s kazim gucune ulas.",
            TaskCategory::HARDWARE, "", 150.0, 0.0, 800.0, "", 0.0, false, false, true
        },
        {
            "TASK_HASHRATE_500", "Usta Madenci (500 MH/s)", "Tesisinde toplam 500 MH/s kazim gucune ulas.",
            TaskCategory::HARDWARE, "", 500.0, 0.0, 2500.0, "TEX", 100.0, false, false, true
        },
        {
            "TASK_HASHRATE_1000", "Gigahash Bariyeri (1 GH/s)", "Tesisinde toplam 1,000 MH/s (1 GH/s) gucune ulas.",
            TaskCategory::HARDWARE, "", 1000.0, 0.0, 6000.0, "SOL", 5.0, false, false, true
        },
        {
            "TASK_COOLING_3000", "Termal Kontrol", "Sogutma kapasitesini 3,000 Watt seviyesine yukselt.",
            TaskCategory::HARDWARE, "", 3000.0, 0.0, 800.0, "", 0.0, false, false, true
        },
        {
            "TASK_COOLING_6000", "Maksimum Daldırma Sogutma", "Sogutmayi 6,000 Watt Daldırma Sivi kademesine cikar.",
            TaskCategory::HARDWARE, "", 6000.0, 0.0, 2500.0, "SOL", 3.0, false, false, true
        },
        {
            "TASK_POWER_7500", "Sanayi Trafosu", "Trafo guvenli guc kapasitesini 7,500 Watt seviyesine cikar.",
            TaskCategory::HARDWARE, "", 7500.0, 0.0, 1000.0, "", 0.0, false, false, true
        },
        {
            "TASK_POWER_30000", "Agır Sanayi Sebekesi", "Trafo guvenli guc kapasitesini 30,000 Watt seviyesine cikar.",
            TaskCategory::HARDWARE, "", 30000.0, 0.0, 3000.0, "SOL", 4.0, false, false, true
        },
        {
            "TASK_SOLAR_1000", "Yesil Enerji (1 kW)", "En az 1,000 Watt gunes paneli kur.",
            TaskCategory::HARDWARE, "", 1000.0, 0.0, 1500.0, "", 0.0, false, false, true
        },

        // =========================================================================
        // 5. BORSA TİCARETİ, HABERLER VE PORTFÖY (TRADING)
        // =========================================================================
        {
            "TASK_FIRST_TRADE", "Ilk Kripto Takasi", "Kripto borsasinda ilk coin alim-satimini yap.",
            TaskCategory::TRADING, "", 1.0, 0.0, 300.0, "", 0.0, false, false, true
        },
        {
            "TASK_TRADES_10", "Aktif Trader (10 Islem)", "Borsada toplam 10 alim-satim islemi gerceklestir.",
            TaskCategory::TRADING, "", 10.0, 0.0, 800.0, "TEX", 50.0, false, false, true
        },
        {
            "TASK_TRADES_50", "Piyasa Yapici (50 Islem)", "Borsada toplam 50 alim-satim islemi gerceklestir.",
            TaskCategory::TRADING, "", 50.0, 0.0, 3000.0, "SOL", 5.0, false, false, true
        },
        {
            "TASK_READ_NEWS", "Haber Taktigi (3 Haber)", "Kripto haber panelinden en az 3 haber oku.",
            TaskCategory::TRADING, "", 3.0, 0.0, 250.0, "", 0.0, false, false, true
        },
        {
            "TASK_READ_NEWS_10", "Piyasa Analisti (10 Haber)", "Kripto haber panelinden en az 10 haber oku.",
            TaskCategory::TRADING, "", 10.0, 0.0, 700.0, "TEX", 60.0, false, false, true
        },
        {
            "TASK_SELL_CRYPTO", "Kripto Tuccari ($500)", "Borsada toplam en az $500 degerinde kripto satisi yap.",
            TaskCategory::TRADING, "", 500.0, 0.0, 450.0, "", 0.0, false, false, true
        },
        {
            "TASK_SELL_CRYPTO_5000", "Buyuk Satis ($5,000)", "Borsada toplam en az $5,000 degerinde kripto satisi yap.",
            TaskCategory::TRADING, "", 5000.0, 0.0, 1000.0, "SOL", 2.0, false, false, true
        },
        {
            "TASK_SELL_CRYPTO_25000", "Kripto Arbitraji ($25k)", "Borsada toplam en az $25,000 degerinde kripto satisi yap.",
            TaskCategory::TRADING, "", 25000.0, 0.0, 3500.0, "SOL", 8.0, false, false, true
        },
        {
            "TASK_SELL_CRYPTO_100000", "Balina Hareketi ($100k)", "Borsada toplam en az $100,000 degerinde kripto satisi yap.",
            TaskCategory::TRADING, "", 100000.0, 0.0, 12000.0, "BTC", 0.2, false, false, true
        },
        {
            "TASK_DIVERSIFY", "Cesitli Portfoy (3 Coin)", "Cuzdaninda ayni anda en az 3 farkli kripto para bulundur.",
            TaskCategory::TRADING, "", 3.0, 0.0, 1000.0, "", 0.0, false, false, true
        },
        {
            "TASK_DIVERSIFY_6", "Kripto Fonu (6 Coin)", "Cuzdaninda ayni anda en az 6 farkli kripto para bulundur.",
            TaskCategory::TRADING, "", 6.0, 0.0, 3000.0, "SOL", 5.0, false, false, true
        },
        {
            "TASK_PORTFOLIO_10K", "Kripto Balinasi ($10k)", "Toplam portfoy degerini $10,000 seviyesine ulastir.",
            TaskCategory::TRADING, "", 10000.0, 0.0, 2000.0, "", 0.0, false, false, true
        },
        {
            "TASK_PORTFOLIO_50K", "Kripto Fon Yoneticisi ($50k)", "Toplam portfoy degerini $50,000 seviyesine ulastir.",
            TaskCategory::TRADING, "", 50000.0, 0.0, 5000.0, "SOL", 5.0, false, false, true
        },
        {
            "TASK_PORTFOLIO_250K", "Hedge Fund ($250k)", "Toplam portfoy degerini $250,000 seviyesine ulastir.",
            TaskCategory::TRADING, "", 250000.0, 0.0, 15000.0, "SOL", 20.0, false, false, true
        },
        {
            "TASK_PORTFOLIO_1M", "Milyar Dolarlik Vizyon ($1M)", "Toplam portfoy degerini $1,000,000 seviyesine ulastir!",
            TaskCategory::TRADING, "", 1000000.0, 0.0, 50000.0, "BTC", 1.0, false, false, true
        }
    };
}

void TaskManager::NotifyCardInspected() {
    m_cardInspected = true;
}

void TaskManager::NotifyCardOverclocked() {
    m_cardOverclocked = true;
}

void TaskManager::NotifyCryptoSold(double usdAmount) {
    m_totalCryptoSoldUsd += usdAmount;
}

void TaskManager::NotifyGpuPurchased() {
    m_gpusPurchasedCount++;
}

void TaskManager::NotifyTradeExecuted() {
    m_tradesExecutedCount++;
}

void TaskManager::NotifyNewsRead() {
    m_newsReadCount++;
}

void TaskManager::NotifyRigFrameUpgraded() {
    m_rigFrameUpgradesCount++;
}

void TaskManager::NotifyCPUPurchased() {
    m_cpusPurchasedCount++;
}

void TaskManager::UpdateProgress(const Warehouse& warehouse,
                                const EconomyManager& economy,
                                const CoolingManager& cooling,
                                const PowerGrid& powerGrid,
                                const FacilityManager* facilityManager) {
    double totalCash = economy.GetFiatBalance();
    double totalPortfolio = economy.GetTotalPortfolioValueUSD();
    double warehouseHash = warehouse.CalculateTotalHashrate();
    double maxRigLevel = 1.0;
    double maxCpuHash = 0.0;
    for (const auto& r : warehouse.GetAllRigs()) {
        if (r) {
            maxRigLevel = std::max(maxRigLevel, static_cast<double>(r->GetRigLevel()));
            maxCpuHash = std::max(maxCpuHash, r->GetCPUHashrateKH());
        }
    }

    // Facility unlock checks
    bool icelandPurchased = false;
    bool norwayPurchased = false;
    bool germanyPurchased = false;
    bool siberiaPurchased = false;
    double totalFacilitiesPurchased = 1.0;

    const FacilityLocation* icelandFac = nullptr;
    const FacilityLocation* norwayFac = nullptr;
    const FacilityLocation* germanyFac = nullptr;
    const FacilityLocation* siberiaFac = nullptr;

    if (facilityManager) {
        double pCount = 0.0;
        for (const auto& fac : facilityManager->GetAllFacilities()) {
            if (fac.isPurchased) {
                pCount += 1.0;
                if (fac.id == "FAC_ICELAND") { icelandPurchased = true; icelandFac = &fac; }
                else if (fac.id == "FAC_NORWAY") { norwayPurchased = true; norwayFac = &fac; }
                else if (fac.id == "FAC_GERMANY") { germanyPurchased = true; germanyFac = &fac; }
                else if (fac.id == "FAC_SIBERIA") { siberiaPurchased = true; siberiaFac = &fac; }
            }
        }
        totalFacilitiesPurchased = pCount;
    }

    for (auto& task : m_tasks) {
        // Unlock state evaluation
        if (!task.requiredFacilityId.empty()) {
            if (task.requiredFacilityId == "FAC_ICELAND") task.isUnlocked = icelandPurchased;
            else if (task.requiredFacilityId == "FAC_NORWAY") task.isUnlocked = norwayPurchased;
            else if (task.requiredFacilityId == "FAC_GERMANY") task.isUnlocked = germanyPurchased;
            else if (task.requiredFacilityId == "FAC_SIBERIA") task.isUnlocked = siberiaPurchased;
        } else {
            task.isUnlocked = true;
        }

        if (task.isCompleted) continue;

        // Cash milestones
        if (task.id == "TASK_CASH_1000" || task.id == "TASK_CASH_2500" ||
            task.id == "TASK_CASH_5000" || task.id == "TASK_CASH_10000" ||
            task.id == "TASK_CASH_25000" || task.id == "TASK_CASH_50000" ||
            task.id == "TASK_CASH_100000" || task.id == "TASK_CASH_250000" ||
            task.id == "TASK_CASH_500000" || task.id == "TASK_CASH_1000000") {
            task.currentProgress = totalCash;
        }
        // Crypto coins
        else if (task.id == "TASK_MINE_TEX_100" || task.id == "TASK_MINE_TEX_500" ||
                 task.id == "TASK_MINE_TEX_2500" || task.id == "TASK_MINE_TEX_10000" ||
                 task.id == "TASK_MINE_TEX_50000") {
            const auto* c = economy.GetCoinById("TEX");
            task.currentProgress = c ? c->balance : 0.0;
        }
        else if (task.id == "TASK_MINE_RVN_1000" || task.id == "TASK_MINE_RVN_5000" ||
                 task.id == "TASK_MINE_RVN_25000") {
            const auto* c = economy.GetCoinById("RVN");
            task.currentProgress = c ? c->balance : 0.0;
        }
        else if (task.id == "TASK_MINE_ETC_1" || task.id == "TASK_MINE_ETC_5" ||
                 task.id == "TASK_MINE_ETC_20") {
            const auto* c = economy.GetCoinById("ETC");
            task.currentProgress = c ? c->balance : 0.0;
        }
        else if (task.id == "TASK_MINE_ETHW_1" || task.id == "TASK_MINE_ETHW_5" ||
                 task.id == "TASK_MINE_ETHW_25") {
            const auto* c = economy.GetCoinById("ETHW");
            task.currentProgress = c ? c->balance : 0.0;
        }
        else if (task.id == "TASK_MINE_XMR_1" || task.id == "TASK_MINE_XMR_5" ||
                 task.id == "TASK_MINE_XMR_25" || task.id == "TASK_MINE_XMR_100") {
            const auto* c = economy.GetCoinById("XMR");
            task.currentProgress = c ? c->balance : 0.0;
        }
        else if (task.id == "TASK_MINE_RTM_1000" || task.id == "TASK_MINE_RTM_10000" ||
                 task.id == "TASK_MINE_RTM_50000") {
            const auto* c = economy.GetCoinById("RTM");
            task.currentProgress = c ? c->balance : 0.0;
        }
        else if (task.id == "TASK_MINE_BTC_01" || task.id == "TASK_MINE_BTC_05" ||
                 task.id == "TASK_MINE_BTC_1" || task.id == "TASK_MINE_BTC_2") {
            const auto* c = economy.GetCoinById("BTC");
            task.currentProgress = c ? c->balance : 0.0;
        }
        else if (task.id == "TASK_HOLD_SOL_10" || task.id == "TASK_HOLD_SOL_50") {
            const auto* c = economy.GetCoinById("SOL");
            task.currentProgress = c ? c->balance : 0.0;
        }
        // Facility purchase tasks
        else if (task.id == "TASK_TEXAS_RIGS_3" || task.id == "TASK_TEXAS_CAP_FULL") {
            task.currentProgress = static_cast<double>(warehouse.GetRigCount());
        }
        else if (task.id == "TASK_BUY_ICELAND") {
            task.currentProgress = icelandPurchased ? 1.0 : 0.0;
        }
        else if (task.id == "TASK_ICELAND_RIGS_3" || task.id == "TASK_ICELAND_CAP_FULL") {
            task.currentProgress = (icelandFac && icelandFac->warehouse) ? static_cast<double>(icelandFac->warehouse->GetRigCount()) : 0.0;
        }
        else if (task.id == "TASK_ICELAND_HASH_400") {
            task.currentProgress = (icelandFac && icelandFac->warehouse) ? icelandFac->warehouse->CalculateTotalHashrate() : 0.0;
        }
        else if (task.id == "TASK_BUY_NORWAY") {
            task.currentProgress = norwayPurchased ? 1.0 : 0.0;
        }
        else if (task.id == "TASK_NORWAY_RIGS_5" || task.id == "TASK_NORWAY_CAP_FULL") {
            task.currentProgress = (norwayFac && norwayFac->warehouse) ? static_cast<double>(norwayFac->warehouse->GetRigCount()) : 0.0;
        }
        else if (task.id == "TASK_NORWAY_HASH_800") {
            task.currentProgress = (norwayFac && norwayFac->warehouse) ? norwayFac->warehouse->CalculateTotalHashrate() : 0.0;
        }
        else if (task.id == "TASK_BUY_GERMANY") {
            task.currentProgress = germanyPurchased ? 1.0 : 0.0;
        }
        else if (task.id == "TASK_GERMANY_RIGS_10" || task.id == "TASK_GERMANY_CAP_FULL") {
            task.currentProgress = (germanyFac && germanyFac->warehouse) ? static_cast<double>(germanyFac->warehouse->GetRigCount()) : 0.0;
        }
        else if (task.id == "TASK_GERMANY_HASH_1500") {
            task.currentProgress = (germanyFac && germanyFac->warehouse) ? germanyFac->warehouse->CalculateTotalHashrate() : 0.0;
        }
        else if (task.id == "TASK_BUY_SIBERIA") {
            task.currentProgress = siberiaPurchased ? 1.0 : 0.0;
        }
        else if (task.id == "TASK_SIBERIA_RIGS_15" || task.id == "TASK_SIBERIA_CAP_FULL") {
            task.currentProgress = (siberiaFac && siberiaFac->warehouse) ? static_cast<double>(siberiaFac->warehouse->GetRigCount()) : 0.0;
        }
        else if (task.id == "TASK_SIBERIA_HASH_3000") {
            task.currentProgress = (siberiaFac && siberiaFac->warehouse) ? siberiaFac->warehouse->CalculateTotalHashrate() : 0.0;
        }
        else if (task.id == "TASK_ALL_FACILITIES") {
            task.currentProgress = totalFacilitiesPurchased;
        }
        // Hardware & Rig Frames
        else if (task.id == "TASK_INSPECT") {
            task.currentProgress = m_cardInspected ? 1.0 : 0.0;
        }
        else if (task.id == "TASK_OVERCLOCK") {
            task.currentProgress = m_cardOverclocked ? 1.0 : 0.0;
        }
        else if (task.id == "TASK_BUY_GPU" || task.id == "TASK_BUY_GPU_5" ||
                 task.id == "TASK_BUY_GPU_10" || task.id == "TASK_BUY_GPU_25" ||
                 task.id == "TASK_BUY_GPU_50") {
            task.currentProgress = static_cast<double>(m_gpusPurchasedCount);
        }
        else if (task.id == "TASK_FRAME_TIER2" || task.id == "TASK_FRAME_TIER3" ||
                 task.id == "TASK_FRAME_TIER4" || task.id == "TASK_FRAME_TIER5") {
            task.currentProgress = maxRigLevel;
        }
        else if (task.id == "TASK_INSTALL_CPU") {
            task.currentProgress = static_cast<double>(m_cpusPurchasedCount);
        }
        else if (task.id == "TASK_CPU_POWER" || task.id == "TASK_CPU_SERVER") {
            task.currentProgress = maxCpuHash;
        }
        else if (task.id == "TASK_HASHRATE_50" || task.id == "TASK_HASHRATE_150" ||
                 task.id == "TASK_HASHRATE_500" || task.id == "TASK_HASHRATE_1000") {
            task.currentProgress = warehouseHash;
        }
        else if (task.id == "TASK_COOLING_3000" || task.id == "TASK_COOLING_6000") {
            task.currentProgress = cooling.CalculateTotalCoolingWatts();
        }
        else if (task.id == "TASK_POWER_7500" || task.id == "TASK_POWER_30000") {
            task.currentProgress = powerGrid.GetMaxCapacityWatts();
        }
        else if (task.id == "TASK_SOLAR_1000") {
            task.currentProgress = powerGrid.GetTotalProductionWatts();
        }
        // Trading & Portfolio
        else if (task.id == "TASK_FIRST_TRADE" || task.id == "TASK_TRADES_10" ||
                 task.id == "TASK_TRADES_50") {
            task.currentProgress = static_cast<double>(m_tradesExecutedCount);
        }
        else if (task.id == "TASK_READ_NEWS" || task.id == "TASK_READ_NEWS_10") {
            task.currentProgress = static_cast<double>(m_newsReadCount);
        }
        else if (task.id == "TASK_SELL_CRYPTO" || task.id == "TASK_SELL_CRYPTO_5000" ||
                 task.id == "TASK_SELL_CRYPTO_25000" || task.id == "TASK_SELL_CRYPTO_100000") {
            task.currentProgress = m_totalCryptoSoldUsd;
        }
        else if (task.id == "TASK_DIVERSIFY" || task.id == "TASK_DIVERSIFY_6") {
            double heldCount = 0.0;
            for (const auto& c : economy.GetCoins()) {
                if (c.balance > 0.0001) heldCount += 1.0;
            }
            task.currentProgress = heldCount;
        }
        else if (task.id == "TASK_PORTFOLIO_10K" || task.id == "TASK_PORTFOLIO_50K" ||
                 task.id == "TASK_PORTFOLIO_250K" || task.id == "TASK_PORTFOLIO_1M") {
            task.currentProgress = totalPortfolio;
        }

        if (task.currentProgress >= task.targetProgress) {
            task.isCompleted = true;
        }
    }
}

const std::vector<GameTask>& TaskManager::GetTasks() const {
    return m_tasks;
}

std::vector<size_t> TaskManager::GetTaskIndicesByCategory(TaskCategory cat) const {
    std::vector<size_t> indices;
    indices.reserve(m_tasks.size());
    for (size_t i = 0; i < m_tasks.size(); ++i) {
        if (cat == TaskCategory::ALL || m_tasks[i].category == cat) {
            indices.push_back(i);
        }
    }
    return indices;
}

size_t TaskManager::GetUnclaimedCompletedCount() const {
    size_t count = 0;
    for (const auto& task : m_tasks) {
        if (task.isCompleted && !task.isClaimed && task.isUnlocked) {
            count++;
        }
    }
    return count;
}

bool TaskManager::ClaimReward(const std::string& taskId, EconomyManager& economy) {
    for (auto& task : m_tasks) {
        if (task.id == taskId) {
            if (task.isCompleted && !task.isClaimed && task.isUnlocked) {
                task.isClaimed = true;
                if (task.rewardCash > 0.0) {
                    economy.AddFiat(task.rewardCash);
                }
                if (!task.rewardCryptoSymbol.empty() && task.rewardCryptoAmount > 0.0) {
                    economy.AddCoinBalance(task.rewardCryptoSymbol, task.rewardCryptoAmount);
                }
                return true;
            }
            return false;
        }
    }
    return false;
}

void TaskManager::SetTaskState(const std::string& taskId, bool completed, bool claimed, double progress) {
    for (auto& task : m_tasks) {
        if (task.id == taskId) {
            task.isCompleted = completed;
            task.isClaimed = claimed;
            task.currentProgress = progress;
            return;
        }
    }
}

} // namespace Core
