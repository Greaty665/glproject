#version 330 core

in vec3 vertexColor;
uniform int drawMode;
out vec4 fragColor;

void main(void) {
    if (drawMode == 2) { // Csak a kontrollpontoknál maszkolunk kört
        float dist = length(gl_PointCoord - vec2(0.5));
        
        // Simább szél: fade-out a szélén 0.48–0.5 között
        float alpha = smoothstep(0.5, 0.48, dist);
        if (alpha < 0.01) discard;

        fragColor = vec4(vertexColor, alpha);
    } else {
        fragColor = vec4(vertexColor, 1.0);
    }
}
