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

---

## 🗺️ 12. Dünya Haritası, Tesis Yönetimi, İkili Güç (PSU & Trafo) ve Türkçe Font Düzeltmesi

### 12.1. `FacilityManager.hpp` & `FacilityManager.cpp` (Küresel Tesis Yönetimi)
* **Görevi:** Dünyanın farklı iklim ve coğrafyalarındaki endüstriyel madencilik tesislerini yönetir.
* **SRP Gerekçesi:** Yalnızca tesis kilitlerini, iklim parametrelerini ve aktif tesis referansını tutar; harita çizimi yapmaz.

#### Tesis Lokasyonları ve İklim/Şebeke Özellikleri:
1. **Teksas Madencilik Hangarı (Austin, ABD) [Başlangıç Deposu]:**
   * *Bedel:* $0 (Varsayılan) | *İklim:* +30°C (Sıcak hava) | *Şebeke:* 15,000W ($0.14/kWh) | *Kapasite:* Max 5 Rig
2. **Reykjavik Jeotermal Vadisi (Reykjavik, İzlanda):**
   * *Bedel:* $8,500 | *İklim:* **-2°C (Doğal soğutma)** | *Şebeke:* 35,000W ($0.05/kWh) | *Kapasite:* Max 8 Rig
3. **Tromsø Fiyort Sualtı Tesisi (Tromsø, Norveç):**
   * *Bedel:* $24,000 | *İklim:* **-6°C (Kutup fiyort suyu)** | *Şebeke:* 65,000W ($0.06/kWh) | *Kapasite:* Max 14 Rig
4. **Frankfurt Endüstri Parkı (Frankfurt, Almanya):**
   * *Bedel:* $55,000 | *İklim:* +16°C | *Şebeke:* 120,000W ($0.18/kWh) | *Kapasite:* Max 20 Rig
5. **Sibirya Kutup Madencilik Üssü (Novosibirsk, Rusya):**
   * *Bedel:* $120,000 | *İklim:* **-18°C (Dondurucu kutup iklimi)** | *Şebeke:* 250,000W ($0.04/kWh) | *Kapasite:* Max 30 Rig

#### Fonksiyonlar:
* `GetActiveFacility()`, `SetActiveFacilityIndex()`: Aktif tesisin işaretçisini döner.
* `PurchaseFacility(index, economy)`: Parasal satın alımı denetler ve tesisi kalıcı olarak oyuncuya açar.
* `SwitchFacility(index)`: Satın alınmış tesisler arasında anlık geçiş yapar.

---

### 12.2. `WorldMapModal.hpp` & `WorldMapModal.cpp` (Taktik Dünya Haritası)
* **Görevi:** Siber-radar tarzında dünya haritasını, kıta sınırlarını, radyal tarama çizgilerini ve küresel madencilik noktalarını çizer.
* **Özellikleri:**
  * Harita üzerinde yeşil (aktif), mavi (sahip olunan) ve altın sarısı (satılık) parlayan nabız düğümleri.
  * Sağ panelde seçili tesisin iklimi, rig kapasitesi, trafo gücü ve elektrik maliyeti özeti.
  * Tesis satın alma ve tek tıkla seyahat/geçiş mekanizması.

---

### 12.3. İkili Güç Hiyerarşisi (Rig PSU + Yüksek Kapasiteli Şebeke)
* **Problem:** 10 GPU takıldığında 3,600W şebeke hemen bitiyor ve şalter atıyordu.
* **Çözüm (İkili Sistem):**
  1. **Ana Tesis Şebekesi (Facility Grid):** 15,000W'tan 250,000W'a kadar devasa trafolarla genişletildi. 10-30 rig rahatça kaldırılabilir.
  2. **Rig İçi Power Supply (`MiningRig::m_psuTier`):** Her rig kasasının kendi bağımsız güç kaynağı vardır:
     * Tier 0: 850W Gold (2-3 GPU)
     * Tier 1: 1300W Platinum ($350 - 4-5 GPU)
     * Tier 2: 1800W Titanium ($650 - 6 GPU)
     * Tier 3: 2600W Server Dual PSU ($1,100 - Ağır ASIC / OC)
  * Kartların watt toplamı rig PSU'sunu aşarsa rig üzerinde `[! PSU ASIRI YUKLENDI !]` uyarısı çıkar.
  * Sağ kontrol panelinden `[ 🔌 PSU YUKSELT ]` butonuyla o kasanın güç kaynağı yükseltilebilir.

---

### 12.4. Ortalama Sıcaklık ve Oda İklimi Göstergeleri
* **Rig Ortalama Sıcaklığı (`MiningRig::CalculateAverageTemperature`):**
  * Her kasanın başlığında anlık ortalama sıcaklık rozeti (`Ort: 62.4°C`) ve PSU tüketim oranı (`PSU: 450W / 850W`) yer alır.
  * Hızlı rig seçim şeridinde her kasanın sıcaklığı canlı yazılır (`[Rig 1: 180M | 62°C]`).
* **Oda İklimi:**
  * Sol viewport başlığında anlık oda sıcaklığı ve iklim simgesi (`❄️ Oda: -2°C` / `🔥 Oda: 30°C`) gösterilir.
  * İzlanda veya Sibirya'ya geçildiğinde oda sıcaklığı derhal düşer, kartlar buz gibi çalışır.

---

### 12.5. Türkçe Karakter ve Font Atlası Düzeltmesi (Unicode Glyph Fix)
* **Problem:** Raylib `LoadFontEx` varsayılan olarak sadece ilk 95 ASCII karakteri yüklediği için ekranda bazı Türkçe harfler (ç, Ç, ğ, Ğ, ı, İ, ö, Ö, ş, Ş, ü, Ü ve °) çıkmıyordu.
* **Çözüm:**
---

## 🎨 13. Tipografi, Metin Gölgelendirmesi (Text Shade), Düzen Çakışması (Overlap Fix) ve Dinamik Ekonomi Dengelemesi

### 13.1. Rajdhani Siber Tipografi & Vektör Font Entegrasyonu
* **Geliştirme:** Resmi Google Fonts deposundan açık kaynaklı, güvenilir ve modern cyberpunk tarzı `Rajdhani-Medium.ttf` ve `Rajdhani-Bold.ttf` fontları projeye dahil edildi (`assets/fonts/`).
* **Özellikleri:**
  * Türkçe Genişletilmiş Kod Noktaları (ç, Ç, ğ, Ğ, ı, İ, ö, Ö, ş, Ş, ü, Ü, °) ile eksiksiz rasterize edilir.
  * Eğer yerel font bulunamazsa Windows Segoe UI veya Arial fontlarına kusursuz geri düşüş (fallback) mekanizması korunur.
  * `TEXTURE_FILTER_BILINEAR` ile yüksek çözünürlüklü ve pürüzsüz antialiased yazı kalitesi sunar.

---

### 13.2. Metin Gölgelendirmesi (Text Drop-Shadow / Shade)
* **Problem:** Yazılar koyu veya renkli zeminlerde bazen düz kalıyor, okunurluk ve derinlik hissi zayıflıyordu.
* **Çözüm:** `UIFrame::DrawTextCustom` içine text drop-shadow eklendi:
  * Her metnin arkasına `(x + 1.2f, y + 1.2f)` ofsetiyle `%85` alfa değerinde yumuşak siyah gölge (`Color{0, 0, 0, 180}`) çizilir.
  * Bu sayede yazılar zemin parazitlerinden tamamen ayrılır ve AAA oyun arayüzü kalitesine kavuşur.
  * `UIFrame::DrawCard` zeminlerine yumuşak ambient gölge eklenerek Glassmorphic panel derinliği artırıldı.

---

### 13.3. Depo Genel Bakış (Rigleri Gör) Buton ve Metin Çarpışması Giderimi
* **Düzeltilen Sorun:** "Rigleri gör" (Kuşbakışı genel bakış) modunda rig kartları listelenirken sol taraftaki metrikler (`Kazım: ... | Güç: ... | Max: ...`) sağdaki `[AÇ/KAPAT]` ve `[İNCELE]` butonlarının üzerine biniyordu.
* **Çözüm (`RigRenderer::DrawWarehouseOverviewGrid`):**
  * Kart düzeni 3 bağımsız ve emniyetli bölgeye ayrıldı:
    1. **Üst Sol:** Rig Adı, Durum Rozeti (`[CALISIYOR]` / `[KAPALI]` / `[PSU ASIRI YUK]`) ve PSU tüketim oranı.
    2. **Alt Sol & Orta:** 6 adet mini GPU yuvası ve metrik kutusu (`metricsX` güvenli aralıkta, butonlara asla yaklaşamaz).
    3. **Sağ Kenar:** `[AÇ/KAPAT]` ve `[İNCELE]` butonları kartın sağ kenarına sabitlendi (`btnInspectX`, `btnPowerX`), metinlerle arasına garantili boşluk bırakıldı.

---

### 13.4. Sol Viewport Sekmeleri ve Rig Kontrol Butonları Emniyeti
* **Düzeltilen Sorun:** `btnTabRigDetail` ve `btnTabOverview` sekmeleri ile rig gezinme butonları (`btnToggleRigPower`, `btnSellRig`, vb.) dar ekranlarda üst üste biniyordu. Ayrıca genel bakış açıkken arka plandaki görünmez rig butonlarına tıklanabiliyordu.
* **Çözüm (`main.cpp`):**
  * Rig kontrol ve hurdaya satma butonları yalnızca `currentViewMode == WarehouseViewMode::RIG_DETAIL` modunda ekrana yerleştirilir, güncellenir ve çizilir.
  * Buton genişlikleri optimize edilerek dar pencerelerde (1100px) dahi sekmelerle butonlar arasında güvenli boşluk temin edildi.

---

### 13.5. Dinamik Madencilik Ağ Zorluğu & Ekonomi Dengelemesi
* **Problem:** Başlangıçta 1-2 kart takıldığında saniyeler içinde on binlerce dolar kazanılıyor, oyunun ilerleme ve tycoon dinamiği dakikalar içinde tükeniyordu.
* **Çözüm (`EconomyManager.hpp` & `EconomyManager.cpp`):**
  * **Baz Zorluk:** `10,000.0` yerine `350,000.0 MH` seviyesine dengelendi.
  * **Dinamik Ağ Zorluğu Formülü:** Oyuncunun depodaki toplam hashrate'i arttıkça küresel ağ zorluğu otomatik adapte olur:
    $$\text{Ağ Zorluğu} = 350,000 + (\text{Toplam Hashrate} \times 180.0)$$
  * **Dengeli Tycoon Eğrisi:**
    * Başlangıç kartı (GTX 1660 Super, 30 MH/s) dakikada ~$10 net getiri sağlar ve amortisman süresi ~25 dakikaya oturur.
    * Kart sayısı ve hashrate arttıkça kazanç büyür ancak zorluk da adapte olarak enflasyonu önler.
    * Bölgesel elektrik tarifeleri (Teksas $0.12 vs İzlanda $0.06 vs Sibirya $0.04) anlamlı bir gider haline gelir; dünya haritasından soğuk ve ucuz tesislere geçmek gerçek bir stratejik avantaja dönüşür.

---

### 13.6. Temiz Stilize Rozetler (Zero-Glitch Text Tags)
* **Düzeltilen Sorun:** Bazı işletim sistemi ve font rasterizasyonlarında `⚡`, `❄️`, `🔥`, `🛒` gibi ham Unicode emojiler `?` glif hatası veya yazı bozulması (syntax hatası hissi) yaratabiliyordu.
* **Çözüm:** Tüm ham emojiler `[MARKET]`, `[ALARM]`, `[SOGUK]`, `[SICAK]`, `[DEMERAJ]`, `[PSU]`, `[CALISIYOR]` gibi fütüristik neon braket formatına dönüştürüldü. Yazı motorunda sıfır glif hatası sağlandı.

---

## ⚡ 14. Canlı Termal Tepki, Saatlik/Günlük Finansal Metrikler ve Modal Tipografi Yenilemesi

### 14.1. Anlık GPU Güç ve Sıcaklık Dinamiği (Direct Thermal Reaction)
* **Problem:** GPU inceleme penceresinde (`GPUInspectionModal`) Power Limit veya Overclock artırıldığında sıcaklık derhal tepki vermiyor, eski donanım değerlerinde kalıyordu. Ayrıca baz termal direnç (0.18) yüksek watt tüketiminde dahi kartı aşırı serin tutuyordu.
* **Çözüm (`ThermalModel.hpp` & `GPUInspectionModal.cpp`):**
  1. `ThermalModel` baz termal direnci `0.18`'den `0.32` seviyesine çıkarıldı. Böylece 120W-250W aralığındaki kartlar gerçekçi fizik yükü altına girer; fan devri düşürüldüğünde veya güç sınırı zorlandığında 80°C - 105°C bandına doğru gerçekçi şekilde ısınır.
  2. `GPUInspectionModal::Update` fonksiyonu artık `const Core::ThermalModel*` parametresi alır. Oyuncu hız aşırtma veya güç sınırını değiştirdiği milisaniyede `CalculateGPUTemperature` çağrılarak `m_currentTemp` anında güncellenir ve arayüzde gecikmesiz yansıtılır.

---

### 14.2. Saatlik ve Günlük Finansal Metrikler (Hourly & Daily Revenue/Profit)
* **Gereksinim:** Oyuncuların her bir kartın saatlik ve günlük ne kadar brüt gelir getirdiğini, ne kadar elektrik yaktığını ve net kârını açıkça görebilmesi.
* **Çözüm (`EconomyManager.hpp` & `GPUInspectionModal.cpp`):**
  * `EconomyManager` sınıfına SRP ilkelerine uygun 5 yeni analitik fonksiyon eklendi:
    * `CalculateHourlyCoins(double hashrate)`: $3600 \times \frac{\text{hashrate}}{\text{zorluk}} \times 0.05$
    * `CalculateHourlyRevenueUSD(double hashrate)`: $\text{Saatlik Coin} \times \text{Coin Fiyatı}$
    * `CalculateDailyRevenueUSD(double hashrate)`: $\text{Saatlik Gelir} \times 24$
    * `CalculateHourlyElectricityCostUSD(double watts, double gridRateKWh)`: $\frac{\text{watts}}{1000} \times \text{gridRateKWh}$
    * `CalculateHourlyNetProfitUSD(...)`: $\text{Saatlik Brüt Gelir} - \text{Saatlik Elektrik Gideri}$
  * `GPUInspectionModal::Draw` içine şık bir finansal bilgi kutusu eklendi:
    * `[GELIR] Saatlik: $X.XX/s | Gunluk: $Y.YY/g` (Canlı altın sarısı renginde)
    * `[NET KAR] $Z.ZZ/s (Elektrik: -$E.EE/s)` (Kâr pozitifse siber yeşil, negatifse kırmızı renkte)

