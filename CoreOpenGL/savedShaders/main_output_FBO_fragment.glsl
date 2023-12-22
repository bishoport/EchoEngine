#version 460 core

layout(location = 0) out vec4 FragColor0;

in vec2 TexCoords;


uniform sampler2D colorBuffer_0;


void main()
{             
    vec3 color0 = texture(colorBuffer_0, TexCoords).rgb;

    FragColor0 = vec4(color0, 1.0);
}
