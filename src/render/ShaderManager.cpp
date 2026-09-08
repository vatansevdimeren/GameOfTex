#include "ShaderManager.hpp"
#include <iostream>

namespace Render {

ShaderManager::ShaderManager()
    : m_shader{}
    , m_locTemp(-1)
    , m_locMode(-1)
    , m_isLoaded(false)
    , m_thermalActive(false)
{
}

ShaderManager::~ShaderManager() {
    if (m_isLoaded) {
        UnloadShader(m_shader);
    }
}

bool ShaderManager::LoadThermalShader(const std::string& fragmentShaderPath) {
    if (m_isLoaded) {
        UnloadShader(m_shader);
        m_isLoaded = false;
    }

    // Farklı çalışma dizinleri için dosya varlık kontrolü
    std::string validPath = fragmentShaderPath;
    if (!FileExists(validPath.c_str())) {
        if (FileExists("src/shaders/thermal_vision.fs")) {
            validPath = "src/shaders/thermal_vision.fs";
        } else if (FileExists("../src/shaders/thermal_vision.fs")) {
            validPath = "../src/shaders/thermal_vision.fs";
        } else if (FileExists("../../src/shaders/thermal_vision.fs")) {
            validPath = "../../src/shaders/thermal_vision.fs";
        }
    }

    // 0 vertex shader means Raylib uses its default vertex shader
    m_shader = LoadShader(nullptr, validPath.c_str());

    if (m_shader.id == 0) {
        std::cerr << "[ShaderManager] Failed to load shader: " << validPath << std::endl;
        return false;
    }

    m_locTemp = GetShaderLocation(m_shader, "u_temp");
    m_locMode = GetShaderLocation(m_shader, "u_thermalMode");

    float defaultMode = 0.0f;
    float defaultTemp = 0.0f;
    SetShaderValue(m_shader, m_locMode, &defaultMode, SHADER_UNIFORM_FLOAT);
    SetShaderValue(m_shader, m_locTemp, &defaultTemp, SHADER_UNIFORM_FLOAT);

    m_isLoaded = true;
    std::cout << "[ShaderManager] Successfully loaded shader: " << fragmentShaderPath << std::endl;
    return true;
}

void ShaderManager::SetThermalState(bool enabled, float normalizedTemp) {
    if (!m_isLoaded) return;

    m_thermalActive = enabled;
    float modeVal = enabled ? 1.0f : 0.0f;
    SetShaderValue(m_shader, m_locMode, &modeVal, SHADER_UNIFORM_FLOAT);
    SetShaderValue(m_shader, m_locTemp, &normalizedTemp, SHADER_UNIFORM_FLOAT);
}

void ShaderManager::BeginShader() {
    if (m_isLoaded) {
        BeginShaderMode(m_shader);
    }
}

void ShaderManager::EndShader() {
    if (m_isLoaded) {
        EndShaderMode();
    }
}

bool ShaderManager::IsThermalActive() const {
    return m_thermalActive;
}

void ShaderManager::ToggleThermal() {
    m_thermalActive = !m_thermalActive;
}

} // namespace Render
