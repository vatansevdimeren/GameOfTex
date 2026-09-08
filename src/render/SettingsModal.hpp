#pragma once

#include "raylib.h"
#include "UIButton.hpp"
#include <string>

namespace Render {

/**
 * @class SettingsModal
 * @brief Modal dialog for adjusting UI text scaling, graphics settings, and fullscreen.
 * 
 * SRP: This class is solely responsible for rendering the settings popup and handling scale selections.
 */
class SettingsModal {
public:
    SettingsModal();

    void Open();
    void Close();
    [[nodiscard]] bool IsOpen() const;

    /**
     * @brief Updates hover/clicks for UI scale buttons and close button.
     */
    void Update();

    /**
     * @brief Draws the modal dialog over the current screen.
     */
    void Draw() const;

private:
    bool m_isOpen;
    UIButton m_btnClose;
    UIButton m_btnScale100;
    UIButton m_btnScale125;
    UIButton m_btnScale150;
    UIButton m_btnScale175;
    UIButton m_btnScale200;
    UIButton m_btnToggleFullscreen;
};

} // namespace Render
