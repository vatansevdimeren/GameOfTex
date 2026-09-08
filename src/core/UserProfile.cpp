#include "UserProfile.hpp"
#include <fstream>
#include <iostream>

namespace Core {

UserProfile::UserProfile()
    : m_companyName("Crypto Core Co.")
    , m_avatarIndex(0)
    , m_claimedBonus(false)
{
}

UserProfile::UserProfile(std::string companyName, int avatarIndex)
    : m_companyName(std::move(companyName))
    , m_avatarIndex(avatarIndex)
    , m_claimedBonus(false)
{
}

const std::string& UserProfile::GetCompanyName() const {
    return m_companyName;
}

void UserProfile::SetCompanyName(const std::string& name) {
    if (!name.empty()) {
        m_companyName = name;
    }
}

int UserProfile::GetAvatarIndex() const {
    return m_avatarIndex;
}

void UserProfile::SetAvatarIndex(int index) {
    m_avatarIndex = index;
}

bool UserProfile::HasClaimedBonus() const {
    return m_claimedBonus;
}

void UserProfile::SetClaimedBonus(bool claimed) {
    m_claimedBonus = claimed;
}

bool UserProfile::SaveToFile(const std::string& filepath) const {
    std::ofstream out(filepath);
    if (!out.is_open()) {
        return false;
    }
    out << m_companyName << "\n";
    out << m_avatarIndex << "\n";
    out << (m_claimedBonus ? 1 : 0) << "\n";
    return true;
}

bool UserProfile::LoadFromFile(const std::string& filepath) {
    std::ifstream in(filepath);
    if (!in.is_open()) {
        return false;
    }
    std::string name;
    int avatar = 0;
    int bonus = 0;

    if (std::getline(in, name) && (in >> avatar) && (in >> bonus)) {
        m_companyName = name;
        m_avatarIndex = avatar;
        m_claimedBonus = (bonus == 1);
        return true;
    }
    return false;
}

} // namespace Core
