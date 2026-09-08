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











