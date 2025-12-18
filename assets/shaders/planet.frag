#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;
uniform int isSun; // 1 if Sun, 0 otherwise

void main() {
    if (isSun == 1) {
        FragColor = vec4(objectColor, 1.0);
        return;
    }

    float ambientStrength = 0.2;
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightPos = vec3(0.0, 0.0, 0.0); 
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}