---

### 14.3. Görevler ve Dünya Haritası Pikselli Font Giderimi (Vektör Tipografi)
* **Problem:** `TaskModal.cpp` ve `WorldMapModal.cpp` dosyalarında Raylib'in varsayılan 10x10 pikselli ham bitmap `DrawText` ve `MeasureText` fonksiyonları kullanılıyordu. Bu durum, diğer pencerelerin aksine görev ve harita ekranında yazıların pikselli, bulanık ve eski görünmesine sebep oluyordu.
* **Çözüm:**
  * `TaskModal.cpp` içindeki 9 adet `DrawText` ve 4 adet `MeasureText` çağrısı,
  * `WorldMapModal.cpp` içindeki 12 adet `DrawText` ve 2 adet `MeasureText` çağrısı,
  * Tamamen `UIFrame::DrawTextCustom` ve `UIFrame::MeasureTextCustom` fonksiyonlarına geçirildi.
  * Artık tüm modal ve pencereler çift filtreli, gölgeli ve pürüzsüz `Rajdhani` vektör font motorundan beslenmektedir.

---

## 💾 15. Siber Giriş Ekranı (Title Screen), Kalıcı Kayıt/Yükleme (Save & Load) ve Çoklu Tesis Simülasyonu

### 15.1. Profesyonel Ana Menü / Giriş Ekranı (`MainMenuScreen`)
* **Görevi:** Oyun doğrudan hesap açma veya oyuna atmak yerine, oyuncuyu animasyonlu cyberpunk bir karşılama ekranıyla karşılar.
* **Tasarım ve Özellikleri:**
  * **Matrix Cyber Grid & Yükselen Parçacıklar:** Arka planda yumuşak akan neon veri ızgarası ve rastgele yükselen siber parçacıklar yer alır.
  * **Pulsing Neon Başlık:** `GAMEOFTEX` neon ışıma efektleri ve `CRYPTO MINING & ENERGY TYCOON` altlığı.
  * **Dinamik Butonlar:**
    1. `[ DEVAM ET (KAYITTAN YÜKLE) ]`: Diskte kayıtlı oyun varsa yeşil neon ile parlar; şirket adı, bakiye ve aktif tesisi altlıkta gösterir. Kayıt yoksa pasif kalır.
    2. `[ YENİ OYUNA BAŞLA ]`: Kayıt varsa oyuncuya ilerlemesinin silineceğini bildiren onay penceresi açar; onaylanırsa `LoginScreen` üzerinden yeni profil ve tesis başlatır.
    3. `[ AYARLAR ]`: Oyuna girmeden önce bile dil, ölçek ve para birimini değiştirebilmeyi sağlar.
    4. `[ ÇIKIŞ ]`: Oyunu güvenle kapatır.

---

### 15.2. Bağımsız ve Çökmelere Dayanıklı Kayıt & Yükleme Sistemi (`SaveManager`)
* **Görevi (SRP):** Oyun durumunun diske (`savegame.dat`) kaydedilmesi, okunması, doğrulanması ve silinmesinden tek başına sorumludur.
* **Kaydedilen Veriler:**
  1. **Profil:** Şirket adı, seçili avatar, başlangıç bonusu durumu.
  2. **Ekonomi:** Nakit bakiye ($), kripto cüzdanı, piyasa coin fiyatı, aktif para birimi (USD, USDT, TRY, EUR).
  3. **Tesisler:** Tüm dünya tesislerinin satın alınma durumları, aktif tesis indeksi.
  4. **Rig ve Donanım Detayları:** Her hangardaki rig'lerin adları, PSU seviyeleri, güç durumları, içlerindeki GPU'ların model adları, baz hashrate, baz watt, silikon kaliteleri, hız aşırtma/voltaj düşürme katsayıları, çekirdek saat farkları, güç limitleri, fan hızları, sağlık durumları ve yanmışlık/bozulmuşluk bayrakları.
  5. **Altyapı ve Soğutma:** Her tesisin trafo kapasitesi, sigorta atma durumu, kurulu soğutma katmanları ve oda sıcaklıkları.
  6. **Pazar & Görevler:** Satın alınmış kalıcı trafo/güneş enerjisi yükseltmeleri ve tamamlanan/ödülü alınan görev ilerlemeleri.
* **Otomatik Kayıt (Auto-Save):** Oyuncu her 45 saniyede bir, ayarlar menüsünden ana menüye dönerken veya oyunu kapatırken oyun arka planda otomatik kaydedilir.
* **Save Toast Bildirimi:** Kayıt yapıldığında ekranın üst kısmında 2.5 saniye beliren neon yeşil `[✓ OYUN BASARIYLA KAYDEDILDI]` rozeti gösterilir.
* **Hızlı Kayıt (F5 / Ctrl+S):** Üst HUD paneline `[S] KAYDET` butonu eklendi, F5 veya Ctrl+S ile anında kayıt yapılabilir.

---

### 15.3. Eşzamanlı Çoklu Tesis Simülasyonu (Multi-Facility Simulation)
* **Düzeltilen Mantık Hatası:** Eskiden oyuncu İzlanda, Norveç veya Sibirya tesislerini satın alsa bile yalnızca ekranda seçili aktif tesis elektrik yakıyor ve coin kazıyordu.
* **Çözüm:** Simülasyon motoru artık oyuncunun satın aldığı TÜM tesisleri arka planda eşzamanlı işletir:
  * Her tesis kendi yerel elektrik tarifesiyle (örneğin İzlanda $0.05 vs Teksas $0.14) enerji tüketir.
  * Her tesis kendi yerel kutup/jeotermal iklimine göre kartlarını soğutur.
  * Tüm tesislerdeki çalışan rig'lerin hashrate'leri toplanarak küresel şirketin madencilik cüzdanına coin kazandırır.
  * Böylece dünya haritasından farklı kıtalarda tesis satın almak gerçek bir küresel madencilik imparatorluğu tycoon deneyimine dönüşür.

---

## 🎨 16. Segoe UI Modern Vektör Tipografisi, F11 Tam Ekran Düzeni ve Sıfır Çakışma (Zero-Overlap Responsive Layout)

### 16.1. Pikselli/Köşeli Fontların Giderilmesi & Segoe UI Entegrasyonu (Vector Typography)
* **Kullanıcı Şikayeti:** "şu pixel gibi yazı stilini çok iyi bir yazı stili ile değiştir adam akıllı yap şunu artık"
* **Kök Neden:** Oyunda varsayılan font olarak `Rajdhani` kullanılıyordu. `Rajdhani`, fütüristik ancak köşeli, dar ve segmentli glif yapısına sahip olduğundan düşük/orta çözünürlüklerde veya küçük puntolarda gözü yoruyor ve pikselli/retro atari yazısı hissi veriyordu.
* **Uygulanan Çözüm (`src/main.cpp` & `assets/fonts/`):**
  1. Resmi ve ultra pürüzsüz Windows sistem vektör yazı tipi ailesi olan `SegoeUI-Regular.ttf` ve `SegoeUI-Bold.ttf` (ayrıca modern Google `Inter.ttf`) projeye dahil edildi (`assets/fonts/`).
  2. Font yükleme sırası yenilendi:
     - **Öncelik 1:** `assets/fonts/SegoeUI-Regular.ttf` (38pt) ve `assets/fonts/SegoeUI-Bold.ttf` (42pt)
     - **Yedek 1:** `C:/Windows/Fonts/segoeui.ttf` ve `C:/Windows/Fonts/segoeuib.ttf`
     - **Yedek 2:** `assets/fonts/Inter.ttf` (38/42pt)
     - **Yedek 3:** `C:/Windows/Fonts/arial.ttf` ve `arialbd.ttf`
     - **Yedek 4:** `assets/fonts/Rajdhani-Medium.ttf`
  3. Tüm font dokularına `SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR)` uygulandı.
  4. Türkçe karakterlerin (`ğ, Ğ, ı, İ, ş, Ş, ç, Ç, ö, Ö, ü, Ü`) yanı sıra tüm Latin Extended-A ve Supplement glifleri tam kapsamlı taranarak belleğe alındı.
  5. Sonuç: Sıfır pikselleşme, pürüzsüz kavisler, yüksek okunabilirlik ve modern AAA arayüz standardı elde edildi.

---

### 16.2. F11 Tam Ekran ve Responsive Arayüzde Üst Üste Binme Hatalarının Çözümü (Zero-Overlap Layout)
* **Kullanıcı Şikayeti:** "üst üste binmesin f11 e basınca kral"
* **Kök Neden 1 (Viewport Başlığı vs Sekmeler/Butonlar):**
  - Sol ana oyun çerçevesinde (`viewportRect`), `UIFrame::DrawCard(viewportRect, viewportTitle, ...)` başlığı `bounds.y + 14` hizasına yazıyordu.
  - Aynı anda `btnTabRigDetail`, `btnTabOverview` ve rig yönetim butonları (`btnToggleRigPower`, `btnSellRig`, `btnPrevRig`, `btnNextRig`) de doğrudan `contentY + 10` hizasına çiziliyordu!
  - Bu durum, özellikle F11 tam ekranda veya tesis adı uzun olduğunda sekmelerin ve butonların doğrudan depo başlık metninin üzerine binmesine yol açıyordu.
* **Kök Neden 2 (Üst HUD Rozetleri vs Aksiyon Butonları):**
  - Üst şeritteki 5 rozet (`Şirket`, `Nakit`, `Cüzdan`, `Piyasa`, `Kazım Gücü`), tam ekran modunda genişlediğinde veya arayüz ölçeği (UI Scale) büyütüldüğünde sağ taraftaki `[DÜNYA HARİTASI]`, `[GÖREVLER]`, `[KAYDET]`, `[AYARLAR]` butonlarıyla temas edebiliyor veya rozet yazıları kutu dışına taşıyordu.
* **Uygulanan Çözüm (`src/main.cpp`):**
  1. **İki Satırlı Temiz Viewport Başlık Düzeni:**
     - `DrawCard` çağrısına boş başlık verilerek çakışan çizim kaldırıldı.
     - **1. Satır (`contentY + 8` - `contentY + 38`):** Sol tarafta neon göstergeli depo adı (`viewportTitle`), sağ tarafta ise `btnTabRigDetail` ve `btnTabOverview` sekmeleri yerleştirildi. Depo adı için dinamik genişlik kontrolü eklendi; isim ne kadar uzun olursa olsun sekmelere asla yaklaşamaz.
     - **Ayırıcı Çizgi:** `contentY + 40` hizasında zarif bir alt çizgi çekildi.
     - **2. Satır (`contentY + 45` - `contentY + 77`):** Yalnızca `RIG_DETAIL` modunda sol tarafta `[< ÖNCEKİ]`, `[SONRAKİ >]`, sağ tarafta `[GÜÇ AÇ/KAPAT]`, `[SAT]` butonları bağımsız bir şerit olarak hizalandı.
     - Rig çizim konumu `contentY + 84`, güç barı `barY + 332` ve hızlı rig seçici `contentY + contentH - 40` olarak güncellenerek tüm dikey öğeler arasında ferah aralıklar sağlandı.
  2. **Üst HUD Rozet Genişliği Sınırlandırması:**
     - Rozet genişliği formülü `badgeW = std::clamp(maxAllowedBadgeW, 110.0f, 215.0f)` ile sınırlandırıldı.
     - Son rozetin sağ kenarı ile `[DÜNYA HARİTASI]` butonunun sol kenarı arasında her koşulda matematiksel olarak garantili boşluk bırakıldı.

---

### 16.3. Dinamik Rozet ve Buton Metin Ölçeklendirme (Auto-Fitting Text Engine)
* **`UIFrame::DrawStatBadge` İyileştirmesi:**
  - Rozet içindeki etiket (`fullLabel`) ve değer (`value`) metinleri çizilmeden önce `MeasureTextCustom` ile ölçülür.
  - Metin genişliği rozetin kullanılabilir genişliğini (`width - 24px`) aşarsa, font boyutu oransal olarak küçültülerek metnin asla rozet dışına taşmaması ve yanındaki rozete binmemesi garanti altına alındı.
* **`UIButton::Draw` İyileştirmesi:**
  - Yüksekliği 42px'den küçük olan butonlarda (örneğin 32px'lik rig butonları) altyazı (`m_subtitle`) varsa, eski kodda başlık butonun dışına (üstüne) taşıyordu.
  - Buton yüksekliğine ve genişliğine göre dinamik `titleSize` ve `subSize` hesaplaması eklendi.
  - Metin buton sınırlarından uzunsa genişliğe göre otomatik ölçeklenir; buton dışına dikey veya yatay sıfır taşma sağlanır.

---

### 16.4. Bağımsız Çalıştırılabilir Paket ve Windows Sürümü (v1.9 Dağıtımı)
* **Güncellenen Sürüm:** `GameOfTex v1.9.2 [Segoe UI Native F11]`
* **Paketleme:** `dist/GameOfTex-Windows.zip` (2.18 MB) içerisine güncel Segoe UI fontları, dokular, shader'lar, harita verileri ve `GameOfTex.exe` dahil edilerek tek tıkla çalışmaya hazır hale getirildi.

---

### 16.5. F11 Basıldığında Yazının Bozulmasının Kök Nedeni ve Native Çözümü (F11 Native Resolution)
* **Kullanıcı Şikayeti:** "kral bak burada f11 e basınca yazı stili cok ama cok kötü oluyor bunları düzenlemen gerekiyor diyorum bunu düzeltsene"
* **Kök Neden:**
  1. Raylib'de `ToggleFullscreen()` doğrudan çağrıldığında ekran kartı 1280x720 piksellik arka tamponu (render buffer) donanımsal olarak 1920x1080 ekrana esnetir (hardware stretching). Bu esnetme sonucunda tüm fontlar, çizgiler ve pencereler bulanıklaşır, piksellenir ve bozulur.
  2. Windows masaüstü DPI ölçeklemesi (%125 veya %150) aktifken oyun DPI-Aware olarak başlatılmadığı için Windows DWM pencereyi sanallaştırarak bulanıklaştırmakta ve ekranın sağ kenarı (`AYARLAR`) ile alt şeridini (`YÖNETİM`) pencere dışına taşırıp kırpmaktadır.
