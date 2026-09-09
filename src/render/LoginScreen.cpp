#include "LoginScreen.hpp"
#include "UIFrame.hpp"
#include <algorithm>

namespace Render {

LoginScreen::LoginScreen()
    : m_inputText("Eren Mining Co.")
    , m_selectedAvatar(0)
    , m_inputActive(false)
    , m_submitButton(Rectangle{0, 0, 100, 50}, "HESAP AC & MADENCILIGE BASLA", "+$1,000 HOS GELDIN HEDIYESI!",
                     Color{0, 140, 90, 255}, Color{0, 255, 150, 255})
    , m_btnCancel(Rectangle{0, 0, 100, 50}, "GERI DON", "Ana Menu",
                  Color{45, 25, 30, 255}, Color{255, 80, 80, 255})
    , m_inputBoxRect{0, 0, 100, 50}
    , m_avatar1Rect{0, 0, 100, 50}
    , m_avatar2Rect{0, 0, 100, 50}
    , m_avatar3Rect{0, 0, 100, 50}
{
}

LoginAction LoginScreen::Update(Core::UserProfile& profile) {
    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    // Dinamik responsive ve ortalanmış kart boyutları (Daha Ferah ve Belirgin)
    const float cardW = std::clamp(screenW * 0.52f, 620.0f, 800.0f);
    const float cardH = std::clamp(screenH * 0.86f, 620.0f, 780.0f);
    const float cardX = (screenW - cardW) / 2.0f;
    const float cardY = (screenH - cardH) / 2.0f;

    const float innerX = cardX + 45.0f;
    const float innerW = cardW - 90.0f;

    // Dinamik koordinatlar (Daha yüksek giriş kutusu ve avatar butonları)
    m_inputBoxRect = Rectangle{innerX, cardY + 285.0f, innerW, 60.0f};

    const float avatarW = (innerW - 24.0f) / 3.0f;
    const float avatarH = 82.0f;
    const float avatarY = cardY + 412.0f;
    m_avatar1Rect = Rectangle{innerX, avatarY, avatarW, avatarH};
    m_avatar2Rect = Rectangle{innerX + avatarW + 12.0f, avatarY, avatarW, avatarH};
    m_avatar3Rect = Rectangle{innerX + (avatarW * 2.0f) + 24.0f, avatarY, avatarW, avatarH};

    const float cancelW = 140.0f;
    const float submitW = innerW - cancelW - 14.0f;
    m_submitButton.SetBounds(Rectangle{innerX, cardY + cardH - 92.0f, submitW, 68.0f});
    m_btnCancel.SetBounds(Rectangle{innerX + submitW + 14.0f, cardY + cardH - 92.0f, cancelW, 68.0f});

    Vector2 mouse = GetMousePosition();

    // Metin kutusu odaklama ve avatar seçimi
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        m_inputActive = CheckCollisionPointRec(mouse, m_inputBoxRect);

        if (CheckCollisionPointRec(mouse, m_avatar1Rect)) m_selectedAvatar = 0;
        if (CheckCollisionPointRec(mouse, m_avatar2Rect)) m_selectedAvatar = 1;
        if (CheckCollisionPointRec(mouse, m_avatar3Rect)) m_selectedAvatar = 2;
    }

    // Klavye ile metin yazımı
    if (m_inputActive) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (m_inputText.length() < 24)) {
                m_inputText.push_back(static_cast<char>(key));
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !m_inputText.empty()) {
            m_inputText.pop_back();
        }
    }

    if (m_btnCancel.UpdateAndCheckClick() || IsKeyPressed(KEY_ESCAPE)) {
        return LoginAction::CANCEL;
    }

    // Başla butonu veya Enter
    if (m_submitButton.UpdateAndCheckClick() || (m_inputActive && IsKeyPressed(KEY_ENTER))) {
        if (m_inputText.empty()) {
            m_inputText = "Eren Mining Co.";
        }
        profile.SetCompanyName(m_inputText);
        profile.SetAvatarIndex(m_selectedAvatar);
        return LoginAction::SUBMIT;
    }

    return LoginAction::NONE;
}

