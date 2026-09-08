#pragma once

#include <string>

namespace Core {

/**
 * @class UserProfile
 * @brief Manages player identity, mining company name, avatar, and account bonus status.
 * 
 * SRP: This class is solely responsible for storing and persisting player profile data.
 */
class UserProfile {
public:
    UserProfile();
    UserProfile(std::string companyName, int avatarIndex);

    [[nodiscard]] const std::string& GetCompanyName() const;
    void SetCompanyName(const std::string& name);

    [[nodiscard]] int GetAvatarIndex() const;
    void SetAvatarIndex(int index);

    [[nodiscard]] bool HasClaimedBonus() const;
    void SetClaimedBonus(bool claimed);

    /**
     * @brief Saves profile to a local text file.
     * @param filepath Target file path.
     * @return True if saved successfully.
     */
    bool SaveToFile(const std::string& filepath) const;

    /**
     * @brief Loads profile from a local text file.
     * @param filepath Source file path.
     * @return True if loaded successfully.
     */
    bool LoadFromFile(const std::string& filepath);

private:
    std::string m_companyName;
    int m_avatarIndex;
    bool m_claimedBonus;
};

} // namespace Core
