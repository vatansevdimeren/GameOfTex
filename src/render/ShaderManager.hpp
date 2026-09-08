#pragma once

#include "raylib.h"
#include <string>

namespace Render {

/**
 * @class ShaderManager
 * @brief Manages loading, uniform updating, and activation of GLSL shaders.
 * 
 * SRP: This class is solely responsible for OpenGL shader lifecycle
 * and uniform parameter binding for post-processing and visual effects.
 */
class ShaderManager {
public:
    ShaderManager();
    ~ShaderManager();

    // Prevent copying
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    /**
     * @brief Loads the thermal vision fragment shader from disk.
     * @param fragmentShaderPath Path to the .fs file.
     * @return True if loaded successfully.
     */
    bool LoadThermalShader(const std::string& fragmentShaderPath);

    /**
     * @brief Activates or deactivates thermal vision rendering mode.
     * @param enabled True to show thermal heatmap, false for normal rendering.
     * @param normalizedTemp Temperature ratio from 0.0 (20C) to 1.0 (90C).
     */
    void SetThermalState(bool enabled, float normalizedTemp);

    /**
     * @brief Begins shader drawing scope if shader is loaded.
     */
    void BeginShader();

    /**
     * @brief Ends shader drawing scope.
     */
    void EndShader();

    [[nodiscard]] bool IsThermalActive() const;
    void ToggleThermal();

private:
    Shader m_shader;
    int m_locTemp;
    int m_locMode;
    bool m_isLoaded;
    bool m_thermalActive;
};

} // namespace Render
