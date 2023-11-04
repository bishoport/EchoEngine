#version 460 core
#include "common.glsl"

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

in vec4 shadowCoord_dirLight;


//--FUNCTIONS
vec3 getNormalFromMap();
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

vec3 CalcDirLight(DirLight light);
vec3 CalcPointLight(PointLight light);
vec3 CalcSpotLight(SpotLight light);

float CalcDirectionalLightShadow(DirLight light);
float CalcSpotLightShadow(SpotLight light);
// ----------------------------------------------------------------------------


//--COMMON
vec3 albedo;
float metallic;
float roughness;
float ao;
vec3 N;
vec3 V;
vec3 R;
vec3 F0;
vec2 adjustedTexCoords;
//-------------------

// IBL
uniform float exposure;
uniform float gamma;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
//-------------------

uniform vec2 repetitionFactor; // Factor de repetición

void main()
{

    adjustedTexCoords = TexCoords * repetitionFactor;

    if (material.hasAlbedoMap) {
        vec3 textureColor = pow(texture(material.albedoMap, adjustedTexCoords).rgb, vec3(2.2));
        albedo = textureColor * material.albedo; // Tonaliza la textura con el valor de material.albedo
    }
    else 
    {
        albedo = material.albedo;
    }

    if (material.hasMetallicMap) 
    {
        float textureMetallic = texture(material.metallicMap, adjustedTexCoords).r;
        metallic = textureMetallic * material.metallic; // Tonaliza la textura con el valor de material.metallic
    }
    else 
    {
        metallic = material.metallic;
    }

    if (material.hasRougnessMap) 
    {
        float textureRoughness = texture(material.roughnessMap, adjustedTexCoords).r;
        roughness = textureRoughness * material.roughness; // Tonaliza la textura con el valor de material.roughness
    }
    else {
        roughness = material.roughness;
    }


    if (material.hasAoMap)
        ao = texture(material.aoMap, adjustedTexCoords).r;
    else
        ao = 1.0;


    N = getNormalFromMap();      
    V = normalize(viewPos - FragPos);
    R = reflect(-V, N);

    // calculate reflectance at N incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    F0 = vec3(material.reflectance);
    F0 = mix(F0, albedo, metallic);



    //FASE DE ILUMINACION
    vec3 Lo = vec3(0.0);
    
    // phase 1: directional lighting
    if(useDirLight == true)
    {
        float shadow = 1.0f;
        if(dirLight.drawShadows == true)
        {
            shadow = CalcDirectionalLightShadow(dirLight);
        }
        Lo += CalcDirLight(dirLight) * shadow;
    }
        
    // phase 2: point lights
    for (int i = 0; i < numPointLights; ++i) 
    {
        Lo += CalcPointLight(pointLights[i]);
    } 
    
    // phase 3: spot light
    for(int i = 0; i < numSpotLights; i++)
    {
        float shadow = 1.0f;
        if(spotLights[i].drawShadows == true)
        {
            shadow = CalcSpotLightShadow(spotLights[i]);
        }

        Lo += CalcSpotLight(spotLights[i]) * shadow;
    }
    //--------------------------------------------------------------------

    Lo += material.hdrMultiply;




    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = mix(albedo, irradiance * albedo, material.hdrIntensity);

    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;     
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    

    //COMBINAMOS TODO
    vec3 ambient = (((kD * diffuse * globalAmbient) + specular) * ao);
    
    vec3 color = ambient + Lo;

    // Ajustar la exposición antes del tonemapping
    color *= pow(2.0, material.exposure);
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Aplicar la corrección gamma
    color = pow(color, vec3(1.0 / material.gamma));

    FragColor = vec4(color, 1.0);
}








vec3 CalcDirLight(DirLight light)
{
    if (light.isActive == false)
        return vec3(0.0f);

    vec3 L = normalize(-light.direction);
    vec3 H = normalize(V + L);

    // Calculate radiance, assuming it comes from far away
    vec3 radiance = light.diffuse;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;

    // kS is equal to Fresnel
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    return vec3(kD * albedo / PI + specular) * radiance * NdotL;
}

