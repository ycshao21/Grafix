#version 450

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UBO{
    vec4 Color;
} ubo;

void main()
{
    outColor = ubo.Color;
}