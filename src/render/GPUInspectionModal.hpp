#pragma once

#include "raylib.h"
#include "../core/GPU.hpp"
#include "UIButton.hpp"
#include "TextureManager.hpp"
#include <string>

namespace Render {

/**
 * @class GPUInspectionModal
 * @brief Modal for 360-degree interactive card inspection, precision overclock tuning,
 * and hardware burn/destruction diagnostics.
 * 
 * SRP: This class is solely responsible for rendering the GPU inspection overlay,
 * 360° rotation visualization, and handling individual card clock/fan adjustments.
 */
class GPUInspectionModal {
public:
    GPUInspectionModal();

    void Open(Core::GPU* gpu, double tempCelsius, size_t slotIndex);
    void Close();
    [[nodiscard]] bool IsOpen() const;
    [[nodiscard]] Core::GPU* GetTargetGPU() const;

    /**
     * @brief Updates rotation dragging, slider buttons, and repair actions.
     * @param economyFiat Player's available cash to pay for repairs.
     * @param outRepairCost Outputs repair cost deducted if repaired this frame.
     */
    void Update(double economyFiat, double& outRepairCost);

    /**
     * @brief Renders the 360-degree card, status gauges, and OC controls.
     * @param animTime Accumulated time for fan rotation.
     * @param textureManager Texture manager for high-res sprites.
     */
    void Draw(double animTime, const TextureManager* textureManager = nullptr) const;

private:
    void Draw3DCardPreview(float centerX, float centerY, float height, double animTime,
                           const TextureManager* textureManager) const;

    bool m_isOpen;
    Core::GPU* m_targetGPU;
    double m_currentTemp;
    size_t m_slotIndex;

    float m_rotationAngle; // 0 to 360 degrees
    bool m_isDraggingRotation;
    float m_lastMouseX;

    // Overclock tuning buttons
    UIButton m_btnClockDown;
    UIButton m_btnClockUp;
    UIButton m_btnPowerDown;
    UIButton m_btnPowerUp;
    UIButton m_btnFanDown;
    UIButton m_btnFanUp;

    // 360 Rotate buttons
    UIButton m_btnRotateLeft;
    UIButton m_btnRotateRight;

    // Actions
    UIButton m_btnRepair;
    UIButton m_btnClose;
};

} // namespace Render