* **Uygulanan Çözüm (`src/main.cpp` & `SettingsModal.cpp`):**
  1. **`SetProcessDPIAware()` Entegrasyonu:** Windows DWM sanal ölçeklemesi tamamen devre dışı bırakıldı; oyun monitörün gerçek fiziksel piksellerine birebir bağlandı.
  2. **`toggleFullscreenNative` Lambda Fonksiyonu:** F11 tuşuna basıldığında Raylib önce monitörün gerçek çözünürlüğünü (`GetMonitorWidth`, `GetMonitorHeight`) okur, `SetWindowSize(monW, monH)` ile OpenGL arka tamponunu tam 1920x1080 boyutuna getirir ve ardından tam ekrana geçer. Tam ekrandan çıkıldığında ise 1280x720 boyutuna geri dönüp pencereyi ekranda ortalar.
  3. **Mipmapped Trilinear Doku Filtresi:** Font dokularına `GenTextureMipmaps` ve `TEXTURE_FILTER_TRILINEAR` uygulandı. Artık tam ekranda fontlar en ufak bir bulanıklık veya esneme olmadan tam vektör keskinliğinde işlenir.

---

### 16.6. Tipografi Netliği ve Arayüz Temizliği (Drop-Shadow & Particle Clamping)
* **Çamurlu Gölgelerin Kaldırılması:** Küçük metinlerde (11px-16px) harfleri kalınlaştırıp bulanık gösteren %85 siyah gölge kaldırıldı. Yalnızca 18px ve üzeri büyük başlıklara zarif %40 gölge bırakıldı; harf aralığı `0.5f` yapılarak okunaklılık en üst düzeye çıkarıldı.
* **Rozet Dikey Oranlaması:** `UIFrame::DrawStatBadge` içinde başlık %16, değer %50 dikey oranlarına yerleştirildi. Metinlerin rozetin üst kenarına çarpması veya birbirine yapışması önlendi.
* **GPU Kıvılcım Taşıntısı Düzeltmesi:** Yanmış kartlardan çıkan kıvılcımların rig başlığının (`Rig 01 - Starter Frame`) üstüne uçması engellendi; kıvılcımlar kart çerçevesi içinde sınırlandırıldı.
* **Üst HUD Sağ Panel Kırpılma Koruması:** Buton genişlikleri optimize edildi (`settingsBtnW = 86px`, `saveBtnW = 82px`, `taskBtnW = 118px`, `worldMapBtnW = 110px`). 1024px gibi dar çözünürlüklerde bile `AYARLAR` butonu sağ kenardan 16px içeride kalır; asla kırpılmaz.

---

## 17. ÇOKLU COİN MADENCİLİĞİ, CANLI KRİPTO BORSASI VE GELİŞMİŞ DONANIM KATALOĞU (v2.0.0)

### 17.1. 5 Farklı Kripto Para Birimi ve Algoritmik Ağ Zorluğu (`CryptoCoin`)
* Oyunda tek bir coin yerine 5 farklı kripto para birimi ve madencilik algoritması entegre edildi:
  1. **TEX (TexCoin):**
     * *Algoritma:* KawPow | *Birim:* MH/s | *Taban Fiyat:* $2.40 | *Taban Zorluk:* 120,000 | *Blok Ödülü:* 5.0 TEX
     * *Gereken Tier:* Seviye 1 (Başlangıç madencilik coini).
  2. **RVN (Ravencoin):**
     * *Algoritma:* X16R | *Birim:* MH/s | *Taban Fiyat:* $0.085 | *Taban Zorluk:* 45,000 | *Blok Ödülü:* 2,500 RVN
     * *Gereken Tier:* Seviye 1 (Hızlı kazılan yüksek hacimli altcoin).
  3. **ETC (Ethereum Classic):**
     * *Algoritma:* ETChash | *Birim:* MH/s | *Taban Fiyat:* $28.50 | *Taban Zorluk:* 450,000 | *Blok Ödülü:* 2.56 ETC
     * *Gereken Tier:* Seviye 2 (Profesyonel GPU madenciliği).
  4. **ETHW (Ethereum PoW):**
     * *Algoritma:* Ethash | *Birim:* MH/s | *Taban Fiyat:* $145.00 | *Taban Zorluk:* 1,200,000 | *Blok Ödülü:* 2.0 ETHW
     * *Gereken Tier:* Seviye 3 (Yüksek bellekli sunucu kartları).
  5. **BTC (Bitcoin):**
     * *Algoritma:* SHA-256 | *Birim:* GH/s | *Taban Fiyat:* $64,250.00 | *Taban Zorluk:* 85,000,000 | *Blok Ödülü:* 3.125 BTC
     * *Gereken Tier:* Seviye 4 (Endüstriyel veri merkezi ve devasa ASIC çipleri).
* **Dinamik Piyasa Simülasyonu (`EconomyManager::UpdateMarket`):**
  * Tüm coinler için saniyelik mikro dalgalanmalar (volatilite), ortalamaya dönüş (mean-reversion) ve 60 periyotluk canlı fiyat geçmişi (`priceHistory`) üretilir.
  * 24 saatlik en yüksek (`high24h`), en düşük (`low24h`), yüzde değişim (`priceChange24hPercent`) ve işlem hacmi canlı hesaplanır.

---

### 17.2. İnteraktif Kripto Borsası & Canlı Grafik Deski (`CryptoExchangeModal`)
* **Açılış Yolları:** Sağ paneldeki `[BORSA] KRIPTO AL / SAT` butonuna basılarak, üst HUD'daki `[WALLET]` veya `[MARKET]` rozetlerine tıklanarak ya da klavyeden `[B]` / `[E]` kısayollarıyla açılır.
* **Canlı Neon Fiyat Grafiği:**
  * 60 periyotluk fiyat geçmişini dinamik Min/Max ölçekleme ile çizen alan grafiği (Area Chart).
  * Fiyat artışında neon zümrüt yeşili, düşüşünde siber kırmızı degrade dolgu ve parlak çizgi.
  * Arka planda ızgara çizgileri ve dinamik fiyat ekseni etiketleri.
  * Grafik üzerinde fare gezdirildiğinde (hover) dikey crosshair çizgisi ve anlık fiyat kutucuğu (tooltip).
* **Kazım Hedefi Değiştirici (`[BU COİNİ KAZ]` Butonu):**
  * Oyuncu tek bir tıkla riglerinin hangi coini kazacağını seçebilir.
  * Seçili coin aktif kazılıyorsa `[ŞU AN KAZILIYOR]` rozetiyle belirtilir.
* **Hızlı Alım-Satım Deski:**
  * `%25`, `%50`, `%75`, `%100 (MAX)` hacim butonları.
  * Anlık harcanacak nakit / alınacak coin tahmini (Alım Deski).
  * Bozdurulacak coin miktarı / kazanılacak fiat nakit tahmini (Satım Deski).
  * Toplam portföy değeri özeti (Nakit + 5 Coinin güncel piyasa değeri).

---

### 17.3. 4 Kademeli Donanım & ASIC Kataloğu (`MarketCatalog` & `MarketModal`)
* Market modalına **[TÜM MODELLER] [TIER 1] [TIER 2] [TIER 3] [TIER 4]** filtreleme butonları ve akıcı fare tekerleği kaydırma (scrolling) eklendi:
  1. **Tier 1 (Giriş / Ev):**
     * *RX 580 8GB:* 28 MH/s | 95W | $180
     * *GTX 1660 Super:* 31 MH/s | 75W | $240
     * *RTX 2060 Super:* 42 MH/s | 110W | $340
  2. **Tier 2 (Pro / Atölye):**
     * *RX 6800 XT:* 64 MH/s | 140W | $520
     * *RTX 3070 Ti:* 68 MH/s | 145W | $590
     * *RTX 3080 Trinity:* 98 MH/s | 210W | $820 (Tesis Seviye 2)
  3. **Tier 3 (Sunucu / Veri Merkezi):**
     * *RTX 4070 Ti Super:* 92 MH/s | 180W | $890 (Tesis Seviye 2)
     * *RTX 4080 Super OC:* 115 MH/s | 220W | $1,150 (Tesis Seviye 2)
     * *RTX 4090 Monster 24GB:* 180 MH/s | 360W | $2,100 (Tesis Seviye 3)
  4. **Tier 4 (Endüstriyel & ASIC Çipleri):**
     * *RTX 6000 Ada DataCenter:* 240 MH/s | 300W | $3,600 (Tesis Seviye 3)
     * *Antminer S19 Pro+ Hydro:* 280 MH/s | 460W | $4,100 (Tesis Seviye 3)
     * *Cryptonex Titan ASIC 400:* 350 MH/s | 540W | $4,800 (Tesis Seviye 4)
     * *Quantum Hash Matrix X1:* 520 MH/s | 720W | $7,500 (Tesis Seviye 4)

---

### 17.4. 14 Kademeli Dengeli Görev Sistemi (`TaskManager`)
* Görev havuzu genişletilerek oyuncuyu sıkmayan, hem erken aşamayı hem de ileri madencilik hedeflerini ödüllendiren 14 göreve çıkarıldı:
  1. `TASK_INSPECT`: Donanım Uzmanı (1 GPU İncele - $200)
  2. `TASK_OVERCLOCK`: Hız Aşırtma (1 GPU Overclock yap - $350)
  3. `TASK_BUY_GPU`: Pazar Alışverişi (1 GPU Satın al - $400)
  4. `TASK_FIRST_TRADE`: İlk Kripto Takası (Borsada işlem yap - $300)
  5. `TASK_HASHRATE_50`: Çırak Madenci (50 MH/s üzerine çık - $350)
  6. `TASK_SELL_CRYPTO`: Kripto Tüccarı ($500 değerinde kripto sat - $450)
  7. `TASK_MULTI_RIG`: Büyük Tesis (En az 2 Rig kasası kur - $1,200)
  8. `TASK_MINE_ETC`: Klasik Vizyon (Cüzdanda 1.0 ETC bulundur - $600)
  9. `TASK_HASHRATE`: Madenci Gücü (150 MH/s üzerine çık - $800)
  10. `TASK_COOLING`: Termal Kontrol (3,000W soğutma gücü kur - $800)
  11. `TASK_POWER`: Sanayi Trafosu (7,500W şebeke panosu aç - $1,000)
  12. `TASK_SOLAR`: Yeşil Enerji (1,000W güneş paneli kur - $1,500)
  13. `TASK_MINE_ETHW`: İş Kanıtı Üssü (Cüzdanda 1.0 ETHW bulundur - $2,500)
  14. `TASK_ASIC_KING`: ASIC Kralı (500 MH/s hesaplama gücüne ulaş - $5,000)

---

### 17.5. Çoklu Coin Kayıt ve Yükleme Entegrasyonu (`SaveManager`)
* `savegame.dat` dosyasına `[ECONOMY]` altında `activeCoinIndex`, `coinCount` ve her coinin bakiyesi (`coin_i_balance`) ile fiyatı (`coin_i_price`) yazılır.
* Eski kayıt dosyalarıyla tam geriye dönük uyumluluk (backward compatibility) korunmuştur; eski sürümlerden kalan kayıtlar hatasız açılır.

---

## 18. MUM GRAFİKLERİ (CANDLESTICK OHLC), PİYASA STABİLİZASYONU VE VERİMLİLİK METRİKLERİ (v2.1.0)

### 18.1. Fiyat Koridoru (Bollinger Kanalı) & Scam Coin Koruması
* **Kullanıcı Şikayeti:** "kanki suanda fiyatlar cok kafasına göre ilerliyor bak TEX coin özellikle 300 dolardan 3 dolara düştü harbidende scam coin gibi oldu, bu kadar fazla dalgalanma olmasın."
* **Kök Neden:**
  1. Eski kayıtlarda kalan `price=300.0` ya da `price=2400.0` gibi değerler `SaveManager::LoadGame` sırasında TEX'in yeni $2.40 taban fiyatına çekilirken saniyeler içinde çöküş hissi yaratıyordu.
  2. `UpdateMarket` fonksiyonundaki rastgele yürüyüş (random walk) %0.7 gibi devasa bir standart sapmayla çalışıyor ve tavan/taban sınırı bulunmuyordu.
* **Uygulanan Çözüm (`EconomyManager.cpp`):**
  - **Sakinleştirilmiş Mikro Volatilite:** Saniyelik standart sapma %0.7'den **%0.08'e** düşürüldü.
  - **Fiyat Koridoru & Kelepçe (Price Channel):** Her coinin fiyatı matematiksel olarak `[basePrice * 0.70, basePrice * 1.45]` aralığına kilitlendi.
    - TEX ($2.40): $1.68 - $3.48 arasında doğal ve profesyonel salınır; asla $300'e uçamaz veya $0.01'e çakılamaz.
    - RVN ($0.085): $0.060 - $0.123 arasında salınır.
    - ETC ($28.50): $20.00 - $41.00 arasında salınır.
    - ETHW ($145.00): $101.00 - $210.00 arasında salınır.
    - BTC ($64,250): $45,000 - $93,000 arasında salınır.
  - `SetCoinPrice` ve `SetCryptoPrice` fonksiyonlarına otomatik sanitizasyon eklenerek eski bozuk kayıt dosyaları bile anında bu banda çekilir.

---