float CalcDirectionalLightShadow(DirLight light)
{
    float shadow = 1.0f;
    vec3 L = normalize(light.position - FragPos);
    vec4 shadowCoord = light.shadowBiasMVP * vec4(FragPos, 1.0);
    vec3 sCoord = shadowCoord.xyz/shadowCoord.w;
    float bias = 0.005 * tan(acos(dot(N,L)));

    if (light.usePoisonDisk == true)
    {
        for (int i=0; i<64; i++)
        {
            if (texture(light.shadowMap, sCoord.xy + poissonDisk[i]/300.0).r < sCoord.z-bias)
                shadow -= 1./64.;
        }
    }
    else
    {
        if (texture(light.shadowMap, sCoord.xy).r < sCoord.z-bias)
            shadow = 0.0;
    }

    // Aplicar shadowIntensity
    shadow = mix(shadow, 1.0, light.shadowIntensity);

    return shadow;
}



vec3 CalcSpotLight(SpotLight light)
{
    if (light.isActive == false)
        return vec3(0.0f);

    vec3 L = normalize(light.position - FragPos);
    vec3 H = normalize(V + L);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Calculate the spotlight intensity based on its cutoff
    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    if(intensity == 0.0) 
        return vec3(0.0);

    vec3 radiance = light.diffuse * intensity * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;

    // kS is equal to Fresnel
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS; 
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    return vec3(kD * albedo / PI + specular) * radiance * NdotL;
}

float CalcSpotLightShadow(SpotLight light)
{
    float shadow = 1.0f;

    vec3 L = normalize(light.position - FragPos);

    vec4 shadowCoord = light.shadowBiasMVP * vec4(FragPos,1.0);

    vec3 sCoord=shadowCoord.xyz/shadowCoord.w;

    float bias = 0.005 * tan(acos(dot(N,L)));

    for (int i=0;i<64;i++)
    {
        if ( texture( light.shadowMap, sCoord.xy + poissonDisk[i]/300.0 ).r < sCoord.z-bias )
            shadow-=1./64.;
    }

    // Aplicar shadowIntensity
    shadow = mix(shadow, 1.0, light.shadowIntensity);

    return shadow;
}


vec3 CalcPointLight(PointLight light)
{
    if (light.isActive == false)
        return vec3(0.0f);

    // calculate per-light radiance
    vec3 L = normalize(light.position - FragPos);
    vec3 H = normalize(V + L);

    float distance = length(light.position - FragPos);

    if (distance > light.range)
        return vec3(0.0f);

    float distanceRatio = distance / light.range;
    float threshold1 = 1.0 - light.lightSmoothness;  // Start of the transition
    float threshold2 = 1.0;  // End of the transition
    float fadeFactor = 1.0 - smoothstep(threshold1, threshold2, distanceRatio);
    float attenuation = fadeFactor / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 radiance = (light.ambient * light.strength)  * attenuation;


    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;
        
    // kS is equal to Fresnel
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic; 

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);  

    return vec3(kD * albedo / PI + specular) * radiance * NdotL;
}




// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(material.normalMap, adjustedTexCoords).xyz * 2.0 - 1.0;
    tangentNormal = mix(vec3(0.0, 0.0, 1.0), tangentNormal, material.normalIntensity);  // Usamos mix para interpolar entre la N sin alterar y la N del mapa.

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(adjustedTexCoords);
    vec2 st2 = dFdy(adjustedTexCoords);

    vec3 N   = normalize(Normal);
    vec3 T   = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B   = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);

    //vec3 tangentNormal = texture(material.normalMap, adjustedTexCoords).xyz * 2.0 - 1.0;

    //vec3 Q1  = dFdx(FragPos);
    //vec3 Q2  = dFdy(FragPos);
    //vec2 st1 = dFdx(adjustedTexCoords);
    //vec2 st2 = dFdy(adjustedTexCoords);

    //vec3 N   = normalize(Normal);
    //vec3 T   = normalize(Q1*st2.t - Q2*st1.t);
    //vec3 B   = -normalize(cross(N, T));
    //mat3 TBN = mat3(T, B, N);

    //return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------