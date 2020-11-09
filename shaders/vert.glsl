#version 450 core

// input
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 8) in vec2 vTexC;

layout (location = 9) uniform mat4 model;
layout (location = 2) uniform mat4 mvp;

// output
out vec3 FragPos;
out vec2 fTexC;
out vec3 fNorm;

void main()
{

    gl_Position = mvp*vec4(vPos, 1.0f);
    FragPos = vec3(model * vec4(vPos, 1.0));

    // piping
    fTexC = vTexC;
    fNorm = vNorm;
}