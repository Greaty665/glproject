   #version 330 core
    out vec4 FragColor;
    uniform vec2 uCircleCenter;
    uniform float uCircleRadius;
    uniform bool uIntersecting;

    void main() {
        float dist = distance(gl_FragCoord.xy, uCircleCenter);
        float t = smoothstep(0.0, uCircleRadius, dist);
        
        vec3 innerColor = uIntersecting ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0); // Zöld vagy piros belül
        vec3 outerColor = uIntersecting ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 1.0, 0.0); // Piros vagy zöld kívül

        vec3 color = mix(innerColor, outerColor, t);
        if (dist > uCircleRadius) discard;
        FragColor = vec4(color, 1.0);
    }