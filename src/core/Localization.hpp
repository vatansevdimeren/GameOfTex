#pragma once

#include <string>
#include <unordered_map>

namespace Core {

enum class Language {
    TURKISH,
    ENGLISH
};

class LocalizationManager {
public:
    static LocalizationManager& Get();

    void SetLanguage(Language lang);
    [[nodiscard]] Language GetLanguage() const;
    void ToggleLanguage();

    [[nodiscard]] const std::string& GetLanguageName() const;
    [[nodiscard]] const char* Get(const std::string& key) const;
    static const char* Tr(const std::string& key);

private:
    LocalizationManager();
    void InitTranslations();

    Language m_currentLanguage;
    std::unordered_map<std::string, std::string> m_translationsTR;
    std::unordered_map<std::string, std::string> m_translationsEN;
};

} // namespace Core
