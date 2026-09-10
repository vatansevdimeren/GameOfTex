#pragma once

#include <string>
#include <vector>

namespace Core {

class UserProfile;
class EconomyManager;
class FacilityManager;
class TaskManager;
class MarketCatalog;
class ResearchManager;

struct SaveMetadata {
    std::string companyName{"Eren Mining Co."};
    int avatarIndex{0};
    double fiatBalance{0.0};
    double cryptoBalance{0.0};
    std::string activeFacilityName{"Teksas"};
    int totalRigs{1};
    int totalGPUs{0};
    std::string timestamp{""};
    bool isValid{false};
};

struct OfflineMiningReport {
    bool hasReport{false};
    double elapsedSeconds{0.0};
    int totalRigsRunning{0};
    std::string gpuCoinSymbol{"TEX"};
    double gpuCoinsMined{0.0};
    double gpuCoinsValueUSD{0.0};
    std::string cpuCoinSymbol{"XMR"};
    double cpuCoinsMined{0.0};
    double cpuCoinsValueUSD{0.0};
    double electricityCostUSD{0.0};
    double netEarningsUSD{0.0};
};

/**
 * @class SaveManager
 * @brief Manages serializing and deserializing complete game state to/from disk.
 * 
 * SRP: This class is solely responsible for saving, loading, verifying,
 * and deleting game save files.
 */
class SaveManager {
public:
    static constexpr const char* DEFAULT_SAVE_PATH = "savegame.dat";

    /**
     * @brief Checks if a valid save file exists on disk.
     */
    [[nodiscard]] static bool HasSaveFile(const std::string& filepath = DEFAULT_SAVE_PATH);

    /**
     * @brief Quick-reads header metadata for displaying on the Main Menu.
     */
    [[nodiscard]] static bool ReadSaveMetadata(SaveMetadata& outMetadata, const std::string& filepath = DEFAULT_SAVE_PATH);

    /**
     * @brief Saves the full game state to disk.
     * @return True if save succeeded.
     */
    static bool SaveGame(const std::string& filepath,
                         const UserProfile& profile,
                         const EconomyManager& economy,
                         const FacilityManager& facilityManager,
                         const TaskManager& taskManager,
                         const MarketCatalog& marketCatalog,
                         const ResearchManager* researchManager = nullptr);

    /**
     * @brief Loads the full game state from disk into live objects.
     * @param outReport Optional pointer to receive offline idle mining simulation results.
     * @return True if loaded successfully.
     */
    static bool LoadGame(const std::string& filepath,
                         UserProfile& profile,
                         EconomyManager& economy,
                         FacilityManager& facilityManager,
                         TaskManager& taskManager,
                         MarketCatalog& marketCatalog,
                         ResearchManager* researchManager = nullptr,
                         OfflineMiningReport* outReport = nullptr);

    /**
     * @brief Deletes the save file for starting completely fresh.
     */
    static bool DeleteSaveFile(const std::string& filepath = DEFAULT_SAVE_PATH);
};

} // namespace Core