void LoginScreen::Draw() const {
    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());

    ClearBackground(Color{10, 12, 16, 255});

    // Dinamik arka plan ızgarası
    for (int x = 0; x < static_cast<int>(screenW); x += 50) {
        DrawLine(x, 0, x, static_cast<int>(screenH), Color{18, 22, 30, 255});
    }
    for (int y = 0; y < static_cast<int>(screenH); y += 50) {
        DrawLine(0, y, static_cast<int>(screenW), y, Color{18, 22, 30, 255});
    }

    // Dinamik ortalanmış kart
    const float cardW = std::clamp(screenW * 0.52f, 620.0f, 800.0f);
    const float cardH = std::clamp(screenH * 0.86f, 620.0f, 780.0f);
    const float cardX = (screenW - cardW) / 2.0f;
    const float cardY = (screenH - cardH) / 2.0f;

    const float innerX = cardX + 45.0f;
    const float innerW = cardW - 90.0f;

    UIFrame::DrawCard(Rectangle{cardX, cardY, cardW, cardH}, "YENI PROFIL VE HESAP OLUSTURMA", Color{0, 220, 255, 255});

    // Büyük ve okunaklı Başlıklar
    UIFrame::DrawTextCustom("GameOfTex: Crypto & Energy Tycoon", innerX, cardY + 62.0f, 30.0f, RAYWHITE, true);
    UIFrame::DrawTextCustom("Kendi Madencilik ve Enerji Imparatorlugunu Kur!", innerX, cardY + 105.0f, 18.0f, Color{140, 160, 190, 255}, false);

    // +$1,000 Bonus Kutusu
    Rectangle bonusBadge{innerX, cardY + 142.0f, innerW, 60.0f};
    DrawRectangleRounded(bonusBadge, 0.2f, 6, Color{16, 45, 35, 240});
    DrawRectangleRoundedLines(bonusBadge, 0.2f, 6, 1.8f, Color{0, 255, 150, 255});
    UIFrame::DrawTextCustom("[HEDIYE] HESAP ACILIS BONUSA: +$1,000 NAKIT", innerX + 20.0f, cardY + 160.0f, 20.0f, Color{0, 255, 150, 255}, true);

    // Şirket Adı Başlığı ve Giriş Kutusu
    UIFrame::DrawTextCustom("MADENCI / SIRKET ADINIZ:", innerX, cardY + 256.0f, 17.0f, Color{180, 195, 220, 255}, true);
    UIFrame::DrawTextInput(m_inputBoxRect, m_inputText, m_inputActive, "Sirket adinizi yazin...");

    // Avatar Seçimi
    UIFrame::DrawTextCustom("PROFIL ROZETI / AVATAR SECIN:", innerX, cardY + 380.0f, 17.0f, Color{180, 195, 220, 255}, true);

    auto drawAvatarOption = [&](Rectangle rect, int index, const char* icon, const char* label) {
        bool selected = (m_selectedAvatar == index);
        Color fill = selected ? Color{32, 52, 75, 255} : Color{20, 24, 34, 255};
        Color border = selected ? Color{0, 230, 255, 255} : Color{50, 60, 80, 255};

        DrawRectangleRounded(rect, 0.2f, 6, fill);
        DrawRectangleRoundedLines(rect, 0.2f, 6, selected ? 2.5f : 1.2f, border);

        UIFrame::DrawTextCustom(icon, rect.x + 16.0f, rect.y + 14.0f, 22.0f, selected ? Color{0, 230, 255, 255} : LIGHTGRAY, true);
        UIFrame::DrawTextCustom(label, rect.x + 16.0f, rect.y + 46.0f, 15.0f, selected ? WHITE : GRAY, false);
    };

    drawAvatarOption(m_avatar1Rect, 0, "[SIBER]", "Tech Hacker");
    drawAvatarOption(m_avatar2Rect, 1, "[SANAYI]", "Endustriyel");
    drawAvatarOption(m_avatar3Rect, 2, "[UZAY]", "Mega Santral");

    // Başla ve Geri Butonları
    m_submitButton.Draw();
    m_btnCancel.Draw();
}

} // namespace Render
