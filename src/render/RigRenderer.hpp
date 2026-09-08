#pragma once

#include "raylib.h"
#include "../core/MiningRig.hpp"
#include "../core/ThermalModel.hpp"
#include "TextureManager.hpp"

namespace Render {

/**
 * @class RigRenderer
 * @brief Renders the visual representation of mining rigs, GPUs, and spinning fans.
 * 
 * SRP: This class is solely responsible for 2D drawing of mining hardware
 * and visual effects like fan rotation and LED states.
 */
class RigRenderer {
public:
    RigRenderer();

    /**
     * @brief Draws the entire mining rig with all its mounted GPUs.
     * @param rig Reference to the rig model.
     * @param thermalModel Reference to thermal model to get individual GPU temps.
     * @param textureManager Optional reference to high-res texture manager.
     * @param posX Screen X coordinate.
     * @param posY Screen Y coordinate.
     * @param animTime Accumulated time for fan rotations.
     */
    void DrawRig(const Core::MiningRig& rig, const Core::ThermalModel& thermalModel,
                 int posX, int posY, double animTime, const TextureManager* textureManager = nullptr) const;

private:
    /**
     * @brief Draws a single GPU card at given coordinates.
     */
    void DrawSingleGPU(const Core::GPU* gpu, double tempCelsius, int x, int y, double animTime,
                       const TextureManager* textureManager = nullptr) const;

    /**
     * @brief Draws an animated cooling fan with rotating blades.
     */
    void DrawSpinningFan(int centerX, int centerY, float radius, float angleDegrees, Color bladeColor) const;
};

} // namespace Render
