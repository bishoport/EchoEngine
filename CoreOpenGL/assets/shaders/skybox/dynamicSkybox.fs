#version 330 core

in vec3 TexCoords;

uniform vec3 sunPosition;
uniform vec3 sunDiskSize; // Tamaño del disco solar
uniform float gradientIntensity; // Intensidad del degradado
uniform float uAuraSize; // Tamaño del aura alrededor del sol
uniform float uAuraIntensity; // Intensidad del aura
uniform vec3 uDayLightColor;
uniform vec3 uSunsetColor; // Color intermedio para la puesta de sol
uniform vec3 uDayNightColor;
uniform vec3 uGroundColor;  // Color del suelo
uniform float uLowerBound;
uniform float uUpperBound;

out vec4 FragColor;

uniform samplerCube skyboxTexture;

void main() {
    // Normalizamos las posiciones para compararlas en el espacio de direcciones.
    vec3 normalizedPosition = normalize(TexCoords);
    vec3 normalizedSun = normalize(sunPosition);

    // Establecemos el intervalo de transición cuando el sol comienza a tocar el horizonte.
    float firstMixFactor = smoothstep(uLowerBound, (uLowerBound + uUpperBound) * 0.5, normalizedSun.y);
    float secondMixFactor = smoothstep((uLowerBound + uUpperBound) * 0.5, uUpperBound, normalizedSun.y);
    vec3 intermediateColor = mix(uDayNightColor, uSunsetColor, firstMixFactor);

    //Get current Texture Fragment
    vec4 currentTextureFragV4 = texture(skyboxTexture, TexCoords);
    vec3 currentTextureFragV3 = vec3(currentTextureFragV4.r,currentTextureFragV4.g,currentTextureFragV4.b);


    vec3 currentSkyColor = mix(intermediateColor, currentTextureFragV3, secondMixFactor);

    // Limitamos el color naranja a una bruma en el horizonte justo donde cruza el sol
    float horizonEffect = max(0.0, 1.0 - abs(normalizedPosition.y / normalizedSun.y));
    vec3 horizonColor = mix(currentSkyColor, uSunsetColor, horizonEffect);

    if (secondMixFactor > 0.0 && secondMixFactor < 1.0) {
        currentSkyColor = horizonColor;
    }

    // Calculamos la diferencia en la posición entre la posición actual y la posición del sol.
    float difference = length(normalizedPosition - normalizedSun);

    // No pintar el sol y usar el color uGroundColor cuando está por debajo de la línea del ecuador.
    if(normalizedPosition.y < 0.0) {
        FragColor = vec4(uGroundColor, 1.0); // Usar el color del suelo
        return;
    }

    // Si la diferencia es menor que el tamaño del disco solar ajustado por gradientIntensity, pintamos el sol blanco.
    if(difference < sunDiskSize.x * gradientIntensity) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else if(difference < sunDiskSize.x) {
        float innerGradient = smoothstep(sunDiskSize.x * gradientIntensity, sunDiskSize.x, difference);
        vec3 transitionColor = mix(vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 0.0), innerGradient);
        FragColor = vec4(transitionColor, 1.0);
    }
    // Añadir el aura alrededor del sol
    else if(difference < (sunDiskSize.x + uAuraSize)) {
        float outerAura = 1.0 - smoothstep(sunDiskSize.x, sunDiskSize.x + uAuraSize, difference);
        vec3 auraColor = mix(currentSkyColor, vec3(1.0, 1.0, 0.0), outerAura * uAuraIntensity);
        FragColor = vec4(auraColor, 1.0);
    }
    else {
        //FragColor = texture(skyboxTexture, TexCoords);
        FragColor = vec4(currentSkyColor, 1.0);
    }
}







//#version 330 core

//in vec3 TexCoords;

//uniform vec3 sunPosition;
//uniform vec3 sunDiskSize; // Tamaño del disco solar
//uniform float gradientIntensity; // Intensidad del degradado
//uniform float uAuraSize; // Tamaño del aura alrededor del sol
//uniform float uAuraIntensity; // Intensidad del aura
//uniform vec3 uDayLightColor;
//uniform vec3 uDayNightColor;

//vec3 blueColor = vec3(0.0f,0.0f,1.0f);

//out vec4 FragColor;

//void main() {
//    // Normalizamos las posiciones para compararlas en el espacio de direcciones.
//    vec3 normalizedPosition = normalize(TexCoords);
//    vec3 normalizedSun = normalize(sunPosition);

//    // Calculamos la diferencia en la posición entre la posición actual y la posición del sol.
//    float difference = length(normalizedPosition - normalizedSun);

//    // No pintar el sol cuando está por debajo de la línea del ecuador.
//    if(normalizedPosition.y < 0.0) {
//        FragColor = vec4(blueColor, 1.0);  // Color azul para el cielo.
//        return;
//    }

//    // Si la diferencia es menor que el tamaño del disco solar ajustado por gradientIntensity, pintamos el sol blanco.
//    if(difference < sunDiskSize.x * gradientIntensity) {
//        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
//    }
//    else if(difference < sunDiskSize.x) {
//        float innerGradient = smoothstep(sunDiskSize.x * gradientIntensity, sunDiskSize.x, difference);
//        vec3 transitionColor = mix(vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 0.0), innerGradient);
//        FragColor = vec4(transitionColor, 1.0);
//    }
//    // Añadir el aura alrededor del sol
//    else if(difference < (sunDiskSize.x + uAuraSize)) {
//        float outerAura = 1.0 - smoothstep(sunDiskSize.x, sunDiskSize.x + uAuraSize, difference);
//        vec3 auraColor = mix(blueColor, vec3(1.0, 1.0, 0.0), outerAura * uAuraIntensity);
//        FragColor = vec4(auraColor, 1.0);
//    }
//    else {
//        FragColor = vec4(blueColor, 1.0);  // Color azul para el cielo.
//    }
//}


















