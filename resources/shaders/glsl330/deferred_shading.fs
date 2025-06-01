#version 330 core
out vec4 finalColor;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

// === Новый блок uniform для солнца и неба ===
uniform vec3 sunDir;         // направление солнца (НОРМАЛИЗОВАННОЕ, из пространства мира, например: vec3(-0.5, -1.0, -0.5))
uniform vec3 sunColor;       // цвет света солнца (например, vec3(1.0, 0.95, 0.8))
uniform vec3 ambientColor;   // цвет/интенсивность неба (например, vec3(0.15, 0.17, 0.23))
uniform vec3 viewPosition;   // позиция камеры, для specular

void main() {
    vec3 fragPosition = texture(gPosition, texCoord).rgb;
    vec3 normal = normalize(texture(gNormal, texCoord).rgb);
    vec3 albedo = texture(gAlbedoSpec, texCoord).rgb;
    float specularStrength = texture(gAlbedoSpec, texCoord).a;

    // --- Ambient
    vec3 ambient = albedo * ambientColor;

    // --- Sun (Directional light) diffuse
    float diff = max(dot(normal, -sunDir), 0.0);  // NB: sunDir указывает "ОТ солнца"
    vec3 diffuse = diff * sunColor * albedo;

    // --- Specular (Blinn-Phong)
    vec3 viewDir = normalize(viewPosition - fragPosition);
    vec3 halfwayDir = normalize(-sunDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0); // 32 — shininess, можно сделать uniform
    vec3 specular = specularStrength * spec * sunColor;

    // --- Итог
    vec3 color = ambient + diffuse + specular;

    finalColor = vec4(color, 1.0);
}
