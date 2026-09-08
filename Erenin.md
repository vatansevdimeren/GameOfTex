# 🎮 Crypto Mining & Energy Tycoon - Mimari ve Kod Günlüğü (Erenin.md)

Bu dosya, projedeki her bir dosyanın, sınıfın ve fonksiyonun **Clean Code** ve **Single Responsibility Principle (SRP)** ilkelerine göre neden oluşturulduğunu, ne iş yaptığını ve sistemin nasıl çalıştığını adım adım belgeler.

---

## 🏛️ Mimari İlkelerimiz (Architecture Principles)

1. **Single Responsibility Principle (Tek Sorumluluk Prensibi):**
   * Her fonksiyon yalnızca ve yalnızca TEK bir iş yapar.
   * Örneğin: Buton sınıfı sadece fare etkileşimini ve buton çizimini bilir; parayı veya GPU'yu bilmez. GPU sınıfı sıcaklık hesaplamaz, sıcaklığı `ThermalModel` hesaplar.
2. **Modülerlik ve Dosya Ayrımı:**
   * Başlık (`.hpp`) ve kaynak (`.cpp`) dosyaları kesinlikle ayrıdır.
   * Çekirdek simülasyon (matematik) katmanı, grafik ve render katmanından tamamen bağımsızdır (Headless çalışabilir).
3. **Data-Driven ve Kullanıcı Odaklı Tasarım:**
   * Oyuncu profili, şirket ismi ve avatar seçimi bağımsız bir `UserProfile` nesnesiyle yönetilir.
4. **2D Tycoon / Glassmorphic Cyber UI:**
   * Düz metinler yerine şık cam efektli kartlar (`UIFrame`), fareye duyarlı canlı butonlar (`UIButton`) ve şebeke güç barları kullanılır.

---

## 📁 1. Çekirdek Simülasyon Katmanı (`src/core/`)

### 1.1. `UserProfile.hpp` & `UserProfile.cpp` (Oyuncu Profili ve Şirket Kimliği)
* **Görevi:** Oyuncunun madencilik şirketi adını, seçtiği avatarı ve hoş geldin bonusu alıp almadığını yönetir.

#### Fonksiyonlar ve Görevleri:
* `UserProfile(std::string companyName, int avatarIndex)`: Profil nesnesini oluşturur.
* `const std::string& GetCompanyName() const` & `void SetCompanyName(const std::string& name)`: Şirket adını okur ve günceller.
* `int GetAvatarIndex() const` & `void SetAvatarIndex(int index)`: Seçilen avatar rozet numarasını (0: Siber, 1: Sanayi, 2: Uzay) döner/ayarlar.
* `bool HasClaimedBonus() const` & `void SetClaimedBonus(bool claimed)`: 
  * **SRP Gerekçesi:** Oyuncunun +1,000$ başlangıç bonusunu alıp almadığını takip eder, parayı bizzat dağıtmaz.
* `bool SaveToFile(const std::string& filepath) const`: Profili yerel diske kaydeder.
* `bool LoadFromFile(const std::string& filepath)`: Diskteki profili okur.

---

### 1.2. `GPU.hpp` & `GPU.cpp` (Ekran Kartı Bileşeni)
* **Görevi:** Tek bir ekran kartının donanımsal durumunu tutar.
* `GetEffectiveHashrate()`: Overclock ve silikon kalitesini çarparak net kazım gücünü hesaplar.
* `GetEffectivePowerWatts()`: Hız aşırtma ile karesel artan güç tüketimini ($P \propto V^2$) hesaplar.
* `IsThrottled()` / `SetThrottled()`: Aşırı ısı durumundaki yavaşlatma bayrağını tutar.

---

### 1.3. `MiningRig.hpp` & `MiningRig.cpp` (Madencilik Rafı / Kasası)
* **Görevi:** Birden çok GPU'yu bir araya getiren açık çerçeve kasadır.
* `InstallGPU()`, `RemoveGPU()`: Kart yuvalarını yönetir.
* `CalculateTotalHashrate()`: Takılı kartların anlık toplam hashrate'ini hesaplar.
* `CalculateTotalPowerWatts()`: Takılı kartların ve anakartın toplam watt tüketimini toplar.

---

### 1.4. `ThermalModel.hpp` & `ThermalModel.cpp` (Termodinamik ve Soğutma)
* **Görevi:** Odanın ve donanımların sıcaklık değişimini fizik kurallarına göre çözer.
* `Update(double heatWatts, double dt)`: Odadaki net ısı birikimini ve sıcaklık diferansiyelini hesaplar ($Q = m \cdot c \cdot \Delta T$).
* `CalculateGPUTemperature(...)`: Fan devrine ve hava akış direncine göre kartın anlık çekirdek sıcaklığını verir.
* `IsOverheating(double temp)`: 85°C kritik eşiğini denetler.

---

