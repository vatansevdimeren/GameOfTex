cmake_minimum_required(VERSION 3.20)
project(GameOfTex VERSION 1.4.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Raylib kütüphanesini otomatik çek ve bağla
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
    GIT_SHALLOW TRUE
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

# Oyun yürütülebilir dosyası (Clean Code & SRP Modülleri)
add_executable(GameOfTex
    src/main.cpp
    src/core/GPU.hpp
    src/core/GPU.cpp
    src/core/MiningRig.hpp
    src/core/MiningRig.cpp
    src/core/Warehouse.hpp
    src/core/Warehouse.cpp
    src/core/CoolingManager.hpp
    src/core/CoolingManager.cpp
    src/core/ThermalModel.hpp
    src/core/ThermalModel.cpp
    src/core/PowerGrid.hpp
    src/core/PowerGrid.cpp
    src/core/EconomyManager.hpp
    src/core/EconomyManager.cpp
    src/core/UserProfile.hpp
    src/core/UserProfile.cpp
    src/core/Localization.hpp
    src/core/Localization.cpp
    src/core/MultiplierManager.hpp
    src/core/MultiplierManager.cpp
    src/render/ShaderManager.hpp
    src/render/ShaderManager.cpp
    src/render/TextureManager.hpp
    src/render/TextureManager.cpp
    src/render/RigRenderer.hpp
    src/render/RigRenderer.cpp
    src/render/UIButton.hpp
    src/render/UIButton.cpp
    src/render/UIFrame.hpp
    src/render/UIFrame.cpp
    src/render/LoginScreen.hpp
    src/render/LoginScreen.cpp
    src/render/SettingsModal.hpp
    src/render/SettingsModal.cpp
    src/render/GPUInspectionModal.hpp
    src/render/GPUInspectionModal.cpp
    src/core/MarketCatalog.hpp
    src/core/MarketCatalog.cpp
    src/render/MarketModal.hpp
    src/render/MarketModal.cpp
    src/core/TaskManager.hpp
    src/core/TaskManager.cpp
    src/render/TaskModal.hpp
    src/render/TaskModal.cpp
    src/core/FacilityManager.hpp
    src/core/FacilityManager.cpp
    src/render/WorldMapModal.hpp
    src/render/WorldMapModal.cpp
    src/core/SaveManager.hpp
    src/core/SaveManager.cpp
    src/render/MainMenuScreen.hpp
    src/render/MainMenuScreen.cpp
    src/render/CryptoExchangeModal.hpp
    src/render/CryptoExchangeModal.cpp
    src/core/NewsManager.hpp
    src/core/NewsManager.cpp
    src/render/NewsModal.hpp
    src/render/NewsModal.cpp
    src/core/ResearchManager.hpp
    src/core/ResearchManager.cpp
    src/render/ResearchModal.hpp
    src/render/ResearchModal.cpp
    src/render/OfflineEarningsModal.hpp
    src/render/OfflineEarningsModal.cpp
)

target_include_directories(GameOfTex PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(GameOfTex PRIVATE
    raylib
)

if(WIN32)
    set_target_properties(GameOfTex PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    if(MSVC)
        set_target_properties(GameOfTex PROPERTIES
            LINK_FLAGS "/ENTRY:mainCRTStartup"
        )
    endif()
endif()

# Shader ve Asset klasörlerini derleme çıktısının yanına kopyala
add_custom_command(TARGET GameOfTex POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders
        $<TARGET_FILE_DIR:GameOfTex>/src/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:GameOfTex>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:GameOfTex>
        ${CMAKE_CURRENT_SOURCE_DIR}/GameOfTex.exe
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:GameOfTex>
        ${CMAKE_CURRENT_SOURCE_DIR}/dist/GameOfTex-Windows/GameOfTex.exe
)
