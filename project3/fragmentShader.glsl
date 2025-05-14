#version 430

in vec3 Normal;
in vec3 FragPos;
in vec2 texCoord;

uniform vec3 lightPos;
uniform bool lightOn;
uniform bool isLight;

uniform sampler2D sunSampler;

out vec4 color;

void main() {
    if (isLight) {
        // Textured yellow sun
        vec4 sunColor = texture(sunSampler, texCoord);
        color = sunColor;
    } else {
        if (lightOn) {
            // Ambient
            float ambientStrength = 0.15;
            vec3 ambient = ambientStrength * vec3(1.0, 1.0, 0.0); // Yellow

            // Diffuse
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * vec3(1.0, 0.8745, 0.1333); // Light yellow

            vec3 objectColor = vec3(1.0, 1.0, 1.0); // White cube base
            vec3 result = (ambient + diffuse) * objectColor;

            color = vec4(result, 1.0);
        } else {
            // Light is OFF
            color = vec4(1.0, 1.0, 1.0, 1.0);
        }
    }
}