### 1.5. `PowerGrid.hpp` & `PowerGrid.cpp` (Elektrik Şebekesi)
* **Görevi:** Toplam tüketim ile yeşil üretim arasındaki dengeyi ve sigorta limitini kontrol eder.
* `CalculateCostForDuration(double dt)`: kWh başına elektrik faturasını hesaplar.
* `IsBreakerTripped()` & `ResetBreaker()`: Aşırı yüklenmede sigortayı attırır ve şalterin kaldırılmasını sağlar.

---

### 1.6. `EconomyManager.hpp` & `EconomyManager.cpp` (Kripto Piyasası ve Cüzdan)
* **Görevi:** Oyuncunun nakit parasını, coin bakiyesini ve borsa fiyatını simüle eder.
* `MineCoins(...)`: Ağ zorluğuna göre üretilen coinleri cüzdana aktarır.
* `SellCrypto(...)`: Kripto paraları nakite dönüştürür.
* `AddFiat(...)` / `DeductFiat(...)`: Bakiye giriş çıkışlarını yönetir.

---

## 🎨 2. Görselleştirme ve Arayüz Katmanı (`src/render/`)

### 2.1. `UIButton.hpp` & `UIButton.cpp` (Modern 2D İnteraktif Buton)
* **Görevi:** Kullanıcı dostu, animasyonlu ve tıklanabilir 2D buton bileşenidir.

#### Fonksiyonlar ve Görevleri:
* `UIButton(Rectangle bounds, std::string title, std::string subtitle, Color baseColor, Color accentColor)`: Butonu oluşturur.
* `bool UpdateAndCheckClick()`: 
  * **Görevi:** Farenin buton üzerinde olup olmadığını (hover), sol tıkla basılıp basılmadığını denetler ve tıklama gerçekleştiğinde `true` döner.
  * **SRP Gerekçesi:** Yalnızca tıklanma olayını tespit eder; tıklanınca ne olacağını çağıran sisteme bırakır.
* `void SetDisabled(bool disabled)`: Bakiye yetersizse veya yuvalar doluysa butonu grileştirir/pasifleştirir.
* `void Draw() const`: Butonun camgöbeği neon kenarlıklarını, hover parlama efektini ve başlık/altlık metinlerini çizer.

---

### 2.2. `UIFrame.hpp` & `UIFrame.cpp` (Çerçeve, Panel ve Gösterge Çizici)
* **Görevi:** 2D Idle oyununun yapısal çerçevelerini (Card containers) ve sayaçlarını çizer.

#### Fonksiyonlar ve Görevleri:
* `DrawCard(Rectangle bounds, const std::string& title, Color accent)`: Koyu cam efektli, ince neon hatlı modern kart çerçevesini çizer.
* `DrawProgressBar(Rectangle bounds, float progressRatio, Color barColor, const std::string& label)`: Şebeke güç kullanımını gösteren akıcı ilerleme barını çizer.
* `DrawStatBadge(int x, int y, int width, int height, ...)`: Üst paneldeki bakiye, coin değeri ve sıcaklık rozetlerini çizer.
* `DrawTextInput(Rectangle bounds, const std::string& text, bool isActive, const std::string& placeholder)`: Metin girişi kutusunu ve yanıp sönen imleci çizer.

---

### 2.3. `LoginScreen.hpp` & `LoginScreen.cpp` (Giriş ve Profil Ekranı)
* **Görevi:** Oyun ilk açıldığında gösterilen sade ve şık profil oluşturma kartıdır.

#### Fonksiyonlar ve Görevleri:
* `bool Update(Core::UserProfile& profile)`: 
  * **Görevi:** Klavyeden şirket adı yazılmasını, avatar seçilmesini ve "HESAP AC (+1000$)" butonuna tıklanmasını yönetir.
  * **SRP Gerekçesi:** Sadece kullanıcı giriş bilgilerini toplar ve onaylandığında `true` döner.
* `void Draw() const`: Merkezdeki sade kartı, +$1,000 bonus rozetini ve avatar butonlarını çizer.

---

### 2.4. `RigRenderer.hpp` & `RigRenderer.cpp` (Rig Çizim Motoru)
* **Görevi:** Açık kasa alüminyum rig şasisini, dönen çift fanları ve RGB LED aydınlatmalarını 2D olarak çizer.

### 2.5. `ShaderManager.hpp` & `ShaderManager.cpp` (GLSL 330 Shader Yöneticisi)
* **Görevi:** Donanımların ısısına göre görüntüyü termal FLIR kamera görünümüne çeviren `thermal_vision.fs` fragment shader'ını yönetir.

