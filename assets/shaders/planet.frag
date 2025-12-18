#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 ViewDir;

uniform vec3 objectColor;
uniform int isSun;
uniform float time;

void main() {
    if (isSun == 1) {
        // Sun glow effect
        float pulse = 0.95 + 0.05 * sin(time * 2.0);
        FragColor = vec4(objectColor * pulse, 1.0);
        return;
    }

    vec3 norm = normalize(Normal);
    vec3 lightPos = vec3(0.0, 0.0, 0.0); 
    vec3 lightDir = normalize(lightPos - FragPos);
    
    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 0.95); // Slightly warm sunlight

    // Ambient
    vec3 ambient = 0.05 * objectColor;

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + ViewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = 0.3 * spec * vec3(1.0);

    // Fresnel / Atmosphere rim light
    float fresnel = pow(1.0 - max(dot(norm, ViewDir), 0.0), 3.0);
    vec3 rimColor = mix(objectColor, vec3(0.6, 0.8, 1.0), 0.5); // Atmospheric blue tint
    vec3 rim = fresnel * rimColor * 0.4;

    vec3 result = (ambient + diffuse) * objectColor + specular + rim;
    
    // Simple gamma correction
    result = pow(result, vec3(1.0/1.8));
    
    FragColor = vec4(result, 1.0);
}