### 18.2. Canlı Mum Grafikleri (Candlestick OHLC Engine)
* **`Candle` Yapısı (`EconomyManager.hpp`):** Açılış (`open`), En Yüksek (`high`), En Düşük (`low`), Kapanış (`close`) ve Hacim (`volume`).
* **Zaman Pencereleri:** Her 4 saniyelik mikro periyot yeni bir muma dönüştürülür. Son 28 mum bellekte tutulur.
* **Görselleştirme (`CryptoExchangeModal::DrawCandlestickChart`):**
  - **Boğa Mumu (Yeşil):** `Close >= Open` olduğunda parlayan yeşil gövde (`Color{45, 225, 125, 255}`).
  - **Ayı Mumu (Kırmızı):** `Close < Open` olduğunda siber kırmızı gövde (`Color{255, 65, 75, 255}`).
  - **Fitiller (Wicks):** High ve Low arasına çizilen keskin dikey gölge çizgileri.
  - **Hacim Barları (Volume):** Mumların altına yerleştirilen dinamik oransal hacim barları.
  - **Mod Seçici:** `[MUM (OHLC)]` ve `[ÇİZGİ]` butonlarıyla iki grafik tarzı arasında tek tıkla geçiş.
  - **Hover Tooltip:** Mumun üzerine gelindiğinde anlık Açılış, Yüksek, Düşük, Kapanış (OHLC) değerleri rozet olarak gösterilir.

---

### 18.3. Dinamik Piyasa Olayları (Event-Driven Market News)
* Rastgele anlamsız savrulmalar yerine, haber tabanlı kontrollü piyasa dalgaları eklendi:
  1. **BOĞA RALLİSİ (+%18):** Kurumsal yatırımcı girişi ve ETF haberleri (24 saniye, yeşil parlayan duyuru şeridi).
  2. **PİYASA DÜZELTMESİ (-%16):** Kâr satışları ve türev tasfiyeleri (24 saniye, kırmızı duyuru şeridi).
  3. **MADENCİLİK PATLAMASI (+%12):** Küresel ağ hashrate büyümesi (20 saniye, altın sarısı duyuru şeridi).
  4. **DÜZENLEME BASKISI (-%10):** Hükümet düzenlemeleri ve piyasa geri çekilmesi (20 saniye, turuncu duyuru şeridi).

---

### 18.4. Madencilik Verimlilik & Karlılık Metrikleri
* Borsa grafik başlığına iki yeni stratejik metrik entegre edildi:
  - **100 MH/s Başına Günlük Getiri:** Seçili coinin zorluk ve güncel fiyatına göre 100 MH/s gücün ürettiği nakit (`Verimlilik (100 MH/s): $0.58 / gün`).
  - **Karlılık Endeksi (Profitability Index):** Ağ zorluğu ile taban getiri oranı karşılaştırılarak hesaplanan canlı kârlılık yüzdesi (`Karlılık Endeksi: %108`). Oyuncu hangi coini kazmanın o an daha karlı olduğunu doğrudan görebilir.

---

## 19. SIFIR CMD KONSOLU, DOĞRUDAN TAM EKRAN (NATIVE FULLSCREEN), KRİPTO UZMANLARI HABER AKIŞI, DÖNEMSEL ALTCOİN BOOM VE 20 GÖREV (v2.2.0)

### 19.1. Arka Plandaki CMD Konsol Penceresinin Tamamen Kaldırılması (Zero-Console GUI Launch)
* **Kullanıcı Talebi:** "Kanki arkada artık cmd açık olmasını istemiyorum bak."
* **Kök Neden:** Standart konsol alt sistemi (`/SUBSYSTEM:CONSOLE`), `main()` fonksiyonu çalıştırıldığında Windows tarafından otomatik bir `cmd.exe` penceresi tahsis edilmesine yol açıyordu.
* **Uygulanan Çözüm (`CMakeLists.txt`):**
  - CMake hedef özelliklerine `WIN32_EXECUTABLE TRUE` ve bağlayıcı parametresi olarak `LINK_FLAGS "/ENTRY:mainCRTStartup"` tanımlandı.
  - Portable standard `int main()` fonksiyon gövdesi bozulmadan korundu, ancak derlenen `GameOfTex.exe` ikili dosyası saf Windows GUI alt sistemine (`Subsystem = 2`) geçirildi.
  - Sonuç: Çift tıklandığında veya kısayoldan açıldığında hiçbir siyah konsol/cmd penceresi açılmaz; oyun doğrudan ve sessizce başlar.

---

### 19.2. Doğrudan Yerel Tam Ekran Başlatma (Direct Native Fullscreen Launch - Sıfır Bulanıklık)
* **Kullanıcı Şikayeti:** "F11 basınca büyüyünce yazı kalitesi çok düşüyor... exe açılınca direkt tam ekran olarak başlasın."
* **Kök Neden:** Raylib 1280x720 pencere oluşturup sonradan büyütüldüğünde dahili framebuffer ölçeklenerek rasterize metinlerde bulanıklığa yol açabiliyordu.
* **Uygulanan Çözüm (`main.cpp`):**
  - `InitWindow` hemen ardından, henüz 0. kare çizilmeden önce `GetCurrentMonitor()`, `GetMonitorWidth()` ve `GetMonitorHeight()` çağrılarak oyuncunun fiziksel monitör çözünürlüğü (örn. 1920x1080) alınır.
  - `SetWindowSize(monWidth, monHeight);` ile dahili arabellek monitörle 1:1 piksel hizalanır ve ardından `ToggleFullscreen();` çağrılır.
  - Sonuç: Oyun açılır açılmaz doğrudan tam ekranda ve monitörün doğal çözünürlüğünde başlar; yazılar ve grafikler jilet gibi keskin, pürüzsüz ve sıfır bulanıktır. İstenirse `F11` veya `ESC` ile pencereli moda dönülebilir.

---

### 19.3. Sahte Kripto Uzmanları & Canlı Haber Akışı (`NewsManager` & `NewsModal`)
* **Kullanıcı Talebi:** "Uzman görüşlerinin haber akışları gelsin sahte bir şekilde, oradan piyasa vs değişsin her zaman doğru olmak durumunda değil... haber için mesaj yeri ekle."
* **Uygulanan Sistem:**
  1. **Uzman ve Analist Profilleri:**
     - `@CryptoWhale` (Balina tüccarı)
     - `Bloomberg Crypto Desk` (Haber ajansı)
     - `Elon Byte` (Teknoloji milyarderi spekülatör)
     - `Satoshi Ghost` (Gizemli OG madenci)
     - `Altcoin Hunter` (Gem avcısı)
     - `Fed Crypto Watch` (Makro analiz)
     - `WallStreet Degen` (Kaldıraç ustası)
  2. **Ters Köşe / Balina Tuzakları (Contrarian & Fake News Mekaniği):**
     - Haberlerin bir kısmı gerçeği yansıtırken (Boğa/Ayı), bazıları "Whale Trap" (Balina Tuzağı) olarak üretilir (`isContrarian = true`). Analist "Fiyat aya çıkacak, hemen alın!" derken balinalar gizlice satış yaparak piyasayı düşürür.
  3. **Canlı Bildirim Toast Şeridi (`HasRecentToast`):**
     - Yeni bir tweet/haber geldiğinde ekranın üst kısmında parlak siber bildirim şeridi kayar (`[YENİ HABER] @CryptoWhale: ... [N] Oku`). Tıklandığında veya `[N]` tuşuna basıldığında haber terminali açılır.
  4. **Bloomberg/Twitter Tarzı Haber Terminali (`NewsModal`):**
     - Üst HUD'da `[N] HABERLER (X)` butonu ve kırmızı okunmamış rozet sayacı.
     - Okunmamış haberlerde mavi neon ışıma ve parlak nokta göstergesi.
     - `[TÜMÜNÜ OKUNDU SAY]` butonu ve tek tıkla okuma desteği.
     - Görev entegrasyonu: Haber okundukça görev yöneticisi otomatik tetiklenir.

---

### 19.4. Ayrıştırılmış Coin Fiyatları ve Dönemsel Altcoin Sezonları (`⭐ GÜNÜN EN KARLI COİNİ`)
* **Kullanıcı Şikayeti:** "Tüm coinler birbirine korele bir şekilde gidiyor, bir de her zaman btc kazımı çok mantıklı oluyor bunlar dönem dönem farklılık göstermesi gerekiyor mesela ilk coini veya en ucuzu kazmak daha kolay olması lazım ve daha fazla kazmak gerekiyor."
* **Kök Neden & Dengeleme:**
  1. **BTC GPU Ölçekleme Hatası Düzeltildi:** BTC algoritması `SHA-256 (GH/s)` olmasına rağmen madencilik hashrate'i `MH/s` olarak doğrudan hesaplanıyordu. Bu durum BTC'yi 1000 kat fazla ödüllendiriyordu. Formül düzeltildi:
     $$\text{Etkili Hashrate} = (\text{Birim} == \text{"GH/s"} \ ? \ \frac{\text{hashrate}}{1000} : \text{hashrate}) \times \text{profitabilityMultiplier}$$
  2. **Başlangıç GPU Kazımı Dengelemesi:**
     - GPU rigleriyle BTC kazıldığında satoshi seviyesinde çok az BTC üretilir ve ASIC olmadan verimsiz kalır.
     - Buna karşılık TEX ve RVN günde yüzlerce/binlerce adet kazılır; başlangıç rigleri için açık ara en mantıklı ve ödüllendirici seçenek haline gelir.
  3. **Bağımsız Rastgele Yürüyüş (Decoupled Random Walk):**
     - Tüm coinlerin aynı anda aynı yöne gitmesi engellendi; her coin bağımsız rassal eğim ve haber etkisiyle ayrışır. Biri düşerken diğeri yükselebilir.
  4. **Dönemsel Karlılık Rotasyonu (`m_rotationTimer`):**
     - Her 50 saniyede bir rastgele bir coinde "Mining Boom / Altcoin Sezonu" başlar ve `profitabilityMultiplier` **%185 - %235** seviyesine fırlar!
     - Borsa sekmesinde ve grafik bilgi panelinde altın sarısı `[⭐] GUNUN EN KARLI COINI` rozeti yanar.

---

### 19.5. 20 Göreve Genişletilmiş Görev ve Başarım Sistemi (`TaskManager`)
* Görev havuzu 14'ten tam 20'ye genişletildi:
  1. `TASK_INSPECT`: Donanım Uzmanı (1 GPU İncele - $200)
  2. `TASK_OVERCLOCK`: Hız Aşırtma (1 GPU Overclock yap - $350)
  3. `TASK_BUY_GPU`: Pazar Alışverişi (1 GPU Satın al - $400)
  4. `TASK_FIRST_TRADE`: İlk Kripto Takası (Borsada işlem yap - $300)
  5. `TASK_READ_NEWS`: **Haber Taktiği (Kripto uzmanlarından 3 haber/analiz oku - $250)**
  6. `TASK_HASHRATE_50`: Çırak Madenci (50 MH/s üzerine çık - $350)
  7. `TASK_MINE_TEX_100`: **Yerel Cevher (Cüzdanda 100.0 TEX biriktir - $500)**
  8. `TASK_MINE_RVN_1000`: **Kuzgun Madenciliği (Cüzdanda 1,000 RVN biriktir - $650)**
  9. `TASK_SELL_CRYPTO`: Kripto Tüccarı ($500 değerinde kripto sat - $450)
  10. `TASK_MULTI_RIG`: Büyük Tesis (En az 2 Rig kasası kur - $1,200)
  11. `TASK_MINE_ETC`: Klasik Vizyon (Cüzdanda 1.0 ETC bulundur - $600)
  12. `TASK_DIVERSIFY`: **Çeşitli Portföy (En az 3 farklı coin cüzdanında bulundur - $1,000)**
  13. `TASK_HASHRATE`: Madenci Gücü (150 MH/s üzerine çık - $800)
  14. `TASK_COOLING`: Termal Kontrol (3,000W soğutma gücü kur - $800)
  15. `TASK_POWER`: Sanayi Trafosu (7,500W şebeke panosu aç - $1,000)
  16. `TASK_SOLAR`: Yeşil Enerji (1,000W güneş paneli kur - $1,500)
  17. `TASK_MINE_ETHW`: İş Kanıtı Üssü (Cüzdanda 1.0 ETHW bulundur - $2,500)
  18. `TASK_PORTFOLIO_10K`: **Kripto Balinası (Toplam portföy değerini $10,000 üzerine çıkar - $2,000)**
  19. `TASK_MAX_FACILITY`: **Küresel Ağ (Dünya haritasında en az 2 tesis satın al - $3,500)**
  20. `TASK_ASIC_KING`: ASIC Kralı (500 MH/s hesaplama gücüne ulaş - $5,000)

---

## 20. ŞARTEL VE TERMAL GÜVENLİK SİSTEMİ, RIG KORUMASI VE AKILLI MARKET SİSTEMİ (v2.2.1)

### 20.1. Şartel Atma Mantığının Düzeltilmesi & Yangınların Önlenmesi
* **Kullanıcı Talebi:** *"şarteller neden atıyor sadece atma durumunu hepsi yanmasın elektrik gücü yetmediğin böyle olsun kral anladın mı"*
* **Kök Neden:**
  1. `PowerGrid::Update` içerisinde her 45 saniyede bir yapay `%25` şebeke dalgalanması (`m_spikeActive = true`) tetikleniyordu. Oyuncu AFK kaldığında anlık yük kapasiteyi aşıp şarteli atıyordu.
  2. Şartel attığında simülasyon döngüsünde ısı modeli güncellenmiyor ve GPU'lar soğuyamıyordu. Ayrıca aşırı ısınma durumunda kartlar 140°C'ye ulaşıp yanıyordu (`SetBurnt(true)`). Yanmış kartların hashrate'i 0 MH/s'ye düştüğü için kullanıcı rig'in silindiğini veya donanımın tamamen yok olduğunu sanıyordu.
* **Uygulanan Çözüm (`PowerGrid.cpp`, `ThermalModel.cpp`, `main.cpp`):**
  1. Rastgele güç sıçraması kaldırıldı (`GetPowerSurgeMultiplier() = 1.0`). Şartel **YALNIZCA VE YALNIZCA** anlık rig tüketimi trafo/pano kapasitesini aştığında atar.
  2. Depolara dış ortamla doğal ısı transferi eklendi; kapalı ortam sıcaklığı 48°C ile sınırlandırıldı.
  3. **Şartel Atınca Güvenli Soğuma:** Şartel attığı an depodaki tüm elektrik akımı derhal sıfırlanır (`0W`). Kartlar çalışmayı durdurur, ısı üretimi kesilir ve ortam pasif olarak dış hava sıcaklığına doğru hızla soğur. **Şartel atması sebebiyle HİÇBİR kart yanmaz veya hasar almaz!**
  4. **105°C Termal Güvenlik Kesicisi:** Soğutma yetersiz kaldığında kartların yanmasını önlemek için modern GPU BIOS koruma mekanizması getirildi. Sıcaklık 105°C sınırına ulaştığında rig güvenli şekilde kendi elektriğini kapatır (`r->SetPoweredOn(false)`). Kartlar hasar görmez, oyuncu soğutmayı artırıp rig'i tekrar açabilir.

