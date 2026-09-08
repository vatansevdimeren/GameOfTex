#pragma once

#include "raylib.h"
#include <string>

namespace Render {

/**
 * @class TextureManager
 * @brief Manages external high-resolution textures (2K/4K PNG/JPG) with procedural fallbacks.
 * 
 * SRP: This class is solely responsible for loading, filtering, caching,
 * and rendering sprite textures for GPUs, fans, and backgrounds.
 */
class TextureManager {
public:
    TextureManager();
    ~TextureManager();

    // Prevent copying
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    /**
     * @brief Scans assets/textures/ and loads available high-res images.
     */
    void LoadTextures(const std::string& assetFolder = "assets/textures");

    [[nodiscard]] bool HasGPUTexture() const;
    [[nodiscard]] bool HasFanTexture() const;
    [[nodiscard]] bool HasWarehouseBackground() const;

    [[nodiscard]] Texture2D GetGPUTexture() const;
    [[nodiscard]] Texture2D GetFanTexture() const;
    [[nodiscard]] Texture2D GetWarehouseBackground() const;

    /**
     * @brief Draws GPU texture scaled to destRect, with optional tint.
     */
    void DrawGPUTexture(Rectangle destRect, Color tint = WHITE) const;

    /**
     * @brief Draws rotating fan texture at centerX, centerY with given radius and angle.
     */
    void DrawFanTexture(float centerX, float centerY, float radius, float angleDegrees, Color tint = WHITE) const;

    /**
     * @brief Draws warehouse background scaled to fill viewport.
     */
    void DrawBackground(Rectangle destRect) const;

    void Unload();

private:
    Texture2D m_texGPU;
    Texture2D m_texFan;
    Texture2D m_texWarehouse;

    bool m_hasGPU;
    bool m_hasFan;
    bool m_hasWarehouse;
};

} // namespace Render
