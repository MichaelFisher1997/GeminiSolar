#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

// Procedural starfield function
float hash(vec3 p) {
    p = fract(p * 0.3183099 + 0.1);
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

void main() {
    vec3 ray = normalize(TexCoords);
    
    // Use high-frequency noise for stars
    vec3 p = ray * 200.0;
    float n = hash(floor(p));
    
    float star = 0.0;
    if (n > 0.998) {
        float size = fract(n * 100.0);
        star = pow(size, 20.0) * 2.0;
    }
    
    // Nebula-like background tint
    float nebula = 0.02 * (sin(ray.x * 2.0) + sin(ray.y * 3.0) + sin(ray.z * 1.5));
    vec3 nebulaColor = vec3(0.05, 0.05, 0.1) + vec3(0.02, 0.0, 0.05) * nebula;

    FragColor = vec4(nebulaColor + vec3(star), 1.0);
}
