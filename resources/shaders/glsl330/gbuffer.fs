#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

void main() {
    // 1. Записываем мировую позицию
    gPosition = fragPosition;

    // 2. Записываем нормаль (нормализуем, вдруг кто-то испортил)
    gNormal = normalize(fragNormal);

    // 3. Цвет из атласа (альбедо)
    gAlbedoSpec.rgb = texture(diffuseTexture, fragTexCoord).rgb;

    // 4. Спекуляр — intensity (если надо, иначе можно захардкодить, например: gAlbedoSpec.a = 0.3;)
    gAlbedoSpec.a = texture(specularTexture, fragTexCoord).r;
}
