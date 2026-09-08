#include "TextureManager.hpp"
#include <iostream>

namespace Render {

TextureManager::TextureManager()
    : m_texGPU{}
    , m_texFan{}
    , m_texWarehouse{}
    , m_hasGPU(false)
    , m_hasFan(false)
    , m_hasWarehouse(false)
{
}

TextureManager::~TextureManager() {
    Unload();
}

void TextureManager::LoadTextures(const std::string& assetFolder) {
    Unload();

    auto tryLoad = [](const std::string& path, Texture2D& outTex, bool& outFlag, const char* name) {
        if (FileExists(path.c_str())) {
            outTex = LoadTexture(path.c_str());
            if (outTex.id > 0) {
                SetTextureFilter(outTex, TEXTURE_FILTER_BILINEAR);
                outFlag = true;
                std::cout << "[TextureManager] Loaded Ultra-Res " << name << ": " << path
                          << " (" << outTex.width << "x" << outTex.height << ")" << std::endl;
                return;
            }
        }
        outFlag = false;
    };

    tryLoad(assetFolder + "/gpu_card.png", m_texGPU, m_hasGPU, "GPU");
    tryLoad(assetFolder + "/fan_blade.png", m_texFan, m_hasFan, "Fan");
    tryLoad(assetFolder + "/warehouse_bg.png", m_texWarehouse, m_hasWarehouse, "Warehouse Background");
}

bool TextureManager::HasGPUTexture() const {
    return m_hasGPU;
}

bool TextureManager::HasFanTexture() const {
    return m_hasFan;
}

bool TextureManager::HasWarehouseBackground() const {
    return m_hasWarehouse;
}

Texture2D TextureManager::GetGPUTexture() const {
    return m_texGPU;
}

Texture2D TextureManager::GetFanTexture() const {
    return m_texFan;
}

Texture2D TextureManager::GetWarehouseBackground() const {
    return m_texWarehouse;
}

void TextureManager::DrawGPUTexture(Rectangle destRect, Color tint) const {
    if (!m_hasGPU) return;

    Rectangle srcRect{0.0f, 0.0f, static_cast<float>(m_texGPU.width), static_cast<float>(m_texGPU.height)};
    DrawTexturePro(m_texGPU, srcRect, destRect, Vector2{0.0f, 0.0f}, 0.0f, tint);
}

void TextureManager::DrawFanTexture(float centerX, float centerY, float radius, float angleDegrees, Color tint) const {
    if (!m_hasFan) return;

    Rectangle srcRect{0.0f, 0.0f, static_cast<float>(m_texFan.width), static_cast<float>(m_texFan.height)};
    float diameter = radius * 2.0f;
    Rectangle destRect{centerX, centerY, diameter, diameter};
    Vector2 origin{radius, radius};

    DrawTexturePro(m_texFan, srcRect, destRect, origin, angleDegrees, tint);
}

void TextureManager::DrawBackground(Rectangle destRect) const {
    if (!m_hasWarehouse) return;

    Rectangle srcRect{0.0f, 0.0f, static_cast<float>(m_texWarehouse.width), static_cast<float>(m_texWarehouse.height)};
    DrawTexturePro(m_texWarehouse, srcRect, destRect, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
}

void TextureManager::Unload() {
    if (m_hasGPU) {
        UnloadTexture(m_texGPU);
        m_hasGPU = false;
    }
    if (m_hasFan) {
        UnloadTexture(m_texFan);
        m_hasFan = false;
    }
    if (m_hasWarehouse) {
        UnloadTexture(m_texWarehouse);
        m_hasWarehouse = false;
    }
}

} // namespace Render