### 1.7. `Warehouse.hpp` & `Warehouse.cpp` (Çoklu Rig ve Depo Yönetim Sistemi)
* **Görevi:** Tek bir depodaki tüm madencilik raflarını (riglerini) bir araya getirir ve koleksiyonu yönetir.
* `AddNewRig(const std::string& name, size_t capacity)`: Depoya yeni bir 6 slotlu raf satın alır ve kurar ($2,500).
* `NextRig()` & `PreviousRig()`: Arayüzdeki viewport ekranında incelenen aktif rig'i değiştirir (`1/X`).
* `CalculateTotalHashrate()` & `CalculateTotalPowerWatts()`: Tüm depodaki riglerin toplam hashrate ve elektrik tüketimini şebekeye ve ekonomiye kümülatif aktarır.

### 1.8. `CoolingManager.hpp` & `CoolingManager.cpp` (Çok Kademeli Endüstriyel Soğutma Sistemi)
* **Görevi:** Depodaki soğutma kademelerini, yatırım maliyetlerini ve toplam ısı tahliye gücünü yönetir.
* `Kademe 0: Temel Oda Fanı`: 300W Soğutma (Başlangıç).
* `Kademe 1: Sanayi Egzoz Fanları`: +800W Soğutma ($600).
* `Kademe 2: HVAC Soğuk Koridor Chiller`: +2,500W Soğutma ($1,800).
* `Kademe 3: Daldırma Sıvı Soğutma Tankı (Immersion)`: +6,000W Soğutma ($6,000) - Kartları mineral yağ tankına batırarak aşırı ısınmayı ve yanmayı tamamen engeller!

---

## 🎨 2. Görselleştirme, Dokular ve Ayarlar Katmanı (`src/render/`)

### 2.6. `TextureManager.hpp` & `TextureManager.cpp` (Ultra Çözünürlüklü Doku Yöneticisi)
* **Görevi:** `assets/textures/` klasöründen harici yüksek çözünürlüklü PNG/JPG resimlerini yükler ve filtreler.
* `gpu_card.png`: Ekran kartı gövdesine giydirilen ultra-res doku.
* `fan_blade.png`: Isıya göre dönen fan pervanesi görseli.
* `warehouse_bg.png`: Tesis arka plan resmi.

### 2.7. `SettingsModal.hpp` & `SettingsModal.cpp` (Ayarlar ve UI Scale Penceresi)
* **Görevi:** Kullanıcının ekran boyutuna ve göz zevkine göre yazı ve arayüz ölçeğini anında büyütmesini sağlar.
* `UI Scale Seviyeleri`: 1.0x (Normal), 1.25x (Büyük), 1.50x (Çok Büyük), 1.75x (Dev), 2.0x (Ultra).
* `ToggleFullscreen()`: Tek tıkla tam ekran moduna geçirir.

### 2.8. `GPUInspectionModal.hpp` & `GPUInspectionModal.cpp` (360° İnceleme & Overclock Modalı)
* **Görevi:** Rig üzerindeki herhangi bir karta tıklandığında açılan devasa 3D inceleme ve hassas ayar ekranıdır.
* `Draw3DCardPreview(...)`: Kartı fareyle sürükleyerek veya butonlarla 360 derece döndürür. Ön yüzde dönen RGB fanlar ve soğutucu ızgara; arka yüzde (backplate) bakır ısı boruları ve fırçalanmış alüminyum plaka sergilenir.
* `Saat Frekansı (Core Clock Offset)`: -300 MHz ile +500 MHz arası hassas ayar.
* `Güç Limiti (Power Limit)`: %60 ile %160 arası voltaj/tüketim kontrolü.
* `Manuel Fan Profili`: %20 ile %100 arası fan devri.
* `140°C Yanma (Burn) ve Tamir Mekaniği`: 
  * Sıcaklık 105°C'yi aşarsa kart sağlığı erimeye başlar.
  * Sıcaklık 140°C'ye ulaşırsa kart alev alarak **YANAR (BURNT)**, hashrate sıfırlanır ve kömür gibi kararır.
  * Oyuncu `[KARTI TAMIR ET ($400)]` butonuyla kartı servise gönderip sağlığını %100'e geri getirebilir.

---

## 🚀 3. Ana Döngü ve Oyun Akışı (`src/main.cpp`)

* `GameState::LOGIN`: Oyun ilk açıldığında `LoginScreen` devreye girer. Kullanıcı şirket adını yazıp avatarını seçtikten sonra hesabı oluşturulur ve **+\$1,000 hoş geldin bonusu** hesabına geçer.
* `GameState::GAMEPLAY`: 
  * **İnteraktif GPU Tıklama:** Kasadaki herhangi bir karta tıkladığında anında 360° döndürülebilir inceleme modalı açılır.
  * **Kademeli Soğutma Satın Alımı:** Sağ panelden sırasıyla Egzoz Fanları, HVAC Chiller ve Daldırma Sıvı Soğutma üniteleri satın alınabilir.
  * **[AYARLAR] Butonu:** Sağ üst köşedeki butonla açılır; font boyutunu ve UI ölçeğini 2.0x seviyesine kadar büyütebilirsin.
  * **Çoklu Rig & Depo Gezintisi:** Sol panelde `[< ONCEKI]` ve `[SONRAKI >]` butonlarıyla depodaki raflar arasında geçiş yapabilir, seçili rig'e yeni GPU'lar takabilirsin.
  * **`[ 🏗️ YENI RIG SATIN AL ($2,500) ]`**: Depoyu genişleterek yeni bir raf satın alır.
  * **[F11] Tam Ekran Desteği:** Tek tuşla ekranı doldurur.

