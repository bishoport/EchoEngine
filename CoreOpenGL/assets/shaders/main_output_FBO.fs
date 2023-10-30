#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

//--HDR
uniform float exposure;
uniform float gamma;
//-------------------

uniform sampler2D colorBuffer;


void main()
{             
    vec3 color = texture(colorBuffer, TexCoords).rgb;

    //// Ajustar la exposici�n antes del tonemapping
    //color *= pow(2.0, exposure);
    //// HDR tonemapping
    //color = color / (color + vec3(1.0));
    //// Aplicar la correcci�n gamma
    //color = pow(color, vec3(1.0 / gamma));

    FragColor = vec4(color, 1.0);
}