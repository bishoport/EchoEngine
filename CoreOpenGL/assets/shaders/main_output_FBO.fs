#version 460 core

layout(location = 0) out vec4 FragColor0;
layout(location = 1) out vec4 FragColor1;

in vec2 TexCoords;


uniform sampler2D colorBuffer_0;
uniform sampler2D colorBuffer_1;


void main()
{             
    vec3 color0 = texture(colorBuffer_0, TexCoords).rgb;
    vec3 color1 = texture(colorBuffer_1, TexCoords).rgb;

    FragColor0 = vec4(color0, 1.0);
    FragColor1 = vec4(color1, 1.0);
}