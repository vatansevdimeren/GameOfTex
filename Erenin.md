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

---

## 🎨 2. Görselleştirme, Dokular ve Ayarlar Katmanı (`src/render/`)

### 2.6. `TextureManager.hpp` & `TextureManager.cpp` (Ultra Çözünürlüklü Doku Yöneticisi)
* **Görevi:** `assets/textures/` klasöründen harici yüksek çözünürlüklü PNG/JPG resimlerini yükler ve filtreler.
* `gpu_card.png`: Ekran kartı gövdesine giydirilen ultra-res doku.
* `fan_blade.png`: Isıya göre dönen fan pervanesi görseli.
* `warehouse_bg.png`: Tesis arka plan resmi.
* **SRP & Fallback:** Görseller bulunamazsa oyun asla çökmez; otomatik olarak keskin procedürel 2D çizim motorunu devreye sokar.

### 2.7. `SettingsModal.hpp` & `SettingsModal.cpp` (Ayarlar ve UI Scale Penceresi)
* **Görevi:** Kullanıcının ekran boyutuna ve göz zevkine göre yazı ve arayüz ölçeğini anında büyütmesini sağlar.
* `UI Scale Seviyeleri`: 1.0x (Normal), 1.25x (Büyük), 1.50x (Çok Büyük), 1.75x (Dev), 2.0x (Ultra).
* `ToggleFullscreen()`: Tek tıkla tam ekran moduna geçirir.

---

## 🚀 3. Ana Döngü ve Oyun Akışı (`src/main.cpp`)

* `GameState::LOGIN`: Oyun ilk açıldığında `LoginScreen` devreye girer. Kullanıcı şirket adını yazıp avatarını seçtikten sonra hesabı oluşturulur ve **+\$1,000 hoş geldin bonusu** hesabına geçer.
* `GameState::GAMEPLAY`: 
  * **[AYARLAR] Butonu:** Sağ üst köşedeki butonla açılır; font boyutunu ve UI ölçeğini 2.0x seviyesine kadar büyütebilirsin.
  * **Çoklu Rig & Depo Gezintisi:** Sol panelde `[< ONCEKI]` ve `[SONRAKI >]` butonlarıyla depodaki raflar arasında geçiş yapabilir, seçili rig'e yeni GPU'lar takabilirsin.
  * **`[ 🏗️ YENI RIG SATIN AL ($2,500) ]`**: Depoyu genişleterek yeni bir raf satın alır.
  * **[F11] Tam Ekran Desteği:** Tek tuşla ekranı doldurur.
  * Sağ panelde tıklanabilir 8 adet modern ve dinamik buton.