### 20.2. Rig Silinmesi / Kaybolması Koruması (`btnSellRig`)
* **Kullanıcı Şikayeti:** *"amk tualete gidip geldim Bir tane rigim silinmiş bunedir"*
* **Kök Neden:**
  - Viewport üzerindeki `btnSellRig` (Rig Sat / Hurda) butonu, içinde pahalı ekran kartları bulunan rig'ler için bile aktifti ve tek tıkla rig'i içindeki tüm kartlarla birlikte $1,200 hurda bedeline siliyordu.
* **Uygulanan Çözüm (`main.cpp`):**
  - Buton artık rig içinde en az 1 GPU varsa otomatik olarak devre dışı bırakılır (`btnSellRig.SetDisabled(true)`).
  - Buton üzerinde `"(Önce kartları satın/çıkarın)"` uyarısı çıkar.
  - Tesisin son rig'i satılamaz (`"(Son kasa satılamaz)"`).
  - Böylece hiçbir rig veya ekran kartı kazara silinemez.

### 20.3. Akıllı Donanım Marketi & Çoklu Rig Slot Dağıtımı (`MarketModal` & `MarketCatalog`)
* **Kullanıcı Şikayeti:** *"hala markette sorun var bunları düzenlemeni isityorum"*
* **Kök Nedenler:**
  1. Market GPU sekmesi yalnızca ekranda seçili `activeRig`'i kontrol ediyordu. Rig 01 dolduğunda (6/6), tesiste boş Rig 02 bulunsa bile buton `"RIG DOLU!"` yazarak kilitleniyordu.
  2. Elektrik yükseltmeleri Teksas'ın 15,000W taban kapasitesinden daha düşük değerlere (ör. 7,500W) sahipti ve satın alındığında kapasiteyi düşürüyordu.
* **Uygulanan Çözüm (`MarketModal.cpp`, `MarketCatalog.cpp`, `main.cpp`):**
  1. **Akıllı Slot Yönlendirmesi:** GPU satın alırken önce seçili rig kontrol edilir; doluysa tesisteki boş slotu olan ilk rig hedef alınır (`action.targetRigIndex`).
  2. **Görsel Durum:** Market başlığında montaj yapılacak hedef kasa gösterilir: `Hedef: Rig 01 [5/6 Slot]` veya `Hedef: Rig 02 [0/6 Slot] (Otomatik Boş Kasa)`. Yalnızca tesisteki TÜM kasalar 6/6 dolduğunda buton kilitlenir.
  3. **Trafo Kademeleri Ölçeklendirildi:**
     - Kademe 1: Standart Şebeke (15,000W - Başlangıç)
     - Kademe 2: Sanayi Tipi Trifaze (30,000W - $2,500)
     - Kademe 3: Özel Trafo İstasyonu (60,000W - $6,500)
     - Kademe 4: Yüksek Gerilim Hattı (120,000W - $15,000)
  4. Kapasite düşürme engellendi: Satın alımlarda `std::max(mevcut, yeni)` uygulanarak kapasitenin daima artması güvence altına alındı.

---

## 21. SIFIR KONSOL (ZERO-CONSOLE GUI), RIG BUTONLARI VE 4K ULTRA HD DOKULAR (v2.2.2)

### 21.1. Arka Plandaki CMD Konsolunun Kesin Olarak Yok Edilmesi
* **Kullanıcı Şikayeti:** *"kanki arkada hala Cmd acık bir sekilde devam ediyor bak cmd acılmasın diyorum"*
* **Kök Neden:**
  - Proje kök dizininde (`c:\Users\ErenV\OneDrive\Desktop\GameOfTex\GameOfTex.exe`) eski bir build'den kalan ve Subsystem 3 (`CONSOLE`) olarak derlenmiş bir çalıştırılabilir dosya bulunuyordu. Kullanıcı projeyi açıp doğrudan kök dizindeki exe'ye çift tıkladığında Windows otomatik olarak konsol penceresi tahsis ediyordu.
* **Uygulanan Çözüm (`main.cpp` & `CMakeLists.txt`):**
  1. Win32 `GetConsoleWindow` ve `ShowWindow(consoleWnd, 0)` API çağrıları `main()` girişine eklendi. Oyun nasıl açılırsa açılsın (ister terminalden, ister çift tıklamayla) konsol penceresi anında gizlenir.
  2. Kök dizindeki `GameOfTex.exe`, `dist/GameOfTex-Windows/GameOfTex.exe` ve derleme çıktısının tümü Subsystem 2 (`IMAGE_SUBSYSTEM_WINDOWS_GUI`) ile güncellendi.
  3. `CMakeLists.txt` içerisine otomatik post-build adımları eklendi; bundan sonra her derlemede kök ve dağıtım dizinindeki exe'ler otomatik olarak GUI modunda güncellenir.

---

### 21.2. Viewport Üstündeki "< ÖNCEKİ" ve "SONRAKİ >" Butonlarının Kaldırılması
* **Kullanıcı Talebi:** *"önceki ve sonraki butonları kalksın altta riglerden ulasır kullanıcı"*
* **Uygulanan Çözüm (`src/main.cpp`):**
  1. Viewport'un ikinci satırında yer alan ve arayüzü kalabalıklaştıran `btnPrevRig` ve `btnNextRig` butonları tamamen kaldırıldı.
  2. Kullanıcıların zaten alt kısımda yer alan interaktif Hızlı Rig Seçici (Quick Rig Selector) hap butonlarına (`[Rig 01]`, `[Rig 02]`...) tıklayarak istedikleri kasaya anında geçiş yapması sağlandı.
  3. Klavye ile oynamayı sevenler için Sol/Sağ yön tuşları (`KEY_LEFT`, `KEY_RIGHT`) kısayolu korundu.
  4. Açılan ferah alanda `[GÜÇ AÇ / KAPAT]` ve `[RIG'İ SAT]` butonları sağ kenara dengeli ve temiz bir şekilde yerleştirildi.

---

### 21.3. 4K Ultra HD & Fotogerçekçi Doku Entegrasyonu
* **Kullanıcı Talebi:** *"birde internet üzerindeki en kalşteki 4k ultra hd görselleri bul onları kullan projede"*
* **Üretilen ve Entegre Edilen 4K Varlıklar (`assets/textures/`):**
  1. **`warehouse_bg.png` (4K Madencilik Hangarı Arka Planı):**
     - Siberpunk LED aydınlatmalı, metal sunucu raflarıyla dolu, devasa endüstriyel havalandırma kanallarına ve ıslak/parlak zemin neon yansımalarına sahip sinematik 4K madencilik tesisi görseli entegre edildi.
  2. **`gpu_card.png` (Ultra HD RTX 4080 Ekran Kartı):**
     - Titanyum ve karbon fiber zırh kaplamalı, çift fan yuvalı, RGB ışık çizgili, dikey açılı fotogerçekçi ekran kartı gövdesi.
  3. **`fan_blade.png` (Şeffaf Alfa Kanallı Dönen Fan Pervanesi):**
     - Metalik merkez göbekli, 9 kanatlı aerodinamik siyah fan rotoru. Şeffaf alfa kanalı sayesinde ekran kartının üzerinde sıcaklığa göre akıcı bir şekilde dönmektedir.

---

## 22. DOĞRUDAN NATİVE TAM EKRAN BAŞLATMA VE KRİSTAL NETLİKTE TİPOGRAFİ (v2.2.3)

### 22.1. Exe Açıldığında Doğrudan Native Tam Ekran Başlama (Direct Fullscreen Launch)
* **Kullanıcı Şikayeti:** *"çözünürlüğü artırman gerekicek direkt ben exe yi actığımda direkt karsıma F11 ile tam ekrana baslıcak kücük ekran olamıcak kanki"*
* **Kök Neden:**
  1. `InitWindow(1280, 720)` pencereyi ilk önce 720p olarak açıyordu. Ardından çağrılan `ToggleFullscreen()` bazı sistemlerde GLFW penceresi henüz ekranda oluşmadan önce çağrıldığı için başarısız oluyor ya da ekran kartını 1280x720 video moduna zorlayıp tüm ekranda pikselli esnetme yapıyordu.
  2. ESC tuşuna basıldığında modal olmasa dahi `toggleFullscreenNative()` çağrılarak oyun aniden küçük pencere moduna küçülüyordu.
* **Uygulanan Çözüm (`main.cpp`):**
  1. `ChangeDirectory(GetApplicationDirectory())` çağrısı `main()` girişine eklendi. Exe masaüstünden veya herhangi bir kısayoldan başlatılsa dahi tüm fontlar ve 4K dokular kesin olarak exe klasöründen eksiksiz yüklenir.
  2. Windows `GetSystemMetrics(SM_CXSCREEN)` ve `GetSystemMetrics(SM_CYSCREEN)` ile kullanıcının monitörünün tam fiziksel çözünürlüğü (1920x1080, 2K veya 4K) tespit edildi.
  3. `InitWindow(nativeScreenWidth, nativeScreenHeight)` doğrudan monitörün fiziksel boyutunda başlatıldı ve ilk karede `ToggleFullscreen()` uygulanarak doğrudan F11 tam ekran modunda açılması sağlandı. **Artık oyun asla 1280x720 küçük pencere olarak başlamaz.**
  4. ESC tuşunun tam ekrandan çıkarma davranışı kaldırıldı; ESC yalnızca açık modalları kapatır. Tam ekran / pencere geçişi yalnızca F11 tuşu ile yapılır.

### 22.2. Yazıların Pikselli / Bulanık Görünümünün Giderilmesi (Pixel-Perfect Snapped Vector Typography)
* **Kullanıcı Şikayeti:** *"yazı kalitesini arttır pixel pixel duruyor gibi oluyor sunları düznlle"*
* **Kök Neden:**
  1. `GenTextureMipmaps` ve `TEXTURE_FILTER_TRILINEAR` fonksiyonları, alfa kanallı font atlasında alt mip seviyelerinde harf kenarlarını grileştirip bozuyordu. Küçük puntolarda bu durum harflerin pikselli ve bozuk görünmesine yol açıyordu.
  2. Fontlar 32px rasterize edilip 11-14px gibi küçük boyutlara küçültülürken harf kalınlıkları piksel sınırlarına denk gelmediği için karakterlerde piksel atlamaları oluşuyordu.
  3. Metin çizim koordinatları küsuratlı (float) olduğunda sub-pixel bulanıklığı meydana geliyordu.
* **Uygulanan Çözüm (`main.cpp` & `UIFrame.cpp`):**
  1. **48px Ultra Çözünürlüklü Vektör Font Atlası:** Segoe UI ve alternatif fontlar 48px yüksek çözünürlükle rasterize edildi.
  2. **Mipmap Kaldırıldı & Bilinear Filtreleme:** Mipmap oluşturma iptal edildi; font atlasına doğrudan donanımsal `TEXTURE_FILTER_BILINEAR` uygulandı.
  3. **Piksel Kenetleme (Pixel-Snapping):** `UIFrame::DrawTextCustom` içinde metin koordinatları `std::round(x)` ve `std::round(y)` ile tam ekran piksellerine kenetlendi. Harflerdeki tüm pikselleşme, bulanıklık ve titreme giderilerek ipeksi pürüzsüzlükte tipografi elde edildi.

---

## 23. RESIZABLE PENCERE MİMARİSİ, ÇİFTE F11 KAYMA HATASININ ÇÖZÜMÜ, PROSEDÜREL GPU TASARIMINA DÖNÜŞ VE GELİŞMİŞ UI ÖLÇEKLENDİRME (v2.2.4)

### 23.1. 2 Kez F11'e Basınca Ekranın Kayması (Drift) Hatasının Çözümü
* **Kullanıcı Şikayeti:** *"2 kez f11 e basıp kücültüp büyültüğümüzde erkran kayıyordu böyle hatalar kabul edilemez adam akıllı yap sunları direkt ekranın size ını al kullanıcının sonra buna göre window olusutrup ona orantulıu bir şekilde çiz"*
* **Kök Neden:**
  1. `FLAG_WINDOW_HIGHDPI` bayrağı, Windows `SetProcessDPIAware()` ile birlikte çalıştığında Raylib'in pencere boyutları ve OpenGL framebuffer'ı arasında çift ölçekleme yapıyordu (%125 veya %150 ölçekli ekranlarda koordinatlar 1.25x bölünerek kayıyordu).
  2. Eski `toggleFullscreenNative` lambdasında tam ekrana geçerken veya tam ekrandan çıkarken `SetWindowSize(monW, monH)` çağrılıyordu. Windows pencereli moddayken monitör boyutunda bir pencere oluşturulduğunda başlık çubuğu (+31px) ve kenarlıklar (+8px) sebebiyle pencereyi aşağı ve sola itiyordu. Ardından `ToggleFullscreen()` bu kaymış konumu kaydediyor, ikinci F11 basışında pencere her seferinde ekranda 31 piksel kayıyordu.
* **Uygulanan Çözüm (`main.cpp` & `SettingsModal.cpp`):**
  1. `FLAG_WINDOW_HIGHDPI` kaldırıldı; `SetProcessDPIAware()` sayesinde donanımsal 1:1 piksel eşlemesi garanti altına alındı.
  2. Monitör boyutları `GetSystemMetrics(SM_CXSCREEN)` ve `GetSystemMetrics(SM_CYSCREEN)` ile doğrudan alındı; pencereli mod için monitörün %80'i boyutunda merkezlenmiş pencere (`windowedWidth, windowedHeight`) oluşturuldu.
  3. F11 geçişi için Raylib 5.0'ın native `ToggleBorderlessWindowed()` fonksiyonuna geçildi. Bu fonksiyon pencere boyutunu bozmadan, video modu değiştirmeden ve ekranı kaydırmadan doğrudan tam ekran ile pencereli mod arasında kusursuz geçiş sağlar. İki kez değil, yüz kez F11'e basılsa dahi piksel kayması sıfıra indirildi.
  4. Sol panelde rig ve güç göstergesi çizimlerinde `rigX` ve `barX` koordinatları `pad + std::max(0.0f, (leftW - rigBaseW) / 2.0f)` ile kenetlendi; pencere ne kadar küçültülürse küçültülsün hiçbir öğe ekran dışına taşmaz.

