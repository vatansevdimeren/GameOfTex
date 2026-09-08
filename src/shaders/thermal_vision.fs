#version 330

// Input vertex attributes from Raylib
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

// Uniforms
uniform sampler2D texture0;
uniform float u_temp;          // Normalized temperature [0.0 = 20C, 1.0 = 90C]
uniform float u_thermalMode;   // 1.0 = thermal vision enabled, 0.0 = normal view

// Heatmap color ramp: Blue -> Cyan -> Green -> Yellow -> Orange -> Red -> White
vec3 Heatmap(float t) {
    t = clamp(t, 0.0, 1.0);
    vec3 c;
    if (t < 0.2) {
        // Deep blue to cyan
        c = mix(vec3(0.0, 0.05, 0.5), vec3(0.0, 0.8, 0.9), t / 0.2);
    } else if (t < 0.4) {
        // Cyan to bright green
        c = mix(vec3(0.0, 0.8, 0.9), vec3(0.1, 0.95, 0.2), (t - 0.2) / 0.2);
    } else if (t < 0.6) {
        // Green to bright yellow
        c = mix(vec3(0.1, 0.95, 0.2), vec3(1.0, 0.95, 0.0), (t - 0.4) / 0.2);
    } else if (t < 0.8) {
        // Yellow to intense red/orange
        c = mix(vec3(1.0, 0.95, 0.0), vec3(0.95, 0.15, 0.0), (t - 0.6) / 0.2);
    } else {
        // Red to blazing incandescent white
        c = mix(vec3(0.95, 0.15, 0.0), vec3(1.0, 1.0, 1.0), (t - 0.8) / 0.2);
    }
    return c;
}

void main() {
    vec4 texel = texture(texture0, fragTexCoord) * fragColor;

    if (u_thermalMode > 0.5) {
        // Luminance of base texture
        float lum = dot(texel.rgb, vec3(0.299, 0.587, 0.114));
        // Blend temperature factor with geometry brightness
        float heat = clamp(u_temp * 0.75 + lum * 0.25, 0.0, 1.0);
        vec3 thermalRamp = Heatmap(heat);
        finalColor = vec4(thermalRamp, texel.a);
    } else {
        finalColor = texel;
    }
}
