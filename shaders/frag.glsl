#version 450 core

// input
in vec3 FragPos;
in vec3 fNorm;
in vec2 fTexC;
uniform sampler2D ourTexture;

layout (location = 3) uniform vec3 ambientLGT;
layout (location = 4) uniform float ambientSTR;
layout (location = 5) uniform vec3 lightPos;

// output
out vec4 fragColor;

void main()
{
    vec3 norm = normalize(fNorm);
    vec3 lightDir = normalize(lightPos - FragPos);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * ambientLGT;
    
    // texture(ourTexture, fTexC)
    fragColor = texture(ourTexture, fTexC) * vec4(ambientLGT + diffuse,1.0f) * ambientSTR;
    

} 