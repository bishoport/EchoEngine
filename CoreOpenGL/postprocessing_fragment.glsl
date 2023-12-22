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


//--COLOR CURVES (LUT)
uniform bool colorCurvesLUT;
uniform sampler2D lookupTableTexture0;
uniform sampler2D lookupTableTexture1;

uniform float sunPosition;

uniform float lutIntensity;  // Intensidad del efecto
uniform float lutGamma;  // Intensidad del efecto
uniform float shadowIntensity;  // Intensidad de la sombra
uniform float midtoneIntensity; // Intensidad del tono medio
uniform float highlightIntensity; // Intensidad de la luz alta

vec3 adjustShadows(vec3 color, float shadows) {
    // Esta funcin atena o amplifica las sombras de la imagen.
    // shadows: -1.0 (ms oscuro) a 1.0 (ms claro), 0.0 es neutral
    float shadowStrength = clamp(shadows, -1.0, 1.0);
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    float shadowMask = smoothstep(0.0, 0.5, luminance); // Esto determina cunto afectar el ajuste a las sombras.
    
    // Aplicar el ajuste basado en el shadowStrength
    color = mix(color, color + shadowStrength * shadowMask, shadowMask);
    return color;
}

vec3 adjustMidtones(vec3 color, float midtones) {
    // Esta funcin ajusta los tonos medios.
    // midtones: -1.0 (ms oscuro) a 1.0 (ms claro), 0.0 es neutral
    float midtoneStrength = clamp(midtones, -1.0, 1.0);
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    float midtoneMask = smoothstep(0.25, 0.75, luminance); // Esto determina cunto afectar el ajuste a los tonos medios.
    
    // Aplicar el ajuste basado en el midtoneStrength
    color = mix(color, color + midtoneStrength * midtoneMask, midtoneMask);
    return color;
}

vec3 adjustHighlights(vec3 color, float highlights) {
    // Esta funcin amplifica o atena las luces altas.
    // highlights: -1.0 (ms oscuro) a 1.0 (ms claro), 0.0 es neutral
    float highlightStrength = clamp(highlights, -1.0, 1.0);
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    float highlightMask = smoothstep(0.5, 1.0, luminance); // Esto determina cunto afectar el ajuste a las luces altas.
    
    // Aplicar el ajuste basado en el highlightStrength
    color = mix(color, color + highlightStrength * highlightMask, highlightMask);
    return color;
}


vec3 adjustTonality(vec3 color, float shadows, float midtones, float highlights) {
    // Aplica ajustes a las sombras, tonos medios y altas luces
    // Este es un pseudocdigo y requerira una funcin real basada en tu necesidad
    color = adjustShadows(color, shadows);
    color = adjustMidtones(color, midtones);
    color = adjustHighlights(color, highlights);
    return color;
}
//-----------------------------

//--TONNEMAPING ACES
uniform bool ACES;
uniform float ACES_a; // Controla la intensidad de las luces. Valores ms altos resultan en realces ms brillantes.
uniform float ACES_b; // Afecta la transicin de las medias tonalidades a las luces altas, actuando como un offset.
uniform float ACES_c; // Ajusta el contraste general de la imagen. Valores ms altos aumentan el contraste.
uniform float ACES_d; // Afecta las sombras/midtones, desempeando un papel en el contraste de reas ms oscuras.
uniform float ACES_e; // Es un trmino de normalizacin para asegurarse de que los negros se mapean correctamente.

vec3 RRTAndODTFit(vec3 v)
{
    vec3 a = v * (v + 0.0245786) - 0.000090537;
    vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return a / b;
}

vec3 ACESFilm(vec3 x)
{
    return clamp((x*(ACES_a*x+ACES_b))/(x*(ACES_c*x+ACES_d)+ACES_e), 0.0, 1.0);
}
//-----------------------------


//--SATURATION_COLOR
uniform bool SATURATION;
uniform float saturationRed;   // Saturacin para el canal rojo
uniform float saturationGreen; // Saturacin para el canal verde
uniform float saturationBlue;  // Saturacin para el canal azul

uniform vec3 whiteBalanceColor; // Color para el balance de blancos

vec3 rgb2yuv(vec3 rgb) {
    // Esta es una matriz de conversin estndar
    mat3 rgb2yuvMatrix = mat3(
        0.299, 0.587, 0.114,
       -0.147, -0.289, 0.436,
        0.615, -0.515, -0.100
    );
    return rgb2yuvMatrix * rgb;
}

vec3 yuv2rgb(vec3 yuv) {
    // Esta es la matriz inversa de la de arriba
    mat3 yuv2rgbMatrix = mat3(
        1.0, 0.0, 1.139,
        1.0, -0.394, -0.580,
        1.0, 2.032, 0.0
    );
    return yuv2rgbMatrix * yuv;
}



vec3 ApplyColorCorrection(vec3 color, float saturation, vec3 balance) {
    // Convertir a espacio YUV o HSV, ajustar la saturacin y luego convertir de vuelta
    vec3 yuv = rgb2yuv(color);
    yuv.yz *= saturation; // Slo ajusta los canales U y V para la saturacin
    vec3 rgb = yuv2rgb(yuv);

    // Ajustar el balance de blancos multiplicando por un color de 'balance' dado
    rgb *= balance;

    return rgb;
}

vec3 AdjustSaturationPerChannel(vec3 color, vec3 saturation) {
    // Convertir a espacio YUV
    vec3 yuv = rgb2yuv(color);
    // Ajustar la saturacin por canal
    yuv.y *= saturation.x; // Saturacin para el canal 'U' que afecta principalmente al rojo y al azul
    yuv.z *= saturation.y; // Saturacin para el canal 'V' que afecta principalmente al verde y al rojo
    // Convertir de vuelta a RGB
    return yuv2rgb(yuv);
}

