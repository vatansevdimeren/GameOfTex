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

    // Metin hizalama (Büyük ve net fontlar)
    float textX = m_bounds.x + 18.0f;
    if (!m_subtitle.empty()) {
        float titleY = m_bounds.y + (m_bounds.height / 2.0f) - 18.0f;
        float subY = m_bounds.y + (m_bounds.height / 2.0f) + 4.0f;
        UIFrame::DrawTextCustom(m_title, textX, titleY, 18.0f, textColor, true);
        UIFrame::DrawTextCustom(m_subtitle, textX, subY, 14.0f, subTextColor, false);
    } else {
        float titleY = m_bounds.y + (m_bounds.height / 2.0f) - 10.0f;
        UIFrame::DrawTextCustom(m_title, textX, titleY, 20.0f, textColor, true);
    }
}

} // namespace Render