### 23.2. Prosedürel Klasik Ekran Kartı Tasarımına Geri Dönüş
* **Kullanıcı Talebi:** *"bu ekran kartlarnıı sevmedim eski stilimize gecbilriiz öylesi daha güzel duruyordu"*
* **Uygulanan Çözüm (`RigRenderer.cpp` & `GPUInspectionModal.cpp`):**
  1. Fotoğraf kaplamaları (`gpu_card.png` ve `fan_blade.png`) rig üzerindeki çizimlerden kaldırıldı.
  2. Kullanıcının çok beğendiği klasik prosedürel tasarım yeniden devreye alındı: Koyu alüminyum gövde, hassas CNC heatsink soğutma ızgaraları, çift ters yönlü dönen RGB aydınlatmalı pervaneler, sıcaklığa göre dinamik renk değiştiren fan merkezleri (Mavi -> Yeşil -> Turuncu -> Kırmızı) ve durum LED'leri.
  3. 3D GPU İnceleme modalında (`GPUInspectionModal.cpp`) da karbon ön yüz, alüminyum ızgaralar ve bakır ısı boruları prosedürel olarak çizilecek şekilde eşitlendi.

### 23.3. Başlangıç Tipografi ve UI Boyutlarının Büyütülmesi
* **Kullanıcı Talebi:** *"birde bu yazıları vs biraz büyüt amk hersey ock kücük baslıyor ayar cek su UI kısmına"*
* **Uygulanan Çözüm (`UIFrame.hpp`, `UIFrame.cpp`, `UIButton.cpp`):**
  1. **Global UI Ölçeği:** `UIFrame::s_uiScale` varsayılan değeri `1.0f`'tan `1.15f`'a yükseltildi. Tüm metinler ve rozetler doğrudan %15 daha büyük ve belirgin başlar.
  2. **Üst HUD Rozetleri:** `UIFrame::DrawStatBadge` etiket fontu 13px (ölçekle ~15px), değer fontu 18/21px (ölçekle ~24px) seviyesine çıkarıldı. Dikey hizalama optimize edildi.
  3. **Buton Tipografisi:** `UIButton::Draw` içerisindeki başlık fontları 13.5px - 18.5px aralığına, alt başlık fontları 10.5px - 12.5px aralığına yükseltildi. Metin genişlik taşma kontrolleri dinamik tutularak butonların sınırlarından taşması engellendi.

---

## 24. IDLE ÇARPAN MEKANİKLERİ, ŞANSLI ALTIN NONCE, RİG SİNERJİLERİ, AR-GE TEKNOLOJİ AĞACI & VENTURE IPO (PRESTIGE) SİSTEMİ (v2.3.0)

### 24.1. Başlangıç Menüsü ve Giriş Ekranı Tipografi / Kutu Boyutlarının Büyütülmesi
* **Kullanıcı Talebi:** *"hala ilk basta oyun basladığında cok kücük yazı fontları kututucklar oluyor bunlara birer ayar cekmen gerekicek kral tamam mdıır?!"*
* **Uygulanan Çözümler (`MainMenuScreen.cpp`, `LoginScreen.cpp`, `UIFrame.hpp`, `UIFrame.cpp`):**
  1. **Global Ölçek:** `UIFrame::s_uiScale` taban değeri `1.25f`'e çekildi. Kart başlıkları (`DrawCard`) 16px'den 20px'e, metin kutuları (`DrawTextInput`) 16px'den 22px'e çıkarıldı.
  2. **Ana Menü (`MainMenuScreen.cpp`):**
     - Başlık "GAMEOFTEX" fontu 54px'den 72px'e çıkarıldı.
     - Alt başlık ("YENI NESIL KRIPTO MADENCILIK SIMULASYONU") 16px'den 22px'e büyütüldü.
     - Menü buton yükseklikleri 58px'den 68px'e, buton genişliği 520px-640px aralığına çıkarıldı. Buton başlıkları 22px, alt yazıları 15px yapıldı.
  3. **Şirket Kurulum / Giriş Ekranı (`LoginScreen.cpp`):**
     - Merkez modal kart genişliği 620-800px, yüksekliği 620-780px aralığına genişletildi.
     - Şirket adı girdi kutusu yüksekliği 60px'e ve fontu 22px'e çıkarıldı.
     - Avatar seçim kartları 82px yüksekliğe çıkarıldı; ikonlar 22px ve açıklamalar 15px olarak büyütüldü.
     - "MADENCİLİĞE BAŞLA" aksiyon butonu 68px yüksekliğe ve 30px devasa fonta ulaştırıldı.

### 24.2. Şanslı Altın Nonce & Çılgınlık Çarpan Motoru (MultiplierManager)
* **Kullanıcı Talebi:** *"IDLE oyunlarında carpan atan bazı seyler vardır bizde buna benzer seyler ekeleybiliriz sen ne diyorsun kral"*
* **Mimari ve İşleyiş (`MultiplierManager.hpp` / `.cpp`):**
  1. **Doğal Nonce Doğuşu:** Depo görünümü (viewport) alanında her 40-75 saniyede bir parıldayan, hafifçe süzülen altın bir madencilik noncesi (Golden Nonce) belirir.
  2. **Etkileşim:** Oyuncu altın nonce'a tıkladığında altın parçacık patlaması eşliğinde 4 farklı Çılgınlık (Frenzy) modundan biri rastgele tetiklenir:
     - **Turbo Hash Hızı (TURBO HASH):** Tüm riglerin kazım gücü 30 saniye boyunca tam **5.0 katına** (`x5.0`) fırlar.
     - **Boğa Rallisi (BULL RUN):** Borsadaki tüm coin fiyatları 25 saniye boyunca anında **+%80 primlenir**.
     - **Süperiletken Soğutma (SUPERCONDUCTOR):** 40 saniye boyunca rig sıcaklıkları ve ortam ısısı minimum seviyede kilitlenir; kartlar sıfır ısınmayla çalışır.
     - **Kripto Fon Airdrop (AIRDROP):** Şirket kasasına anında net varlığın %15'i (veya min $1.500) nakit hibe edilir.
  3. **Canlı HUD Banner:** Frenzy aktifken üst HUD'ın hemen altında geri sayımlı animasyonlu çılgınlık çubuğu görünür.

### 24.3. Donanım ve Rig Sinerjileri (Rig Synergy Multipliers)
* **Özellik:** Oyuncunun rig yapılandırmasını optimize etmesini ödüllendiren dinamik donanım komboları eklendi:
  - **6x Kart Takım Kombosu (`[6x KOMBO +%25]`):** Bir rig içerisindeki 6 slotun tamamı aynı GPU modelinden oluştuğunda rigin kazım gücüne **+%25 sinerji çarpanı** eklenir.
  - **Kriyojenik Soğukluk Bonusu (`[KRIYO +%15]`):** Rig ortalama sıcaklığı 60°C'nin altında tutulabildiğinde **+%15 saf verim çarpanı** kazanılır.
  - **Eko Güç Dengesi (`[EKO GUC +%10]`):** PSU yükü %70'in altında kaldığında optimum gerilimden dolayı **+%10 hashrate bonusu** verilir.
* **Görselleştirme (`RigRenderer.cpp`):** Rig üzerinde aktif sinerjiler altın sarısı / neon yeşil canlı rozetler (`[6x KOMBO +%25]`, `[KRIYO +%15]`) halinde çizilir.

### 24.4. AR-GE Teknoloji Ağacı & Venture IPO (Prestige) Sistemi
* **Mimari ve İşleyiş (`ResearchManager.hpp` / `.cpp`, `ResearchModal.hpp` / `.cpp`):**
  1. **Üst HUD [R] AR-GE Butonu:** Üst panelde `[R] AR-GE (Tekno & IPO)` butonu ve klavye `[R]` kısayolu eklendi.
  2. **Geliştirilebilir 5 Kalıcı Teknoloji:**
     - **Özel BIOS & Firmware:** Kademe başına +%10 (Maks +%50) küresel hashrate artışı.
     - **Yapay Zeka Havuz Yönlendirici:** Kademe başına +%12 (Maks +%60) kazılan coin başına kar artışı.
     - **Grafen Nano-Termal Soğutma:** Kademe başına -%8 (Maks -%40) ısı üretimi düşüşü.
     - **Yeşil Enerji PPA Sözleşmesi:** Kademe başına -%10 (Maks -%50) elektrik faturası indirimi.
     - **Akıllı Tesis Otomasyonu:** Atan şalteri anında otomatik kaldırır ve aşırı ısınan kartların fanlarını anında %100'e getirir.
  3. **Venture IPO / Şirket Halka Arzı (Prestige):**
     - Oyuncunun toplam net serveti $50.000'ı aştığında şirket halka arz edilebilir (`Prestige`).
     - Halka arzda nakit ve donanımlar sıfırlanırken, şirketin değerine göre kalıcı **Girişim Hisseleri (Venture Shares)** kazanılır.
     - Sahip olunan her hisse, sonraki tüm oyunlarda **kalıcı +%5 kazım hızı ve +%5 borsa kazanç çarpanı** sağlar (katlanarak büyüyen Idle Tycoon döngüsü).
  4. **Disk Kaydı:** AR-GE seviyeleri ve Girişim Hisseleri `savegame.dat` dosyasına `[RESEARCH]` başlığı altında kaydedilir ve oyundan çıkıp girildiğinde korunur.

---

## 25. İŞLEMCİ (CPU) MADENCİLİĞİ & KADEMELİ KASA (RİG FRAME) YÜKSELTME SİSTEMİ (v2.4.0)

### 25.1. Problem ve Tasarım Amacı
* **Kullanıcı Talebi:**
  *"projede sıkıntılar var istersen söyle yapabiliriz suan market kazım sisteminini daha fazla güncellememiz geekiyor ilk basta kullanıcıları işlkemcilerle mi baslatsak işlemciler içerisinde mining oluyor mudur bunlara ait coinler farklı crypto paralar cıkartalım bunları kazsın birde her levele göre güzel design edilmiş rigler olsun bu rigleri upgrade edelim yoısa yeni rig almak icin cok fazla beklememiz geerkeicek bunları ayarlamamız lazım"*
* **Çözülen Oynanış Sorunları:**
  1. **Başlangıç Kilitlenmesini Önleme:** Yeni bir oyuncu GPU alacak nakiti yokken bile anakart üzerinde takılı gelen hazır CPU sayesinde ilk saniyeden itibaren kripto para (XMR, RTM) kazmaya başlar; oyunda tıkanıp bekleme devri kapandı.
  2. **Yüksek Yeni Rig Maliyeti ($2,500) Bariyerini Aşma:** Bir rig dolduğunda yeni rig almak için $2,500 biriktirme zorunluluğu kaldırıldı. Artık mevcut kasa şasisi adım adım (2 -> 4 -> 6 -> 8 -> 10 slot) çok uygun maliyetlerle ($180, $450, $950, $1800) yerinde büyütülebilir.

---

### 25.2. Kademeli Kasa (Rig Frame Tier) Yükseltme Sistemi
* **Mimari ve İşleyiş (`MiningRig.hpp`, `MiningRig.cpp`, `main.cpp`):**
  - Kasalara 5 kademeli şasi seviyesi (`m_rigLevel`, 1 ila 5) entegre edildi:
    * **Tier 1 - Ahşap Garaj Kasası:** 2 GPU Slotu. Başlangıç kasasıdır. Marangoz ahşap çıtalar ve pirinç köşe braketleriyle çizilir.
    * **Tier 2 - Alüminyum Açık Kasa:** 4 GPU Slotu. Yükseltme: **$180**. Hafif metalik mavi-gri eloksallı açık hava madencilik şasisi.
    * **Tier 3 - Çelik Pro Miner Şasi:** 6 GPU Slotu. Yükseltme: **$450**. Endüstriyel mat koyu çelik yapı ve siyan neon raylar.
    * **Tier 4 - 4U Server Kabineti:** 8 GPU Slotu. Yükseltme: **$950**. Turuncu endüstriyel sunucu kabini ve yan panellerde dönen Delta yüksek devirli fan ızgaraları.
    * **Tier 5 - Kriyojenik Sıvı Daldırma Tankı:** 10 GPU Slotu. Yükseltme: **$1,800**. Cam pencereli dielektrik sıvı daldırma tankı, floresan turkuaz sıvı parıltısı ve yüzeye yükselen dinamik kabarcık animasyonları.
  - **Kasa Yükseltme Butonu (`[KASAYI YÜKSELT]`):** `RIG_DETAIL` modunda şasi kontrol paneline eklendi. Oyuncunun mevcut seviyesine, sonraki kademe adına ve yükseltme maliyetine göre dinamik olarak güncellenir. Tıklandığında nakit düşülerek kasa şasisi anında genişletilir.
  - **Kapasite ve Culling:** GPU slot genişlikleri kasanın 2, 4, 6, 8 veya 10'luk kapasitesine göre dinamik olarak ölçeklenir; boş slotlar havalandırma ızgarasıyla şık bir şekilde görselleştirilir.

---

### 25.3. İşlemci (CPU) Madenciliği & Özel Kripto Paralar
* **Mimari ve Algoritma (`EconomyManager.hpp`, `EconomyManager.cpp`):**
  - **İşlemciye Özel Kripto Paralar Eklendi:**
    1. **Monero (XMR):** *RandomX Algoritması* - ASIC ve GPU dirençli, saf CPU gücüyle kazılan öncü gizlilik coini ($168.50 taban fiyat).
    2. **Raptoreum (RTM):** *GhostRider Algoritması* - İşlemci L3 önbellek boyutuna göre optimize edilmiş dinamik proof-of-work coini ($0.0022 taban fiyat).
  - **CPU Kazım Motoru (`MineCPUShare`):**
    - Tüm çalışan rig'lerdeki CPU'ların KH/s (KiloHash/saniye) güçleri toplanır.
    - Şirketin AR-GE ve çılgınlık çarpanlarıyla birleştirilerek XMR ve RTM cüzdanlarına düzenli madencilik geliri akar.

