#version 330 core

out vec4 FragColor;

uniform vec2 resolution;  
uniform vec2 circleCenter;  
uniform float circleRadius;  

void main() {
    vec2 uv = gl_FragCoord.xy / resolution;  
    vec2 pixelPos = uv * 2.0 - 1.0;  

    float dist = length(pixelPos - circleCenter);

    float mixFactor = smoothstep(circleRadius * 0.8, circleRadius, dist);
    vec3 color = mix(vec3(0.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0), mixFactor);

    if (dist <= circleRadius) {
        FragColor = vec4(color, 1.0);
    } else {
        discard;
    }
}
