#pragma once

#include "raylib.h"
#include <string>

namespace Render {

/**
 * @class UIFrame
 * @brief Utility class for drawing scalable, high-resolution cyberpunk UI containers, text, and badges.
 * 
 * SRP: This class is solely responsible for rendering UI frames, theme fonts, and gauges.
 */
class UIFrame {
public:
    /**
     * @brief Initializes the crisp vector font theme (Segoe UI / Arial).
     */
    static void InitTheme(Font fontRegular, Font fontBold);

    /**
     * @brief Gets current fonts.
     */
    static Font GetFontRegular();
    static Font GetFontBold();

    /**
     * @brief Draws crisp, scaled anti-aliased text using loaded theme fonts.
     */
    static void DrawTextCustom(const std::string& text, float x, float y, float size, Color color, bool bold = false);

    /**
     * @brief Measures text width accurately using loaded theme fonts.
     */
    static float MeasureTextCustom(const std::string& text, float size, bool bold = false);

    /**
     * @brief Draws a stylized card container with header line and rounded corners.
     */
    static void DrawCard(Rectangle bounds, const std::string& title = "", Color accent = Color{0, 200, 255, 255});

    /**
     * @brief Draws a smoothly animated progress/gauge bar with label.
     */
    static void DrawProgressBar(Rectangle bounds, float progressRatio, Color barColor, const std::string& labelText = "");

    /**
     * @brief Draws a responsive stat badge pill with large, legible numbers.
     */
    static void DrawStatBadge(float x, float y, float width, float height,
                              const std::string& icon, const std::string& label,
                              const std::string& value, Color valueColor);

    /**
     * @brief Draws an interactive text input box with high-res text.
     */
    static void DrawTextInput(Rectangle bounds, const std::string& text, bool isActive, const std::string& placeholder);

    /**
     * @brief Sets the global UI font and layout scaling factor.
     */
    static void SetUIScale(float scale);
    static float GetUIScale();

private:
    static inline Font s_fontRegular{};
    static inline Font s_fontBold{};
    static inline bool s_hasCustomFonts = false;
    static inline float s_uiScale = 1.0f;
};

} // namespace Render