vec3 adjustSaturation(vec3 color, float saturationRed, float saturationGreen, float saturationBlue) {
    vec3 gray = vec3(dot(color, vec3(0.299, 0.587, 0.114)));
    return vec3(
        gray.r + saturationRed * (color.r - gray.r),
        gray.g + saturationGreen * (color.g - gray.g),
        gray.b + saturationBlue * (color.b - gray.b)
    );
}
//-----------------------------

void main()
{
    vec3 originalColor = texture(colorBuffer_0, TexCoords).rgb;
    vec3 brightnessColor = texture(colorBuffer_1, TexCoords).rgb;

    vec3 finalColorScene = originalColor;


    if(colorCurvesLUT) 
    {
        float u  =  floor(finalColorScene.b * 15.0) / 15.0 * 240.0;
              u  = (floor(finalColorScene.r * 15.0) / 15.0 *  15.0) + u;
              u /= 255.0;
        float v  = 1.0 - (floor(finalColorScene.g * 15.0) / 15.0);

        vec3 left0 = texture(lookupTableTexture0, vec2(u, v)).rgb;
        vec3 left1 = texture(lookupTableTexture1, vec2(u, v)).rgb;

        u  =  ceil(finalColorScene.b * 15.0) / 15.0 * 240.0;
        u  = (ceil(finalColorScene.r * 15.0) / 15.0 *  15.0) + u;
        u /= 255.0;
        v  = 1.0 - (ceil(finalColorScene.g * 15.0) / 15.0);

        vec3 right0 = texture(lookupTableTexture0, vec2(u, v)).rgb;
        vec3 right1 = texture(lookupTableTexture1, vec2(u, v)).rgb;

        float interpolatedSunPosition = sin(sunPosition * 3.141516f);
        interpolatedSunPosition = 0.5 * (interpolatedSunPosition + 1);

        vec3 left  = mix(left0,  left1,  interpolatedSunPosition);
        vec3 right = mix(right0, right1, interpolatedSunPosition);

        vec3 colorLUT = vec3(
            mix(left.r, right.r, fract(finalColorScene.r * 15.0)),
            mix(left.g, right.g, fract(finalColorScene.g * 15.0)),
            mix(left.b, right.b, fract(finalColorScene.b * 15.0))
        );

        // Aplica la intensidad del LUT mezclando la imagen afectada por la LUT con la original
        finalColorScene.rgb = mix(finalColorScene.rgb, colorLUT, lutIntensity);
        finalColorScene.rgb = adjustTonality(finalColorScene.rgb, shadowIntensity, midtoneIntensity, highlightIntensity);
        // Si deseas aplicar gamma despus de la LUT:
        if(lutGamma > 0.0f)
        {
            finalColorScene.rgb = pow(finalColorScene.rgb, vec3(lutGamma));
        }
    }

    if(hdr)
    { 
        vec3 mapped = vec3(1.0) - exp(-finalColorScene * exposure);
        mapped = pow(mapped, vec3(1.0 / gamma));
        finalColorScene = mapped;
    }

    if(SATURATION)
    {
        //finalColorScene = AdjustSaturationPerChannel(finalColorScene, vec3(saturationRed, saturationGreen, saturationBlue));
        //finalColorScene *= whiteBalanceColor;

        // Aplicar el ajuste de saturacin individual para cada canal
        finalColorScene = adjustSaturation(finalColorScene, saturationRed, saturationGreen, saturationBlue);
        // Aplicar el balance de blancos
        finalColorScene *= whiteBalanceColor;
    }


    if(ACES)
    {
        // Aplicar la curva de tonemapping ACES a la imagen HDR
        finalColorScene = ACESFilm(finalColorScene);
    }


    

    FragColor0 = vec4(finalColorScene, 1.0);
    FragColor1 = vec4(finalColorScene, 1.0);
}


//  if(colorCurvesLUT) {

//        finalColorScene.rgb = pow(finalColorScene.rgb, vec3(gamma));

//        float u  =  floor(finalColorScene.b * 15.0) / 15.0 * 240.0;
//              u  = (floor(finalColorScene.r * 15.0) / 15.0 *  15.0) + u;
//              u /= 255.0;
//        float v  = 1.0 - (floor(finalColorScene.g * 15.0) / 15.0);

//        vec3 left0 = texture(lookupTableTexture0, vec2(u, v)).rgb;
//        vec3 left1 = texture(lookupTableTexture1, vec2(u, v)).rgb;

//        u  =  ceil(finalColorScene.b * 15.0) / 15.0 * 240.0;
//        u  = (ceil(finalColorScene.r * 15.0) / 15.0 *  15.0) + u;
//        u /= 255.0;
//        v  = 1.0 - (ceil(finalColorScene.g * 15.0) / 15.0);

//        vec3 right0 = texture(lookupTableTexture0, vec2(u, v)).rgb;
//        vec3 right1 = texture(lookupTableTexture1, vec2(u, v)).rgb;

//        float interpolatedSunPosition = sin(sunPosition * 3.141516f);
//        interpolatedSunPosition = 0.5 * (interpolatedSunPosition + 1);

//        vec3 left  = mix(left0,  left1,  interpolatedSunPosition);
//        vec3 right = mix(right0, right1, interpolatedSunPosition);

//        finalColorScene.r = mix(left.r, right.r, fract(finalColorScene.r * 15.0));
//        finalColorScene.g = mix(left.g, right.g, fract(finalColorScene.g * 15.0));
//        finalColorScene.b = mix(left.b, right.b, fract(finalColorScene.b * 15.0));

//        finalColorScene.rgb = pow(finalColorScene.rgb, vec3(gamma.x));
//    }
