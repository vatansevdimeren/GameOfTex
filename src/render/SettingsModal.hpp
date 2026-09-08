#pragma once

#include "raylib.h"
#include "UIButton.hpp"
#include "../core/EconomyManager.hpp"
#include "../core/Localization.hpp"
#include <string>

namespace Render {

enum class SettingsAction {
    NONE,
    SAVE_GAME,
    RETURN_TO_MAIN_MENU
};

/**
 * @class SettingsModal
 * @brief Modal dialog for adjusting UI text scaling, language, currency, save game, and fullscreen.
 * 
 * SRP: This class is solely responsible for rendering the settings popup and handling user preferences.
 */
class SettingsModal {
public:
    SettingsModal();

    void Open();
    void Close();
    [[nodiscard]] bool IsOpen() const;

    /**
     * @brief Updates hover/clicks for scale, language, currency, save, and close buttons.
     */
    SettingsAction Update(Core::EconomyManager& economy);

    /**
     * @brief Draws the modal dialog over the current screen.
     */
    void Draw(const Core::EconomyManager& economy) const;

private:
    bool m_isOpen;
    UIButton m_btnClose;
    UIButton m_btnScale100;
    UIButton m_btnScale125;
    UIButton m_btnScale150;
    UIButton m_btnScale175;
    UIButton m_btnScale200;
    UIButton m_btnToggleFullscreen;
    UIButton m_btnToggleLanguage;
    UIButton m_btnToggleCurrency;
    UIButton m_btnSaveGame;
    UIButton m_btnMainMenu;
};

} // namespace Render
