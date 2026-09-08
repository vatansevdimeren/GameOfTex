#pragma once

#include "raylib.h"
#include <string>

namespace Render {

/**
 * @class UIButton
 * @brief Represents a modern interactive 2D button with hover, active, and disabled states.
 * 
 * SRP: This class is solely responsible for rendering a button and detecting user mouse clicks.
 */
class UIButton {
public:
    UIButton();
    UIButton(Rectangle bounds, std::string title, std::string subtitle = "",
             Color baseColor = Color{35, 40, 52, 255}, Color accentColor = Color{0, 200, 255, 255});

    void SetBounds(Rectangle bounds);
    [[nodiscard]] Rectangle GetBounds() const;

    void SetTitle(std::string title);
    void SetSubtitle(std::string subtitle);
    void SetDisabled(bool disabled);
    [[nodiscard]] bool IsDisabled() const;
    void SetAccentColor(Color color);
    void SetBaseColor(Color color);

    /**
     * @brief Updates hover/press state and checks if the button was clicked this frame.
     * @return True if left clicked inside bounds and not disabled.
     */
    bool UpdateAndCheckClick();

    /**
     * @brief Renders the button with high-tech glassmorphic styling, borders and text.
     */
    void Draw() const;

private:
    Rectangle m_bounds;
    std::string m_title;
    std::string m_subtitle;
    Color m_baseColor;
    Color m_accentColor;

    bool m_isHovered;
    bool m_isPressed;
    bool m_isDisabled;
};

} // namespace Render
