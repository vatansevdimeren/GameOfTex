#pragma once

#include "raylib.h"
#include "../core/MiningRig.hpp"
#include "../core/Warehouse.hpp"
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

    /**
     * @brief Detects if mouse clicked on an installed GPU card slot.
     * @return Slot index (0 to N-1) or -1 if none clicked.
     */
    [[nodiscard]] int GetClickedGPUIndex(int posX, int posY, size_t gpuCount, Vector2 mousePos) const;

    /**
     * @brief Draws an overview grid of all rigs in the facility.
     * @param warehouse Reference to warehouse holding all rigs.
     * @param thermalModel Reference to thermal model.
     * @param bounds Bounds of the overview panel area.
     * @param animTime Animation timer for fans/lights.
     * @param mousePos Mouse cursor position.
     * @param outSelectedRigIndex Returns index of rig clicked to inspect in detail (-1 if none).
     * @param outToggledRigIndex Returns index of rig whose power button was toggled (-1 if none).
     */
    void DrawWarehouseOverviewGrid(const Core::Warehouse& warehouse, const Core::ThermalModel& thermalModel,
                                   const Rectangle& bounds, double animTime, Vector2 mousePos,
                                   int& outSelectedRigIndex, int& outToggledRigIndex) const;

private:
    /**
     * @brief Draws a single GPU card at given coordinates.
     */
    void DrawSingleGPU(const Core::GPU* gpu, double tempCelsius, int x, int y, double animTime,
                       bool rigPoweredOn, const TextureManager* textureManager = nullptr) const;

    /**
     * @brief Draws animated smoke and ember sparks rising from a burnt card.
     */
    void DrawSmokeAndSparks(int centerX, int centerY, double animTime) const;

    /**
     * @brief Draws braided PCIe power harness cables to the GPU.
     */
    void DrawBraidedPCIeCable(int startX, int startY, int endX, int endY) const;

    /**
     * @brief Draws an animated cooling fan with rotating blades.
     */
    void DrawSpinningFan(int centerX, int centerY, float radius, float angleDegrees, Color bladeColor) const;
};

} // namespace Render