---

### 25.4. Donanım Marketi: [İŞLEMCİ (CPU)] Sekmesi & Satın Alma Kataloğu
* **Arayüz ve Katalog (`MarketCatalog.hpp`, `MarketCatalog.cpp`, `MarketModal.hpp`, `MarketModal.cpp`):**
  - Donanım Marketine 5. ana kategori olarak **`[ISLEMCI (CPU)]`** sekmesi eklendi.
  - 7 Farklı Gerçekçi Masaüstü ve Sunucu İşlemcisi Listelendi:
    1. **AMD Ryzen 5 3600 (Giriş/Başlangıç):** 6C/12T, 7.2 KH/s, 65W TDP - *$95* (Tüm başlangıç kasalarında hazır takılı gelir).
    2. **Intel Core i5-12400 (F/P):** 6C/12T, 9.5 KH/s, 75W TDP - *$140*.
    3. **AMD Ryzen 7 5800X (Hızlı Orta Segment):** 8C/16T, 14.8 KH/s, 105W TDP - *$240*.
    4. **AMD Ryzen 9 5950X (Pro Miner):** 16C/32T, 28.5 KH/s, 142W TDP - *$480*.
    5. **AMD Ryzen 9 7950X (Zen 4 Canavarı):** 16C/32T, 38.0 KH/s, 170W TDP - *$650*.
    6. **AMD Threadripper 3990X (HEDT İş İstasyonu):** 64C/128T, 82.0 KH/s, 280W TDP - *$1,850*.
    7. **AMD EPYC 9654 Zen4 Server (Endüstriyel Veri Merkezi):** 96C/192T, 115.0 KH/s, 360W TDP - *$3,200*.
  - **Canlı Performans Göstergeleri:** Her CPU kartı üzerinde RandomX KH/s gücü barı, TDP tüketim barı ve satın alıp aktif kasaya takma butonu bulunur.

---

### 25.5. Anakart Üzerinde Dönen CPU Soğutucu Fanı & Canlı KH/s Rozeti
* **Görselleştirme (`RigRenderer.cpp`):**
  - Kasa çizimlerinin sol alt kısmında gerçek bir anakart CPU soketi çizilir:
    * Koyu entegre devre yolları, montaj vidaları ve soket çerçevesi.
    * CPU hava/sıvı soğutucu bloğu ve rig açıkken gerçek zamanlı dönen neon fan pervanesi.
    * Canlı CPU modeli etiketi ve anlık RandomX KH/s hız rozeti (`AMD Ryzen 5 3600 - 7.2 KH/s`).

---

### 25.6. Kayıt ve Yükleme Geriye Uyumluluğu (`SaveManager.cpp`)
* Disk kayıt formatına `rigLevel`, `cpuName`, `cpuHash` ve `cpuWatts` alanları eklendi.
* Eski kayıtlar yüklendiğinde varsayılan değerlerle (Tier 1 ahşap kasa, Ryzen 5 3600) sorunsuz ve geriye uyumlu şekilde açılması sağlandı.

---

## 26. GELİŞMİŞ GÖREV AĞI (+50 YENİ GÖREV & ÖDÜL), KRİPTO ÇAPRAZ ÖDÜLLER (1 BTC -> 10 SOL), DİNAMİK TESİS KİLİTLERİ & KATEGORİZE EDİLMİŞ SADE ARAYÜZ (v2.5.0)

### 26.1. Problem ve Kullanıcı Talebi
* **Kullanıcı Talebi:**
  *"tamamdır kanki ödüllerde ekstra min +50 ödül ekle vede her fabrika aldığında tekrardan yeni görevler ekleyerejj devam et mesela 1000 dolara ulasınca 100 dolar ekstra bonmus versin 1 btc kazınca ekstra atıyorum 10 tane solana versin crypto marketi vs de ekle uygulamanın arayüzü sade ve anlaşılır olsun"*