---

## 🔒 4. Güvenlik, Gizlilik ve GitHub Dağıtımı (`vatansevdimeren/GameOfTex`)

* **Sıfır Güvenlik Riski:** Kod tabanında API Key, token, veritabanı parolası veya özel kimlik bilgisi bulunmaz.
* **.gitignore Mimarisi:** `build/`, `.vs/`, derleme ara dosyaları (`*.obj`, `*.tlog`), `.exe` çıktıları ve kullanıcı yolu içeren CMake önbellek dosyaları git takibinden çıkarılmıştır.
* **GitHub Repository:** Proje başarıyla `https://github.com/vatansevdimeren/GameOfTex` adresindeki `main` branch'ine pushlanmıştır.

---

## ⚡ 5. Sürüm 1.5 Yenilikleri: Hurdaya Satma, Rig Kapatma, ESC ve Depo Kuşbakışı

### A. Yanan / İstenmeyen Kartları Çöpe Atma & Hurdaya Satma (`GPUInspectionModal`)
* **`m_btnScrap` (`[ 🗑️ KARTI HURDAYA SAT / CIKAR (+$75) ]`):**
  * **İşlevi:** Yanan veya elden çıkarılmak istenen kart incelenirken tek tıkla anakart slotundan sökülür.
  * **Ekonomi Entegrasyonu:** Oyuncunun bakiyesine **+\$75 hurda metal ve yedek parça geri dönüşüm bedeli** eklenir.
  * **Slot Serbest Bırakma:** Slot anında boşalır (`EMPTY`), yerine yeni kart takılabilir.

### B. Rig Güç Açma / Kapatma (Power Toggle) Mekaniği (`MiningRig`)
* **`IsPoweredOn()`, `SetPoweredOn(bool)`, `TogglePower()`:**
  * **İşlevi:** Rig aşırı ısındığında veya sigorta yükünü azaltmak istendiğinde rig tamamen kapatılabilir.
  * **Sıfır Enerji & Isı:** Rig kapalıyken elektrik çekişi neredeyse 0W'a düşer (standby 2W), hashrate sıfırlanır ve kartlar güvenle oda sıcaklığına (22°C) soğur. Yangın riski anında bertaraf edilir.
  * **Görsel Durum:** Kapalı rig'de fanlar durur, LED'ler kararır ve arayüzde `[⏸️ KAPALI / DEVRE DIŞI]` rozeti görüntülenir.

### C. Boş / İhtiyaç Fazlası Rig'leri Satma (`Warehouse::RemoveRig`)
* **`RemoveRig(size_t index)`:**
  * **İşlevi:** Oyuncunun artık kullanmak istemediği veya nakde dönüştürmek istediği rig kasasını depodan söker.
  * **İade Değeri:** Kasayı hurdaya satarak **+\$1,200** nakit geri kazanımı sağlar (minimum 1 rig kalacak şekilde güvenlik kontrolü yapılmıştır).

### D. ESC Tuşu ve Akıllı Pencere Yönetimi
* **`SetExitKey(KEY_NULL)`:** Raylib'in oyunu anında sonlandırmasını engeller.
* **Katmanlı ESC Davranışı:**
  1. GPU 360° İnceleme penceresi açıksa -> Modalı kapatır.
  2. Ayarlar menüsü açıksa -> Menüyü kapatır.
  3. Oyun tam ekrandaysa (`F11`) -> Oyunu kapatmadan **küçük pencere (Windowed) moduna** döner!

### E. Depo Kuşbakışı Genel Bakış Matrisi (`WarehouseViewMode::OVERVIEW_GRID`)
* **`DrawWarehouseOverviewGrid(...)`:**
  * Sol paneldeki `[ 🏭 DEPO GENEL BAKIS ]` sekmesiyle açılır.
  * Hangar içerisindeki tüm rig'leri yan yana server-blade raf kartları halinde listeler.
  * Her rig kartında:
    * Rig adı ve çalışma durumu (`⚡ ÇALIŞIYOR` / `⏸️ KAPALI`),
    * 6 adet mini GPU slot durumu (Yeşil: Normal, Turuncu: Sıcak, Kırmızı: Yandı, Gri: Boş),
    * Anlık hashrate, güç çekişi ve en sıcak kart derecesi,
    * Tek tıkla rig'i kapatıp açabileceğin `[AC / KAPAT]` butonu,
    * İncelemek ve kart takmak için tek tıkla rig'e geçiş yapan `[INCELE]` butonu.

