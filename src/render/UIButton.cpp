#include "UIButton.hpp"
#include "UIFrame.hpp"
#include <algorithm>

namespace Render {

UIButton::UIButton()
    : m_bounds{0, 0, 100, 40}
    , m_title("Button")
    , m_subtitle("")
    , m_baseColor(Color{35, 40, 52, 255})
    , m_accentColor(Color{0, 200, 255, 255})
    , m_isHovered(false)
    , m_isPressed(false)
    , m_isDisabled(false)
{
}

UIButton::UIButton(Rectangle bounds, std::string title, std::string subtitle,
                   Color baseColor, Color accentColor)
    : m_bounds(bounds)
    , m_title(std::move(title))
    , m_subtitle(std::move(subtitle))
    , m_baseColor(baseColor)
    , m_accentColor(accentColor)
    , m_isHovered(false)
    , m_isPressed(false)
    , m_isDisabled(false)
{
}

void UIButton::SetBounds(Rectangle bounds) {
    m_bounds = bounds;
}

Rectangle UIButton::GetBounds() const {
    return m_bounds;
}

void UIButton::SetTitle(std::string title) {
    m_title = std::move(title);
}

void UIButton::SetSubtitle(std::string subtitle) {
    m_subtitle = std::move(subtitle);
}

void UIButton::SetDisabled(bool disabled) {
    m_isDisabled = disabled;
}

bool UIButton::IsDisabled() const {
    return m_isDisabled;
}

void UIButton::SetAccentColor(Color color) {
    m_accentColor = color;
}

void UIButton::SetBaseColor(Color color) {
    m_baseColor = color;
}

bool UIButton::UpdateAndCheckClick() {
    if (m_isDisabled) {
        m_isHovered = false;
        m_isPressed = false;
        return false;
    }

    Vector2 mousePos = GetMousePosition();
    m_isHovered = CheckCollisionPointRec(mousePos, m_bounds);
    m_isPressed = m_isHovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    return m_isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

void UIButton::Draw() const {
    // Renk hesaplama: Disabled -> soluk, Hovered -> parlak, Pressed -> koyu
    Color fillColor = m_baseColor;
    Color borderColor = m_accentColor;
    Color textColor = WHITE;
    Color subTextColor = LIGHTGRAY;

    if (m_isDisabled) {
        fillColor = Color{25, 28, 35, 180};
        borderColor = Color{60, 65, 75, 150};
        textColor = Color{120, 125, 135, 255};
        subTextColor = Color{90, 95, 105, 255};
    } else if (m_isPressed) {
        fillColor = Color{
            static_cast<unsigned char>(std::clamp(static_cast<int>(m_baseColor.r) - 10, 0, 255)),
            static_cast<unsigned char>(std::clamp(static_cast<int>(m_baseColor.g) - 10, 0, 255)),
            static_cast<unsigned char>(std::clamp(static_cast<int>(m_baseColor.b) - 10, 0, 255)),
            255
        };
        borderColor = WHITE;
    } else if (m_isHovered) {
        fillColor = Color{
            static_cast<unsigned char>(std::clamp(static_cast<int>(m_baseColor.r) + 20, 0, 255)),
            static_cast<unsigned char>(std::clamp(static_cast<int>(m_baseColor.g) + 20, 0, 255)),
            static_cast<unsigned char>(std::clamp(static_cast<int>(m_baseColor.b) + 25, 0, 255)),
            255
        };
        borderColor = Color{
            static_cast<unsigned char>(std::clamp(static_cast<int>(m_accentColor.r) + 30, 0, 255)),
            static_cast<unsigned char>(std::clamp(static_cast<int>(m_accentColor.g) + 30, 0, 255)),
            static_cast<unsigned char>(std::clamp(static_cast<int>(m_accentColor.b) + 30, 0, 255)),
            255
        };
    }

    // Buton gövdesi
    DrawRectangleRounded(m_bounds, 0.2f, 4, fillColor);
    DrawRectangleRoundedLines(m_bounds, 0.2f, 4, 1.5f, borderColor);

    // Hover anında sol kenarda küçük parlak neon şerit
    if (m_isHovered && !m_isDisabled) {
        DrawRectangle(static_cast<int>(m_bounds.x + 4),
                      static_cast<int>(m_bounds.y + 6),
                      3, static_cast<int>(m_bounds.height - 12), m_accentColor);
    }

    // Metin hizalama ve akıllı boyutlandırma (Taşma ve üst üste binmeyi engeller)
    const float padX = (m_bounds.width < 110.0f) ? 8.0f : 14.0f;
    const float maxW = std::max(10.0f, m_bounds.width - (padX * 2.0f));
    const float textX = m_bounds.x + padX;

    if (!m_subtitle.empty()) {
        float titleSize = (m_bounds.height < 42.0f) ? 12.0f : 16.0f;
        float subSize = (m_bounds.height < 42.0f) ? 10.0f : 12.0f;

        float tW = UIFrame::MeasureTextCustom(m_title, titleSize, true);
        if (tW > maxW && tW > 0.0f) titleSize = std::max(9.0f, titleSize * (maxW / tW));

        float sW = UIFrame::MeasureTextCustom(m_subtitle, subSize, false);
        if (sW > maxW && sW > 0.0f) subSize = std::max(8.0f, subSize * (maxW / sW));

        float titleY = (m_bounds.height < 42.0f)
            ? (m_bounds.y + (m_bounds.height * 0.18f))
            : (m_bounds.y + (m_bounds.height / 2.0f) - titleSize - 1.0f);
        float subY = (m_bounds.height < 42.0f)
            ? (m_bounds.y + (m_bounds.height * 0.56f))
            : (m_bounds.y + (m_bounds.height / 2.0f) + 3.0f);

        UIFrame::DrawTextCustom(m_title, textX, titleY, titleSize, textColor, true);
        UIFrame::DrawTextCustom(m_subtitle, textX, subY, subSize, subTextColor, false);
    } else {
        float titleSize = (m_bounds.height < 34.0f) ? 13.0f : ((m_bounds.height < 44.0f) ? 15.0f : 18.0f);
        float tW = UIFrame::MeasureTextCustom(m_title, titleSize, true);
        if (tW > maxW && tW > 0.0f) titleSize = std::max(10.0f, titleSize * (maxW / tW));

        float titleY = m_bounds.y + (m_bounds.height / 2.0f) - (titleSize * 0.55f);
        UIFrame::DrawTextCustom(m_title, textX, titleY, titleSize, textColor, true);
    }
}

} // namespace Render