* **Ana Hedefler:**
  1. Görev havuzuna minimum +50 yeni görev ekleyerek toplam görev sayısını 75+'e çıkarmak.
  2. Kademeli nakit kilometre taşları oluşturmak ($1,000 kasaya ulaşınca anında +$100 bonus, $2,500'a ulaşınca +$250 bonus vb.).
  3. Kripto çapraz ödül sistemini entegre etmek: Özellikle 1 BTC kazıldığında cüzdana anında **+10 SOL (Solana)** hediye edilmesi, 0.1 BTC -> 2 SOL, 2 BTC -> 25 SOL vb.
  4. Solana'yı (`SOL`) kripto borsasına, fiyat motoruna ve cüzdan sistemine 8. coin olarak eklemek.
  5. Dünya haritasında yeni fabrika/tesis satın alındıkça (İzlanda, Norveç, Almanya, Sibirya) o tesise özel kilitli görevlerin dinamik olarak açılması (`isUnlocked`). Henüz satın alınmamış tesislerin görevlerinin şık kilit rozetiyle oyuncuyu teşvik etmesi.
  6. Görev modalını sade, anlaşılır ve modern bir kategori sekme çubuğu (`[HEPSİ]`, `[NAKİT]`, `[KRİPTO]`, `[TESİS]`, `[DONANIM]`, `[TİCARET]`) ile yenilemek.

---

### 26.2. Solana (SOL) Kripto Para ve Borsa Entegrasyonu
* **Mimari (`EconomyManager.hpp`, `EconomyManager.cpp`):**
  - Borsaya 8. tradeable ve cüzdanda tutulabilir coin olarak **Solana (`SOL`)** eklendi:
    * Taban fiyat: **$145.00**
    * Algoritma: **Proof of History (PoH)**
    * 24s Hacim: **125M USD**
    * Başlangıç 20 mumluk geçmiş piyasa verisi simülasyonu.
  - Cüzdan kredilendirme metodu `AddCoinBalance(const std::string& coinId, double amount)` eklendi; görevler tamamlandığında doğrudan SOL, BTC, ETH, RVN, XMR bakiyelerine anında yükleme yapılır.
  - Kripto borsa modalı (`CryptoExchangeModal`) 8 coin'i dinamik olarak listeler ve SOL alım/satımını destekler.

---

### 26.3. Genişletilmiş Görev Mimarisi ve +50 Yeni Görev (75+ Toplam Görev)
* **Veri Modeli (`TaskManager.hpp`, `TaskManager.cpp`):**
  - Görev yapısına `category` (`TaskCategory`), `requiredFacilityId`, `rewardCryptoSymbol`, `rewardCryptoAmount` ve `isUnlocked` alanları eklendi.
  - 6 Farklı Kategoride Zengin Görev Havuzu:
    1. **Nakit Kilometre Taşları (`TaskCategory::CASH`):**
       - $1,000 Nakit Hedefi -> **+$100 Nakit Bonusu** (Kullanıcı spesifik talebi)
       - $2,500 Nakit Hedefi -> +$250 Nakit Bonusu
       - $5,000 Nakit Hedefi -> +$500 Nakit Bonusu
       - $10,000 Nakit Hedefi -> +$1,000 Nakit Bonusu
       - $25,000, $50,000, $100,000, $250,000, $500,000 ve $1,000,000 Nakit Milestone'ları (Ödül: +$100,000 ve +1 BTC!).
    2. **Kripto Çapraz Ödülleri (`TaskCategory::CRYPTO`):**
       - 1.0 BTC Kazımı -> **+$1,000 Nakit + 10.00 SOL** (Kullanıcı spesifik talebi)
       - 0.1 BTC Kazımı -> +$250 Nakit + 2.00 SOL
       - 0.5 BTC Kazımı -> +$600 Nakit + 5.00 SOL
       - 2.0 BTC Kazımı -> +$2,500 Nakit + 25.00 SOL
       - 10.0 ETH Kazımı -> +$1,200 Nakit + 5.00 SOL
       - 5.0 XMR Kazımı -> +$300 Nakit + 1.50 SOL
       - 5,000 TEX Kazımı -> +$150 Nakit + 500 RVN
       - 10,000 RVN Kazımı -> +$200 Nakit + 1,000 TEX vb.
    3. **Dinamik Tesis Görevleri (`TaskCategory::FACILITY`):**
       - İzlanda Jeotermal Tesisini Satın Al -> +$3,500 Nakit + 5 SOL
       - [İzlanda Kilidi Açıldıktan Sonra]: İzlanda'da 3 Rig Kur, 20 GPU Çalıştır, Sıcaklığı 55°C Altında Tut.
       - Norveç Fiyort Hidroelektrik Tesisini Satın Al -> +$10,000 Nakit + 15 SOL
       - [Norveç Kilidi Açıldıktan Sonra]: Norveç'te 5 Rig Doldur, Sıfır Karbon 50kW Güç Üret.
       - Almanya Endüstriyel Park Tesisini Satın Al -> +$25,000 Nakit + 30 SOL
       - [Almanya Kilidi Açıldıktan Sonra]: Almanya'da 50 GPU Çalıştır, Tier 4 Soğutmaya Ulaş.
       - Sibirya Kriyojenik Arktik Tesisini Satın Al -> +$60,000 Nakit + 60 SOL + 1 BTC
       - [Sibirya Kilidi Açıldıktan Sonra]: Sibirya'da 10 Rig Kur, Tier 5 Sıvı Daldırma Kasaları Aç.
    4. **Donanım ve Kasa Geliştirme Görevleri (`TaskCategory::HARDWARE`):**
       - İlk CPU'yu Tak -> +$50 Nakit + 0.1 XMR
       - Kasayı Alüminyum Şasiye Yükselt (Tier 2) -> +$100 Nakit
       - Kasayı Çelik Pro Kasaya Yükselt (Tier 3) -> +$250 Nakit
       - Kasayı 4U Sunucu Kabinine Yükselt (Tier 4) -> +$500 Nakit + 2 SOL
       - Kasayı Kriyojenik Sıvı Daldırma Tankına Çıkar (Tier 5) -> +$1,200 Nakit + 5 SOL
       - Güç kaynağı, soğutma yükseltmeleri ve 50 GPU'ya kadar kademeli filo görevleri.
    5. **Ticaret, Haberler ve Piyasa Görevleri (`TaskCategory::TRADING`):**
       - İlk Kripto İşlemini Yap, 5 İşlem Yap, 25 İşlem Yap, 5 Kripto Haberini Analiz Et, $10,000 Hacme Ulaş.

---

### 26.4. Sade, Anlaşılır ve Kategorize Edilmiş Görev Modalı (`TaskModal`)
* **Kullanıcı Arayüzü İyileştirmeleri (`TaskModal.hpp`, `TaskModal.cpp`):**
  - **Kategori Filtre Sekmeleri:** Üst kısıma 6 adet hızlı filtre butonu eklendi:
    * `[HEPSİ]`, `[NAKİT]`, `[KRİPTO]`, `[TESİS]`, `[DONANIM]`, `[TİCARET]`
    * Seçili sekme neon altın/mavi renkle aydınlatılır, görev sayacı başlıkta gösterilir.
  - **Çift Ödül Rozetleri:** Nakit ve kripto ödülleri yan yana şık ve okunaklı rozetlerde sergilenir (`+$100.00 + 10.00 SOL`).
  - **Kilitli Tesis Göstergesi:** Henüz satın alınmamış tesislerin görevleri koyu kırmızı/kehribar kilit rozetiyle (`[KİLİTLİ - İzlanda Tesisi Satın Alındığında Açılır]`) gösterilir ve oyuncuya net bir hedef sunar.
  - **Akıcı Kaydırma (Scroll):** Çok sayıda görev sorunsuz biçimde fare tekerleğiyle taranabilir.

---

## 27. UI TEMİZLİĞİ (PARANTEZSİZ SADE ARAYÜZ), 2 KATLI BORSA IZGARASI, HIZLI KAZIM DENGESİ, SI BİRİM DÖNÜŞÜMÜ (T / G / M), AYRIK CPU/GPU KAZIMI VE GERÇEK DÜNYA SAATLİ ÇEVRİMDIŞI (AFK) MADENCİLİK (v2.6.0)

### 27.1. Problem ve Kullanıcı Talebi
* **Kullanıcı Talebi:**
  *"kanki bu uyugulamamızla birlikte burada sağa doğru cıkmıs UI kısmı bok gibi olsun birde üst menülerde [] parantezler içerisinde birşeyler yazma cok kötü duruyor neyse direkt onu yaz daha kaliteli dursun program birde kanki Kazım gücü her makina aldığında artıyorda cok ama cok yavas crypto kazıyoruz bunu daha efektif bir hala nasısl getireibliriz mesela kullanıcı oyunda değpilkjen bile rigler calısmaya devam etsin birde biz bu CPUları aldıkta bunlarla neler kazılıyor sanıırm bir hata var hem GPU hemde CPU ile XLM mi ne kazılıyor mesela bunlara dikkat et amk birde riglerde kac T kazdığını vs göster birde atıuyorum 1293MH mı ne oluyor ya onu trilyona cevir T olsun 1000 den snra ekstra yeni harf gelsin anladın sen onu sonrasında riglerin saaatlik vede gündelik kazanclarını alıp böyle bir sistem yapalım UI kısmında görsün oyuncu bunları birde bu oyunda yokken kazım işini nasıl yapıcaz suanki girdiği vakti dünya saati bazında alırız sonra saat hesa..."*

* **Çözülen 7 Temel Madde:**
  1. **Borsa Modalında Sağa Taşma ve Çakışma Düzeltmesi:** 8 kripto para tek satıra dizildiğinde sağ pencere çerçevesinden dışarı taşıyordu ve CANDLESTICK/LINE mod butonlarıyla çakışıyordu.
  2. **Köşeli Parantez `[...]` Temizliği:** Üst HUD, butonlar, rozetler ve rig başlıklarındaki tüm `[MARKET]`, `[BORSA]`, `[CASH]`, `[SPEED]`, `[S]`, `[R]`, `[M]`, `[*]`, `[SICAK]` ibareleri kaldırılarak temiz, modern ve kurumsal bir tasarım diline geçildi.
  3. **Aşırı Yavaş Kazım Hissiyatı & Zorluk Eğrisi Revizyonu:** Daha önce uygulanan lineer zorluk ölçeklemesi (`effectiveHash * 160.0`) oyuncu GPU aldıkça kazımı anlamsız şekilde yavaşlatıyordu. Bu formül karekök tabanlı (`std::sqrt(effectiveHash) * 45.0`) alt-lineer ölçeklemeye ve yükseltilmiş blok çarpanlarına dönüştürülerek donanım yatırımlarının anında hissedilir derecede ödüllendirici olması sağlandı.
  4. **Ayrık GPU ve CPU Kazım Hedefleri:** GPU ile kazılan coinler (TEX, RVN, ETC, ETHW, BTC, SOL) ile CPU ile kazılan coinler (XMR, RTM) tamamen birbirinden ayrıldı. Borsa ekranında seçilen coine göre buton dinamik olarak `GPU ILE KAZ` / `CPU ILE KAZ` durumuna geçer.
  5. **SI Birim Hashrate Biçimlendirmesi (T, G, M, K, P, E):** 1000 MH/s aşıldığında GH/s, 1000 GH/s aşıldığında kullanıcının talep ettiği Trilyon / TeraHash seviyesi ("T" veya "TH/s"), ardından PetaHash ("P") ve ExaHash ("E") otomatik formatlanır.
  6. **Saatlik ve Günlük Rig Kazanç/Gider Telemetri Şeridi:** Aktif rig panelinde `KAZANC: +$X.XX / sa | +$Y.YY / gun`, `ELEKTRIK: -$Z.ZZ / gun` ve `NET KAR: +$W.WW / gun` canlı telemetri şeridi eklendi.
  7. **Gerçek Dünya Saatli Çevrimdışı (AFK) Madencilik Motoru:** Oyun kapalıyken geçen gerçek dünya süresi Unix epoch timestamp (`std::time(nullptr)`) ile tespit edilir. 24 saate kadar açık riglerin elektrik tüketimi ve GPU/CPU kazımları simüle edilerek oyuncu geri döndüğünde şık bir "Hasat Raporu" (`OfflineEarningsModal`) ile karşılanır.

---

### 27.2. Kripto Borsa Modalı 2 Katlı Dinamik Izgara (`CryptoExchangeModal`)
* **Düzen:** Tek satırlık 8 butonluk taşan yapı kaldırıldı; 2 satır 4 sütunluk (2x4) mükemmel hizalı bir ızgara oluşturuldu:
  - **1. Satır (GPU):** TEX, RVN, ETC, ETHW
  - **2. Satır (GPU & CPU):** BTC, SOL, XMR (CPU), RTM (CPU)
* **Grafik & İstatistik Çakışması:** CANDLESTICK ve LINE butonları grafiğin sağ üst köşesine estetik şekilde çekildi; algoritma bilgisi, zorluk ve 24 saatlik fiyat aralıkları butonların altına yerleştirildi.
* **Akıllı Kazım Butonu:**
  - GPU coini seçildiğinde: `GPU ILE KAZ` / `GPU KAZIMI AKTIF` (Neon Mavi).
  - CPU coini (XMR, RTM) seçildiğinde: `CPU ILE KAZ` / `CPU KAZIMI AKTIF` (Neon Sarı).

---

### 27.3. UI Tipografi ve Parantezlerin Kaldırılması
* **Üst Menü ve Butonlar:**
  - `[MARKET]` -> `DONANIM VE TESIS MARKETI`
  - `[BORSA]` -> `KRIPTO BORSASI - AL / SAT`
  - `[R] AR-GE (Tekno & IPO)` -> `AR-GE & IPO`
  - `[M] DÜNYA HARİTASI` -> `DÜNYA HARİTASI`
  - `[*] GÖREVLER` -> `GÖREVLER`
  - `[HABERLER]` -> `HABERLER`
  - `[S] KAYDET` -> `KAYDET`
* **HUD Rozetleri:** `[NAKİT]`, `[CRYPTO]`, `[PİYASA]`, `[HASH]` gibi köşeli parantez etiketleri sade ve kaliteli metin formatına çevrildi.
* **Rig Başlığı ve Durumlar:** `[! SALTER ATTI !]` -> `! SALTER ATTI !`, `[- Celik Pro Sasi]` -> `- Celik Pro Sasi`.

---

### 27.4. SI Prefix Hashrate Formatlayıcı & Hızlı Kazım Matematiği
* **Fonksiyonlar (`EconomyManager.hpp` / `.cpp`):**
  - `FormatHashrate(double mhs)`:
    * `< 1.0`: `XX.X KH/s`
    * `< 1,000.0`: `XX.X MH/s`
    * `< 1,000,000.0`: `XX.X GH/s`
    * `< 1,000,000,000.0`: `XX.X TH/s` (Kullanıcının talep ettiği 'T' trilyon seviyesi)
    * `< 1,000,000,000,000.0`: `XX.X PH/s` ('P')
    * `EH/s` ('E')
  - `FormatCPUHashrate(double khs)`: KH/s, MH/s, GH/s, TH/s dönüşümleri.
* **Kazım Hızı ve Ödül Dengesi:**
  - `MineCoins` formülü sublinear karekök ölçekleme ile güncellendi: Çok sayıda kart takıldığında zorluk duvara çarpmadan hashrate ile orantılı blok çözümü yapılır.
  - Saatlik ve günlük getiri tahmin fonksiyonları (`CalculateRigHourlyRevenueUSD`, `CalculateRigDailyRevenueUSD`, `CalculateRigDailyProfitUSD`) gerçek anlık piyasa fiyatlarıyla entegre edildi.

---

### 27.5. Gerçek Dünya Saatli Çevrimdışı (AFK) Madencilik & Hasat Modalı
* **Epoch Zaman Damgası (`SaveManager.hpp` / `.cpp`):**
  - Oyun kaydedildiğinde `[HEADER]` bölümüne `epochTimestamp=<unix_time>` yazılır.
  - Oyun açıldığında `currentEpoch - savedEpoch` hesaplanır.
  - Eğer fark 10 saniyeden fazlaysa (maksimum 24 saat = 86.400 saniye sınırı ile) simülasyon başlatılır:
    * Çalışır durumdaki tüm riglerin GPU ve CPU kazımları işletilir.
    * Tesisin kWh elektrik birim fiyatına göre elektrik maliyeti hesaplanır ve şirketin nakit kasasından düşülür (kasada para biterse rigler durur).
    * Kazanılan coinler cüzdan bakiyelerine eklenir.
    * `OfflineMiningReport` nesnesi doldurulur.
* **Hasat Raporu Modalı (`OfflineEarningsModal.hpp` / `.cpp`):**
  - Oyuncu oyuna girdiğinde karşısına şık, yarı saydam altın-siyan çerçeveli AFK karşılama ekranı açılır:
    * Çevrimdışı Kalınan Süre (saat, dakika, saniye).
    * Aktif Çalışan Rig Sayısı.
    * GPU Madencilik Hasatı (Coin miktarı ve USD karşılığı).
    * CPU Madencilik Hasatı (XMR/RTM miktarı ve USD karşılığı).
    * Harcanan Elektrik Faturası (-$XX.XX).
    * Net Elde Edilen Kar (+$XX.XX).
    * `[HASILATI TOPLA VE DEVAM ET]` aksiyon butonu.

---

## 28. İPEKSİ TRILINEAR MIPMAP FONT RENDER, BÜYÜTÜLMÜŞ MARKET & HUD ARAYÜZÜ, ÇAKIŞMA ÖNLEYİCİ AKILLI METİN VE KALİBRE EDİLMİŞ AYARLAR ÖLÇEĞİ (v2.6.1)

### 28.1. Kullanıcı Geri Bildirimi ve Problemler
* **Kullanıcı Talebi:**
  *"krall bak yazılar cok ama cok kücük duruyor oyunu ilk defa actıklarında yazılar biraz daha büyük olması gerekiyor cok kücük duruyor birde buradaki yazıların kalitelerini arttırmak lazım kral birde ekstradan ayarlar kısmındaki bölmeden UI kısmın büyültüp kücülttüğüm zaman yazılar birbirine giriyor bunlarıda kontrol edip yeni bir ayar cekmen gerekiyor"*
* **Tespit Edilen 3 Kritik Aksaklık:**
  1. **Küçük Yazılar:** Oyun ilk açıldığında `MarketModal` donanım kartları, barlar, butonlar ve HUD etiketleri 10-13px taban boyutlarında kalarak yüksek çözünürlüklü tam ekranda küçük görünüyordu.
  2. **Yazı Kalitesi & Pikselleşme:** Segoe UI 48px font atlasından 11-13px boyutuna downscale yapılırken `GenTextureMipmaps` çağrılmadığı ve sadece `TEXTURE_FILTER_BILINEAR` kullanıldığı için OpenGL'de GL_LINEAR minification sonucu harf çizgileri kayboluyor, parıldıyor ve piksel piksel kırılıyordu.
  3. **Ölçekleme Çakışmaları:** Ayarlar menüsünden UI ölçeği (%150, %175, %200) artırıldığında, yalnızca font büyüklüğü artıyor; ancak buton yükseklikleri, satır aralıkları ve rozet konumları sabit kaldığı için başlıklar alt başlıkların üstüne biniyor, butonlardan taşıyor ve metinler birbirine giriyordu.

---

### 28.2. İpeksi Pürüzsüz Font Kalitesi & Trilinear Mipmap Filtreleme (`main.cpp`)
* **56px Ultra Yüksek Çözünürlüklü Vektör Atlası:**
  - Segoe UI, Inter ve Arial fontları 56px ultra yüksek çözünürlükte rasterize edildi.
  - Kod noktalarına Türk Lirası (`₺` / 0x20BA), Tether (`₮` / 0x20AE), Euro (`€` / 0x20AC), menü okları ve onay işaretleri eklendi.
* **Trilinear Mipmapping (`GenTextureMipmaps` & `TEXTURE_FILTER_TRILINEAR`):**
  - Font dokusu yüklendikten sonra OpenGL mipmap piramidi (`GenTextureMipmaps`) oluşturuldu.
  - Doku filtrelemesi `TEXTURE_FILTER_TRILINEAR` (`GL_LINEAR_MIPMAP_LINEAR`) moduna geçirildi.
  - Sonuç: 56px'ten 11px veya 14px'e küçültülürken hiçbir harf çizgisi incelip kaybolmaz, harf kenarlarında pikselleşme veya titreme sıfıra iner; ipeksi pürüzsüzlükte kristal netliğinde tipografi elde edildi.

---

### 28.3. Donanım Marketi & HUD Tipografi Büyütmeleri (`MarketModal.cpp`, `main.cpp`)
* **Ferah ve Geniş Kart Tasarımı (`MarketModal`):**
  - Kart yüksekliği (`cardH`): **74px -> 88px** seviyesine çıkarıldı.
  - GPU & CPU İsimleri: **18px Bold** yapıldı (önceden 16px).
  - Tier ve Algoritma Etiketleri: **13px** yapıldı (önceden 11px).
  - Gerekli Tesis Seviyesi: **12.5px** yapıldı.
  - Performans Barları (`barH`): **13px -> 17px** seviyesine büyütüldü (`barGap = 5px`). Metinler bar içinde dinamik dikey ortalandı (`12.5px`).
  - Satın Alma Butonu: **36px -> 42px yükseklik, 155px genişlik** ile belirginleştirildi.
  - Fiyat Etiketi: **20px Bold** yeşil olarak büyütüldü.
  - Tier Filtre Butonları: **26px -> 34px** seviyesine çıkarıldı.
  - Kategori Sekmeleri: **38px -> 42px** seviyesine çıkarıldı.
* **Ana Ekran HUD & Alt Bilgi:**
  - Üst rozet etiketleri (`labelSize`): **14.5px** yapıldı.
  - Üst rozet değerleri (`valSize`): **21px - 24px Bold** yapıldı.
  - Alt bilgi ipucu (`TIP_FOOTER`): **14.5px** ve yüksek kontrastlı parlak renkle (`Color{215, 230, 255, 255}`) okunaklı kılındı.

---

### 28.4. Çakışma Önleyici Akıllı Metin ve Dikey Ortalama (`UIButton.cpp`, `UIFrame.cpp`)
* **Butonlarda Sıfır Çakışma Garantisi (`UIButton::Draw`):**
  - Başlık ve alt başlığın toplam çizim yüksekliği (`totalH = titleDrawnH + gap + subDrawnH`) hesaplanır.
  - Eğer toplam yükseklik buton alanını aşarsa (`totalH > bounds.height - 6px`), her iki metin otomatik olarak tam sığacak oranda küçültülür (`vRatio = availH / totalH`).
  - Dikey konumlandırma matematiksel olarak ortalanır:
    `startY = bounds.y + (bounds.height - totalH) * 0.5f`
    `titleY = startY`
    `subY = startY + titleDrawnH + gap`
  - Bu formül sayesinde UI ölçeği ne kadar büyütülürse büyütülsün başlık ve alt başlığın üst üste binmesi veya buton kenarından taşması imkansız hale getirildi.
  - Tek satırlı butonlar dikey ve yatayda kusursuz ortalandı.
* **Rozetlerde Çakışma Önleme (`UIFrame::DrawStatBadge`):**
  - İkon/Etiket ve Değer metinleri arasındaki dikey mesafe dinamik hesaplanır. Toplam yükseklik rozet sınırını aşarsa otomatik auto-fit küçültme uygulanır.

---

### 28.5. Kalibre Edilmiş Ayarlar UI Ölçek Menüsü (`SettingsModal.cpp`, `UIFrame.hpp`)
* **Varsayılan Ölçek:** `s_uiScale` taban değeri `1.30f` (%130) olarak ayarlandı; oyun ilk kez açıldığında dahi arayüz geniş, büyük ve son derece rahat okunur.
* **5 Kademeli Optimize Ölçek Adımları:**
  - `%100 (1.00x) Standart` - Kompakt ekranlar
  - `%115 (1.15x) Orta` - Dengeli boyut
  - `%130 (1.30x) Büyük [Önerilen / Varsayılan]`
  - `%145 (1.45x) Çok Büyük` - Geniş monitörler
  - `%160 (1.60x) Maksimum` - Ultra net font
* **Görsel Geri Bildirim:** Aktif seçili ölçek butonu parlak altın sarısı bir çerçeveyle işaretlenerek oyuncunun hangi boyutta olduğunu anında görmesi sağlandı.