### F. Gelişmiş Görsel Efektler (`RigRenderer`)
* **Duman ve Kor Parçacıkları (`DrawSmokeAndSparks`):** Yanan kartlardan yukarı doğru yükselen yarı saydam duman halkaları ve havada uçuşan kırmızı/turuncu kor kıvılcımları.
* **Örgülü PCIe Güç Kabloları (`DrawBraidedPCIeCable`):** Üst alüminyum destek kanalından her ekran kartının 8-pin besleme soketine inen sarı-siyah örgülü yüksek akım kablo demeti.
* **Akıllı LED Göstergeleri:** Kart durumuna göre renk değiştiren durum LED'i (Yeşil: Kazımda, Turuncu: Termal Kısılma, Kırmızı Flaş: Yandı, Sönük Gri: Rig Kapalı).

---

## 🌐 6. Çoklu Dil Desteği (Localization Architecture)

### 6.1. `Localization.hpp` & `Localization.cpp` (Yerelleştirme Yöneticisi)
* **Görevi:** Oyun içi tüm metinlerin, buton etiketlerinin ve uyarıların Türkçe (TR 🇹🇷) ve İngilizce (EN 🇺🇸) dillerinde dinamik olarak yönetilmesini sağlar.
* **Tasarım Deseni (Design Pattern):** Singleton (`LocalizationManager::Get()`) deseni ile projenin her yerinden statik fonksiyonla (`LocalizationManager::Tr("KEY")`) hızlı erişim sunar.

#### Fonksiyonlar ve Görevleri:
* `static LocalizationManager& Get()`: Tekil nesne örneğini döner.
* `void SetLanguage(Language lang)`: Aktif dili (`Language::TURKISH` veya `Language::ENGLISH`) ayarlar.
* `Language GetLanguage() const`: Mevcut dil enum değerini döner.
* `void ToggleLanguage()`: Türkçe ve İngilizce arasında tek tıkla geçiş yapar.
* `const std::string& GetLanguageName() const`: Ayarlar menüsünde buton üzerinde görünecek dinamik dil adını döner.
* `const char* Get(const std::string& key) const`: İlgili dil sözlüğünden metni arar; bulunamazsa anahtarın kendisini döner (güvenli fallback mekanizması).
* `static const char* Tr(const std::string& key)`: Kısayol yardımcı fonksiyon. Kod içerisinde `Core::LocalizationManager::Tr("BTN_BUY_GPU")` şeklinde pratik çağrılır.
* `void InitTranslations()`:
  * **SRP Gerekçesi:** Yalnızca dil haritalarını (std::unordered_map) başlatır; çizim veya mantık kodu içermez.

---

## 💱 7. Çoklu Para Birimi Sistemi (Multi-Currency Support)

### 7.1. `CurrencyType` & `EconomyManager` Güncellemeleri
* **Görevi:** Oyuncunun nakit bakiyesini ve mağaza fiyatlarını farklı küresel para birimlerinde görmesini sağlar.
* **Desteklenen Para Birimleri:**
  * `USD ($)`: Amerikan Doları (Varsayılan baz birim)
  * `USDT (₮)`: Tether Kripto Sabit Coin (1:1 parite)
  * `TRY (₺)`: Türk Lirası (Gerçekçi parite çarpanı: ~38.5 ₺/$)
  * `EUR (€)`: Euro (Gerçekçi parite çarpanı: ~0.92 €/$)

#### Fonksiyonlar ve Görevleri:
* `void NextCurrency()`: Para birimlerini sırayla döngüye sokar (USD -> USDT -> TRY -> EUR -> USD).
* `CurrencyType GetCurrentCurrency() const`: Aktif para birimi türünü döner.
* `std::string GetCurrencySymbol() const`: Seçili birimin sembolünü döner ($, ₮, ₺, €).
* `std::string GetCurrencyName() const`: Ayarlar ekranındaki buton etiketi için birim kodunu ve sembolünü döner.
* `std::string FormatFiat(double usdAmount) const`:
  * **İşlevi:** Baz USD cinsinden tutulan bakiyeyi aktif para biriminin paritesine çarpar ve yerelleştirilmiş formatta string olarak döner.
  * **Örnek:** USD modunda `$1,250.00`, TRY modunda `48,125.00 ₺`.
* `std::string FormatPrice(double usdPrice) const`:
  * **İşlevi:** Mağazadaki butonlar ve yükseltme maliyetleri için tamsayı formatlı fiyat etiketi üretir.

---

## 🚀 8. Dağıtım & Arkadaşların Oyunu Çalıştırması Rehberi

### "Arkadaşlarımda derleyici / Visual Studio yok, oyunu nasıl açacaklar?"

