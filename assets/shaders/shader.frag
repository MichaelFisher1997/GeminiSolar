#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 lightPos = vec3(0.0, 0.0, 0.0); 
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    
    // Ambient - Increased from 0.05 to 0.2
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Simple hack for Sun: if it's at the center (approx), it's the light source
    if (length(fragPos) < 5.0) { 
         ambient = vec3(1.0);
         diffuse = vec3(0.0); 
    }

    vec3 result = (ambient + diffuse) * fragColor;
    outColor = vec4(result, 1.0);
}
