#include "UIFrame.hpp"
#include <algorithm>
#include <cmath>

namespace Render {

void UIFrame::InitTheme(Font fontRegular, Font fontBold) {
    s_fontRegular = fontRegular;
    s_fontBold = fontBold;
    s_hasCustomFonts = (fontRegular.texture.id > 0);
}

Font UIFrame::GetFontRegular() {
    return s_fontRegular;
}

Font UIFrame::GetFontBold() {
    return s_fontBold;
}

void UIFrame::SetUIScale(float scale) {
    s_uiScale = std::clamp(scale, 0.8f, 2.2f);
}

float UIFrame::GetUIScale() {
    return s_uiScale;
}

void UIFrame::DrawTextCustom(const std::string& text, float x, float y, float size, Color color, bool bold) {
    const float scaledSize = size * s_uiScale;
    const float snapX = std::round(x);
    const float snapY = std::round(y);
    if (s_hasCustomFonts) {
        Font fontToUse = bold ? s_fontBold : s_fontRegular;
        // Text drop-shadow: Yalnizca buyuk basliklar icin (>= 18px) hafif golge
        if (scaledSize >= 18.0f && color.a > 50) {
            Color shadowColor{0, 0, 0, static_cast<unsigned char>(color.a * 0.35f)};
            DrawTextEx(fontToUse, text.c_str(), Vector2{snapX + 1.0f, snapY + 1.0f}, scaledSize, 0.5f, shadowColor);
        }
        DrawTextEx(fontToUse, text.c_str(), Vector2{snapX, snapY}, scaledSize, 0.5f, color);
    } else {
        if (color.a > 30) {
            Color shadowColor{0, 0, 0, static_cast<unsigned char>(color.a * 0.50f)};
            DrawText(text.c_str(), static_cast<int>(snapX + 1.0f), static_cast<int>(snapY + 1.0f), static_cast<int>(scaledSize), shadowColor);
        }
        DrawText(text.c_str(), static_cast<int>(snapX), static_cast<int>(snapY), static_cast<int>(scaledSize), color);
    }
}

float UIFrame::MeasureTextCustom(const std::string& text, float size, bool bold) {
    const float scaledSize = size * s_uiScale;
    if (s_hasCustomFonts) {
        Font fontToUse = bold ? s_fontBold : s_fontRegular;
        return MeasureTextEx(fontToUse, text.c_str(), scaledSize, 0.5f).x;
    } else {
        return static_cast<float>(MeasureText(text.c_str(), static_cast<int>(scaledSize)));
    }
}

void UIFrame::DrawCard(Rectangle bounds, const std::string& title, Color accent) {
    // Yumuşak kart arka gölgesi (Ambient card shade)
    DrawRectangleRounded(Rectangle{bounds.x + 3.0f, bounds.y + 4.0f, bounds.width, bounds.height}, 0.03f, 6, Color{0, 0, 0, 90});

    // Koyu yarı saydam gövde (Glassmorphism etkisi)
    DrawRectangleRounded(bounds, 0.03f, 6, Color{18, 22, 30, 245});
    // Belirgin neon hatlı kenarlık
    DrawRectangleRoundedLines(bounds, 0.03f, 6, 1.8f, Color{45, 55, 75, 255});

    // Kart başlığı varsa üst bant çiz
    if (!title.empty()) {
        DrawRectangle(static_cast<int>(bounds.x + 16), static_cast<int>(bounds.y + 16),
                      4, 24, accent);

        DrawTextCustom(title, bounds.x + 28, bounds.y + 14, 20.0f, RAYWHITE, true);

        DrawLine(static_cast<int>(bounds.x + 16), static_cast<int>(bounds.y + 46),
                 static_cast<int>(bounds.x + bounds.width - 16), static_cast<int>(bounds.y + 46),
                 Color{38, 46, 62, 255});
    }
}

void UIFrame::DrawProgressBar(Rectangle bounds, float progressRatio, Color barColor, const std::string& labelText) {
    const float clampedRatio = std::clamp(progressRatio, 0.0f, 1.0f);

    // Arka plan kanalı
    DrawRectangleRounded(bounds, 0.35f, 6, Color{22, 26, 36, 255});
    DrawRectangleRoundedLines(bounds, 0.35f, 6, 1.5f, Color{50, 60, 80, 255});

    // Doldurulan bar
    if (clampedRatio > 0.01f) {
        Rectangle fillBounds = bounds;
        fillBounds.width *= clampedRatio;
        DrawRectangleRounded(fillBounds, 0.35f, 6, barColor);
    }

    // Büyük ve okunaklı ortalanmış metin
    if (!labelText.empty()) {
        float textWidth = MeasureTextCustom(labelText, 16.0f, true);
        float textX = bounds.x + (bounds.width / 2.0f) - (textWidth / 2.0f);
        float textY = bounds.y + (bounds.height / 2.0f) - 9.0f;
        DrawTextCustom(labelText, textX, textY, 16.0f, WHITE, true);
    }
}

void UIFrame::DrawStatBadge(float x, float y, float width, float height,
                           const std::string& icon, const std::string& label,
                           const std::string& value, Color valueColor) {
    Rectangle badgeBounds{x, y, width, height};
    
    DrawRectangleRounded(badgeBounds, 0.2f, 6, Color{20, 24, 34, 240});
    DrawRectangleRoundedLines(badgeBounds, 0.2f, 6, 1.4f, Color{45, 56, 76, 220});

    const float padX = 12.0f;
    const float availW = std::max(20.0f, width - (padX * 2.0f));

    // İkon ve Başlık
    std::string fullLabel = icon.empty() ? label : (icon + " " + label);
    float labelSize = 13.0f;
    float labelWidth = MeasureTextCustom(fullLabel, labelSize, false);
    if (labelWidth > availW && labelWidth > 0.0f) {
        labelSize = std::max(9.5f, labelSize * (availW / labelWidth));
    }
    float labelY = y + (height * 0.14f);
    DrawTextCustom(fullLabel, x + padX, labelY, labelSize, Color{150, 165, 190, 255}, false);

    // Değer Metni (Genişlik ve yüksekliğe göre mükemmel dikey konumlandırma)
    float valSize = (height < 50.0f) ? 18.0f : 21.0f;
    float valWidth = MeasureTextCustom(value, valSize, true);
    if (valWidth > availW && valWidth > 0.0f) {
        valSize = std::max(11.5f, valSize * (availW / valWidth));
    }
    float valY = y + (height * 0.48f);
    DrawTextCustom(value, x + padX, valY, valSize, valueColor, true);
}

void UIFrame::DrawTextInput(Rectangle bounds, const std::string& text, bool isActive, const std::string& placeholder) {
    Color bg = isActive ? Color{26, 32, 45, 255} : Color{18, 22, 30, 255};
    Color border = isActive ? Color{0, 230, 255, 255} : Color{60, 70, 90, 255};

    DrawRectangleRounded(bounds, 0.15f, 6, bg);
    DrawRectangleRoundedLines(bounds, 0.15f, 6, 2.0f, border);

    if (text.empty()) {
        DrawTextCustom(placeholder, bounds.x + 16, bounds.y + (bounds.height / 2.0f) - 11.0f, 22.0f, GRAY, false);
    } else {
        DrawTextCustom(text, bounds.x + 16, bounds.y + (bounds.height / 2.0f) - 11.0f, 22.0f, RAYWHITE, true);
    }

    // Yanıp sönen imleç (Cursor)
    if (isActive) {
        float textWidth = MeasureTextCustom(text, 22.0f, true);
        DrawRectangle(static_cast<int>(bounds.x + 18 + textWidth),
                      static_cast<int>(bounds.y + 10),
                      3, static_cast<int>(bounds.height - 20), Color{0, 230, 255, 255});
    }
}

} // namespace Render