1. **GitHub Releases (En Profesyonel ve Önerilen Yöntem):**
   * GitHub deposunda sağ tarafta bulunan **Releases** sekmesine tıklanır -> **Draft a new release** seçilir.
   * `dist/GameOfTex-Windows.zip` dosyası buraya yüklenir.
   * Arkadaşların doğrudan tek bir "Download ZIP" linkine tıklayarak oyunu indirir.
   * ZIP dosyasını klasöre çıkartıp `GameOfTex.exe`'ye çift tıklamaları yeterlidir. Hiçbir C++, CMake veya Visual Studio kurulumuna gerek **YOKTUR**.

2. **Doğrudan ZIP Paylaşımı:**
   * Proje kökünde oluşturduğumuz `dist/GameOfTex-Windows.zip` (~270 KB) paketini Discord, Telegram veya Google Drive üzerinden arkadaşlarınıza doğrudan atabilirsiniz.

3. **Neden `.exe` Doğrudan Git Deposuna (Commit İle) Eklenmez?**
   * Git bir **kaynak kod versiyon kontrol** sistemidir. `.exe` gibi ikili (binary) dosyalar her derlemede birkaç megabayt değişir ve reponun boyutunu hızla şişirir (repo cloning süreleri çok uzar).
   * Dünyadaki tüm standart açık kaynak ve oyun projelerinde (Unreal, Godot, Linux vb.) kodlar depoda tutulur, derlenmiş çalıştırılabilir dosyalar ise **GitHub Releases** veya **Artifacts** kısmında dağıtılır.

---

## 🛒 9. Donanım ve Tesis Marketi Mimarisi (Hardware Market Architecture)

### 9.1. `MarketCatalog.hpp` & `MarketCatalog.cpp` (Pazar Veritabanı ve Envanter Yöneticisi)
* **Görevi:** Satın alınabilir tüm donanım modellerini, şebeke/trafo seviyelerini ve depo/tesis geliştirmelerini tek bir çatı altında tutar.
* **SRP Gerekçesi:** Sadece ürün katalog verisini ve satın alınan kalıcı geliştirmeleri (yeşil enerji, yangın söndürme) yönetir. Ekrana çizim yapmaz veya bakiye kesintisi yapmaz.

#### Fonksiyonlar ve Görevleri:
* `const std::vector<GPUModelItem>& GetGPUModels() const`:
  * Sistemdeki 5 farklı GPU modelini döner:
    1. **GTX 1660 Super:** Giriş seviye (30 MH/s, 75W, $250)
    2. **RTX 3070 Ti:** F/P canavarı (65 MH/s, 145W, $550)
    3. **RTX 4080 Super:** Yüksek performans (110 MH/s, 220W, $1,100)
    4. **RTX 4090 Monster:** Amiral gemisi (175 MH/s, 360W, $2,000)
    5. **Cryptonex Titan ASIC:** Endüstriyel madencilik canavarı (320 MH/s, 550W, $4,500)
* `std::vector<PowerUpgradeItem>& GetPowerUpgrades()`:
  * Şebeke sigorta sınırını artıran trafo modellerini yönetir (3.6 kW -> 7.5 kW -> 15 kW -> 30 kW).
* `std::vector<FacilityUpgradeItem>& GetFacilityUpgrades()`:
  * Güneş panelleri ve yangın söndürme modüllerini tutar.
* `bool HasAutoFireSuppression() const`:
  * **Mekanik:** Kartlar 140°C'ye ulaştığında yanıp patlamak yerine otomatik termal köpük korumasını devreye sokarak rig'i güvenle kapatır.
* `double GetTotalGreenWatts() const`:
  * Kurulu güneş panellerinden gelen bedava yeşil elektrik üretimini toplar.

---

### 9.2. `MarketModal.hpp` & `MarketModal.cpp` (Pazar Arayüzü ve Görsel Barlar)
* **Görevi:** Glassmorphic modern pazar penceresini çizer, sekmeler arası geçişi sağlar ve kartların teknik özelliklerini görsel renkli ilerleme barları halinde sunar.
* **SRP Gerekçesi:** Yalnızca UI çizimini ve tıklama tespitini yapar. Satın alma olaylarını `MarketPurchaseAction` yapısıyla ana döngüye iletir.

#### Fonksiyonlar ve Görevleri:
* `Open()`, `Close()`, `IsOpen()`: Modal pencerenin görünürlüğünü yönetir.
* `MarketPurchaseAction Update(...)`:
  * Sekme butonlarını, satın alma butonlarını ve ESC tuşunu dinler.
  * Rig doluluğunu ve nakit yeterliliğini denetleyerek buton durumlarını (`[SATIN AL]`, `[RIG DOLU!]`, `[YETERSIZ BAKIYE]`) dinamik günceller.
* `void Draw(...)`:
  * Ekranı yarı saydam karartır ve ortalanmış pazar penceresini çizer.
* `void DrawTabs(...)`:
  * 4 farklı kategori menüsünü (`EKRAN KARTLARI`, `ELEKTRİK & TRAFO`, `SOĞUTMA SİSTEMLERİ`, `TESİS & DEPO`) şık neon alt çizgilerle çizer.
