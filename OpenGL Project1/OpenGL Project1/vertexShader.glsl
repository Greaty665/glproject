#version 330 core

layout(location = 0) in vec3 position;
uniform int drawMode; // 0: curve, 1: polygon, 2: control points

out vec3 vertexColor;

void main(void) {
    gl_Position = vec4(position, 1.0);

    if (drawMode == 0) {
        vertexColor = vec3(1.0, 0.949, 0.0); // görbe: sárga (255, 242, 0)
    } else if (drawMode == 1) {
        vertexColor = vec3(0.0, 0.647, 0.004); // kontrollpoligon: zöld (0, 165, 81)
    } else if (drawMode == 2) {
        vertexColor = vec3(0.9373, 0.102, 0.1765); // kontrollpontok: piros (239, 26, 45)
    } else {
        vertexColor = vec3(1.0, 1.0, 1.0); // hiba esetén: fehér
    }
}
