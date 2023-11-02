#version 460 core

layout(location = 0) out vec4 FragColor0; //Original
layout(location = 1) out vec4 FragColor1; //Brighness

in vec2 TexCoords;

uniform sampler2D colorBuffer_0;
uniform sampler2D colorBuffer_1;

//--HDR
uniform bool hdr;
uniform float exposure;
uniform float gamma;
//-----------------------------


void main()
{
    vec3 originalColor = texture(colorBuffer_0, TexCoords).rgb;
    vec3 brightnessColor = texture(colorBuffer_1, TexCoords).rgb;

    //if(hdr)
    //{
    //    // exposure tone mapping
    //    vec3 mapped = vec3(1.0) - exp(-originalColor * exposure);

    //    // gamma correction 
    //    mapped = pow(mapped, vec3(1.0 / gamma));
  
    //    FragColor0 = vec4(mapped, 1.0);
    //}
    //else
    //{
    //    FragColor0 = vec4(originalColor, 1.0);
    //}

    FragColor0 = vec4(originalColor, 1.0);
    FragColor1 = vec4(brightnessColor, 1.0);
}