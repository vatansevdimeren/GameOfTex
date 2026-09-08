#pragma once

#include "raylib.h"
#include "../core/UserProfile.hpp"
#include "UIButton.hpp"
#include <string>

namespace Render {

/**
 * @class LoginScreen
 * @brief Manages the minimalist profile creation / login card screen.
 * 
 * SRP: This class is solely responsible for rendering the login interface
 * and gathering company name and avatar choices from the user.
 */
class LoginScreen {
public:
    LoginScreen();

    /**
     * @brief Handles mouse and keyboard input for profile creation.
     * @param profile Reference to the user profile to populate.
     * @return True if account creation is submitted and game should transition to gameplay.
     */
    bool Update(Core::UserProfile& profile);

    /**
     * @brief Renders the login card, avatars, text input, and start button.
     */
    void Draw() const;

private:
    std::string m_inputText;
    int m_selectedAvatar;
    bool m_inputActive;
    UIButton m_submitButton;
    Rectangle m_inputBoxRect;
    Rectangle m_avatar1Rect;
    Rectangle m_avatar2Rect;
    Rectangle m_avatar3Rect;
};

} // namespace Render
