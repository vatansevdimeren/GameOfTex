# 🎮 GameOfTex - Crypto Mining & Hardware Empire (v1.6.0)

2D Idle & Hardware Tycoon simülasyonu. Kendi kripto para madencilik tesisinizi kurun, rig'lerinizi yönetin, ekran kartlarınızı 360 derece inceleyin ve overclock/undervolt ayarlarıyla sınırları zorlayın!

---

## ⚡ Oyunu Hemen İndir ve Oyna (Kurulumsuz / Portable)

Arkadaşlarınızın oyunu oynaması için bilgisayarlarında Visual Studio, C++ veya CMake kurulu olmasına **KESİNLİKLE GEREK YOKTUR**!

* 📥 **[BURAYA TIKLAYARAK EN GÜNCEL SÜRÜMÜ İNDİRİN (GameOfTex-Windows.zip)](https://github.com/vatansevdimeren/GameOfTex/releases/download/v1.6.0/GameOfTex-Windows.zip)**
* 📦 Veya GitHub Releases sekmesinden: [GitHub Releases v1.6.0](https://github.com/vatansevdimeren/GameOfTex/releases/tag/v1.6.0)

### 🕹️ Nasıl Çalıştırılır?
1. İndirdiğiniz `GameOfTex-Windows.zip` dosyasını sağ tıklayıp bir klasöre çıkartın.
2. Klasör içindeki **`GameOfTex.exe`** dosyasına çift tıklayın.
3. *(Eğer Windows SmartScreen uyarısı verirse: "Ek Bilgi" -> "Yine de Çalıştır" deyin).*

---

## ✨ Temel Özellikler

* **🌐 Türkçe 🇹🇷 & İngilizce 🇺🇸 Dil Desteği:** Ayarlar menüsünden tek tıkla dil değiştirme.
* **💱 Çoklu Para Birimi:** USD ($), USDT (₮), Türk Lirası (₺) ve Euro (€) dinamik parite desteği.
* **🔍 360° 2.5D GPU İnceleme Paneli:** Kasanızdaki herhangi bir karta tıklayarak fareyle kartı 360 derece döndürebilir, gerçekçi fan animasyonlarını izleyebilir, saat hızı, voltaj ve güç limitlerini ayarlayabilirsiniz.
* **🔥 Gerçekçi Termodinamik & Yangın Simülasyonu:** 140°C'yi geçen kartlar duman ve kor parçacıkları saçarak yanar! Yanan kartları tamir edebilir veya hurdaya satabilirsiniz (+$75).
* **🏭 Depo Kuşbakışı Genel Bakış (Overview Matrix):** Tüm madencilik hangarlarınızı server-blade raf kartları halinde izleyebilir, rig'leri uzaktan açıp kapatabilir veya elden çıkarabilirsiniz.
* **⚡ Sigorta & Şebeke Yönetimi:** Aşırı yüklenmede atan şalter mekaniği.
* **💻 Akıllı Çözünürlük & ESC Davranışı:** F11 ile tam ekran, ESC ile güvenli pencere modu.

---

## 🛠️ Geliştiriciler İçin (Kaynak Koddan Derleme)

Projeyi sıfırdan derlemek isterseniz:
* **Gereksinimler:** C++20 destekli MSVC / GCC / Clang, CMake 3.20+
* Raylib 5.0 kütüphanesi `FetchContent` ile CMake tarafından otomatik indirilir ve derlenir.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

---

## 📖 Kod Mimarisi & Fonksiyon Detayları

Tüm fonksiyonların SRP (Single Responsibility Principle) ve Clean Code gerekçeleri için [Erenin.md](Erenin.md) dosyasını inceleyebilirsiniz.