* `void DrawStatBar(...)`:
  * **İşlevi:** Donanımların Kazım Gücü (Cyan), Güç Tüketimi (Sarı/Turuncu) ve Enerji Verimliliği (Yeşil) değerlerini oranlayarak canlı grafik barları halinde çizer.
* `void DrawGPUsCategory(...)`, `DrawPowerCategory(...)`, `DrawCoolingCategory(...)`, `DrawFacilitiesCategory(...)`:
  * İlgili sekmenin kartlarını ve fiyatlarını seçili para birimine (USD, USDT, TRY, EUR) göre yerelleştirerek listeler.

---

## ⚡ 10. Ekstra Watt Olayları & Depo Akıcı Kaydırma (Power Surges & Warehouse Scroll)

### 10.1. Ekstra Watt Çekme & Şebeke Sıçrama Mekanikleri
* **Demeraj / İlk Çalışma Güç Sıçraması (`MiningRig::m_startupSurgeTimer`):**
  * **İşlevi:** Bir rig açıldığında (`SetPoweredOn(true)` veya `TogglePower()`) fanlar %100 hızla döner ve güç kaynağı filtre kondansatörleri dolar. Bu esnada ilk 3 saniye boyunca rig **+30% anlık ekstra watt** çeker.
  * **Oynanış Dengesi:** Şebeke sınırına yakınken aynı anda birden fazla rig açılırsa şalter atar! Oyuncu şebeke kapasitesini doğru yönetmelidir.
  * **Görsel Rozet:** Detaylı rig görünümünde `[⚡ DEMERAJ KALKIS AKIMI: +30% GUC CEKISI]` kutusu belirir.
* **Küresel Ağ Zorluk & Güç Zirvesi (`PowerGrid::Update` / Network Spike):**
  * **İşlevi:** 45 saniyede bir küresel blok zincirinde rastgele bir blok yarışması (Proof-of-Work Spike) başlar.
  * 8 saniye boyunca ekran kartları turbo boost frekansına çıkar: **+25% ekstra güç** çeker, karşılığında **%40 daha fazla coin** üretir!
  * **Görsel Efekt:** HUD panelinin hemen altında altın/turuncu parlayan canlı geri sayım banner'ı açılır.
* **Voltaj Düşümü & Şebeke Zorlanması (`PowerGrid::IsGridStrained`):**
  * Toplam tüketim trafo kapasitesinin %85'ini aştığında voltaj düşüşünden (voltage sag) dolayı kartlar %15 daha fazla ısınır.

---

### 10.2. Depo Kuşbakışı Akıcı Kaydırma & Hızlı Seçim (`RigRenderer`)
* **`DrawWarehouseOverviewGrid` Akıcı Kaydırma (Smooth Scroll):**
  * **Çözülen Problem:** Önceden 3-4 rig'den fazlası ekrana sığmadığı için çizim döngüsü kesiliyordu (`break`).
  * **Fare Tekerleği:** `GetMouseWheelMove()` ile farenin tekerleği döndürüldüğünde pazar hangarı akıcı şekilde aşağı/yukarı kaydırılır.
  * **Scissor Viewport Kırpması:** `BeginScissorMode` ile kartlar çerçeve dışına taşmadan temizce kesilir.
  * **Görsel Neon Scrollbar:** Listenin neresinde olunduğunu gösteren sağ kenar kaydırma çubuğu.
* **Hızlı Rig Seçici Şeridi (`DrawQuickRigSelector`):**
  * **İşlevi:** Detaylı rig görünümünde alt tarafa tüm rig'lerin mini durum butonlarını (`[Rig 1: 180M] [Rig 2: 65M]...`) yan yana yerleştirir.
  * Oyuncu onlarca rig arasından tek bir tıkla istediği kasanın detayına sıçrayabilir.

---

## 📋 11. Görev & Hedef Merkezi, Dinamik Fan Hızı ve Tesis Geneli Soğutma

### 11.1. `TaskManager.hpp` & `TaskManager.cpp` (Görev & Başarım Motoru)
* **Görevi:** Oyuncunun madencilik imparatorluğu kurma sürecinde tamamlayabileceği aşamalı hedefleri, görev ilerlemelerini ve nakit/kripto ödüllerini takip eder.
* **SRP Gerekçesi:** Görev tamamlama şartlarını ve ödül tahsisini yönetir; arayüz çizimi veya dosya kaydı yapmaz.

