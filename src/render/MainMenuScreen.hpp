#pragma once

#include "raylib.h"
#include "UIButton.hpp"
#include "../core/SaveManager.hpp"
#include <string>
#include <vector>

namespace Render {

enum class MainMenuAction {
    NONE,
    CONTINUE_GAME,
    START_NEW_GAME,
    OPEN_SETTINGS,
    QUIT_GAME
};

struct MenuParticle {
    float x;
    float y;
    float speedY;
    float speedX;
    float size;
    float alpha;
    Color color;
};

/**
 * @class MainMenuScreen
 * @brief Renders the cyberpunk animated Title Screen and manages main menu navigation.
 * 
 * SRP: Solely responsible for rendering title visuals, animations,
 * and reporting selected navigation actions to the main game coordinator.
 */
class MainMenuScreen {
public:
    MainMenuScreen();

    /**
     * @brief Checks disk for existing save and updates button labels and states.
     */
    void RefreshSaveState();

    /**
     * @brief Handles mouse input and button updates for the main menu.
     * @param dt Frame delta time.
     * @return Selected action.
     */
    MainMenuAction Update(float dt);

    /**
     * @brief Renders the background matrix, animated particles, logo, and menu buttons.
     */
    void Draw() const;

private:
    void InitParticles();
    void UpdateParticles(float dt);

    bool m_hasSave{false};
    Core::SaveMetadata m_saveMeta;
    bool m_showConfirmNewGame{false};

    UIButton m_btnContinue;
    UIButton m_btnNewGame;
    UIButton m_btnSettings;
    UIButton m_btnQuit;

    UIButton m_btnConfirmReset;
    UIButton m_btnCancelReset;

    std::vector<MenuParticle> m_particles;
    float m_animTime{0.0f};
};

} // namespace Render