#### Başlangıç Görevleri ve Şartları:
1. `TASK_INSPECT`: Donanım Uzmanı - Bir GPU'ya tıklayarak 360° İnceleme panelini aç ($200 ödül).
2. `TASK_OVERCLOCK`: Hız Aşırtma - Herhangi bir karta Overclock uygula ($350 ödül).
3. `TASK_BUY_GPU`: Pazar Alışverişi - Donanım marketinden en az 1 GPU satın al ($500 ödül).
4. `TASK_HASHRATE`: Madenci Gücü - Toplam kazım hızını 150 MH/s üzerine çıkar ($600 ödül).
5. `TASK_SELL_CRYPTO`: Kripto Tüccarı - Borsada en az $500 değerinde kripto sat ($450 ödül).
6. `TASK_MULTI_RIG`: Büyük Tesis - Depodaki rig kasası sayısını 2 veya üzerine çıkar ($1,200 ödül).
7. `TASK_COOLING`: Termal Kontrol - Tesis soğutma gücünü 3,000W üzerine çıkar ($800 ödül).
8. `TASK_POWER`: Sanayi Trafosu - Şebeke panosunu 7,500W veya üzerine yükselt ($1,000 ödül).
9. `TASK_SOLAR`: Yeşil Enerji - Tesisine en az 1,000W güneş paneli paketi kur ($1,500 ödül).

#### Fonksiyonlar ve Görevleri:
* `UpdateProgress(warehouse, economy, cooling, powerGrid)`: Tüm hedefleri aktif oyun dünyası verileriyle otomatik karşılaştırır ve eşik aşıldığında görevi tamamlandı (`isCompleted = true`) olarak işaretler.
* `NotifyCardInspected()`, `NotifyCardOverclocked()`, `NotifyCryptoSold()`, `NotifyGpuPurchased()`: Anlık etkileşim olaylarını görev motoruna bildirir.
* `ClaimReward(taskId, economy)`: Tamamlanmış ancak alınmamış bir görevin ödülünü oyuncunun cüzdanına ekler ve görevi `isClaimed = true` yapar.
* `GetUnclaimedCompletedCount()`: Toplanmaya hazır ödül sayısını döner; üst HUD butonunun altın sarısı parlamasını sağlar.

---

### 11.2. `TaskModal.hpp` & `TaskModal.cpp` (Görev Merkezi Arayüzü)
* **Görevi:** Görev listesini, ilerleme barlarını ve parlayan ödül butonlarını içeren Glassmorphic penceredir.
* **Özellikleri:**
  * Fare tekerleğiyle akıcı kaydırma (`m_scrollOffset`).
  * Görev durumuna göre renk değişimi (Tamamlandıysa altın sarısı, alındıysa koyu yeşil, devam ediyorsa çelik mavisi).
  * Parlayan animasyonlu `[ 🎁 ÖDÜLÜ AL ]` butonu (`sin(GetTime() * 6.0f)` nabız efekti).

---

### 11.3. Sigorta Attığında Fanların Durması (Breaker Fan Stop Bugfix)
* **Düzeltilen Sorun:** Şebeke sigortası attığında rig'deki fanlar dönmeye devam ediyordu.
* **Çözüm:** 
  * `RigRenderer::DrawRig` ve `DrawSingleGPU` fonksiyonlarına `isBreakerTripped` kontrolü eklendi.
  * `bool hasActivePower = isPowered && !isBreakerTripped;`
  * Elektrik kesildiğinde fan dönüş açı hızı anında sıfırlanır (`0.0f`), LED'ler kararır ve başlıkta kırmızı `[! SEBEKE KESILDI - SALTER ATTI !]` uyarısı çıkar.

---

### 11.4. Dinamik Fan Hızı & 360° GPU Pervane Bıçakları
* **Düzeltilen Sorun:** Fan hızı artırıldığında fan dönüş hızı görsel olarak değişmiyordu ve 360° ekranda sadece düz daire çiziliyordu.
* **Çözüm:**
  * Fan dönüş hızı doğrudan `gpu->GetFanSpeedPercent()` ile orantılı hale getirildi (`fanDuty * 1600.0 deg/sec`).
  * `GPUInspectionModal::DrawCardPreview`: 3D önizleme fan dairelerinin içine 7 kanatlı dönen gerçek pervane bıçakları ve merkez göbek (hub) çizildi. Fan devri artırıldıkça pervaneler gözle görülür şekilde hızlanır.

---

### 11.5. Tesis Geneli Soğutma Dağıtımı (Facility-wide Cooling Integration)
* **Düzeltilen Sorun:** Soğutma sistemleri (Duvar Fanı, Endüstriyel HVAC, Daldırma Sıvı Soğutma) alındığında yalnızca ilk karta etki ediyor gibi görünüyordu.
* **Çözüm:**
  * `ThermalModel::CalculateGPUTemperature` metoduna `facilityCoolingBonus = 1.0 / (1.0 + (m_coolingPowerWatts / 2500.0))` global ısı transfer çarpanı entegre edildi.
  * Soğutma yükseltmesi yapıldığında depodaki istisnasız TÜM rig ve kartların termal direnci anında düşer; HVAC veya Sıvı soğutma kurulduğunda oda sıcaklığı ve tüm kartlar 15-25°C birden serinler.





