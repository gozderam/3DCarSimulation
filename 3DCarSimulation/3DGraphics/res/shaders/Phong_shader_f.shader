#version 330 core

// structures
// all data in world coordinates
struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    float power;
};

// all data in world coordinates
struct StationaryLight
{
    vec3 position;
    vec3 color;
    float power;
};

// all data in world coordinates
struct Spotlight
{
    vec3 position;
    vec3 direction;
    vec3 color;
    float power;
};

// all data in world coordinates
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform Material material;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform int useTexture_f;

uniform StationaryLight stationaryLights_f[2];
uniform DirectionalLight directionalLights_f[1];
uniform Spotlight spotlights_f[2];

uniform vec3 viewPos;

uniform float day_n_night_factor;

uniform int fogEnabled_f;
uniform float fogFactor_f;
const float density = 0.04;
const float gradient = 1.1;
float n_shiny = 5.0;

vec3 addDirectionalLights(vec3 color, vec3 Normal, vec3 Diffuse, vec3 Specular, vec3 viewDir)
{
    for (int i = 0; i < 1; ++i)
    {
        vec3 lightDir = normalize(-directionalLights_f[i].direction);
        vec3 R = reflect(-lightDir, normalize(Normal)); // R = (2dot(N, L))N - L

        vec3 diffuse = Diffuse * directionalLights_f[i].color * max(dot(Normal, lightDir), 0.0);
        vec3 specular = Specular * directionalLights_f[i].color * pow(max(dot(viewDir, R), 0.0), n_shiny);

        color += day_n_night_factor * (diffuse + specular);
    }
    return color;
}

vec3 addStationaryLights(vec3 color, vec3 FragPos, vec3 Normal, vec3 Diffuse, vec3 Specular, vec3 viewDir)
{
    for (int i = 0; i < 2; ++i)
    {
        vec3 lightDir = normalize(stationaryLights_f[i].position - FragPos);
        vec3 R = reflect(-lightDir, normalize(Normal)); // R = (2dot(N, L))N - L
        float distance = length(stationaryLights_f[i].position - FragPos);

        vec3 diffuse = Diffuse * stationaryLights_f[i].color * max(dot(Normal, lightDir), 0.0);    
        vec3 specular = Specular * stationaryLights_f[i].color * pow(max(dot(viewDir, R), 0.0), n_shiny);

        color += stationaryLights_f[i].power * ( diffuse + specular) / (distance*distance);
    }
    return color;
}

vec3 addSpotlights(vec3 color, vec3 FragPos, vec3 Normal, vec3 Diffuse, vec3 Specular, vec3 viewDir)
{
    for (int i = 0; i < 2; ++i)
    {
        vec3 lightDir = normalize(spotlights_f[i].position - FragPos);
        vec3 R = reflect(-lightDir, normalize(Normal)); // R = (2dot(N, L))N - L
        float distance = length(spotlights_f[i].position - FragPos);
        float angle_cos = max(dot(lightDir, normalize(-spotlights_f[i].direction)), 0);

        vec3 diffuse = Diffuse * spotlights_f[i].color * max(dot(Normal, lightDir), 0.0);
        vec3 specular = Specular * spotlights_f[i].color * pow(max(dot(viewDir, R), 0.0), n_shiny);

        color += pow(angle_cos, 7) * spotlights_f[i].power * (diffuse + specular) / (distance * distance);
    }
    return color;
}

vec3 addFog(vec3 color, vec3 FragPos)
{
    if (fogEnabled_f == 1)
    {
        float distanceFromCamera = length(viewPos - FragPos);
        vec3 fogColor = vec3(day_n_night_factor, day_n_night_factor, day_n_night_factor);
        float visibility = exp(-pow(distanceFromCamera * density, gradient));
        visibility = clamp(visibility, 0, 1);
        float x = (1 - visibility) * fogFactor_f;
        return mix(fogColor, color, 1 - x);
    }
    return color;
}


void main()
{
    vec3 Diffuse;
    vec3 Specular;
    if (useTexture_f == 1)
    {
        Diffuse = texture(texture_diffuse1, TexCoords).rgb;
        Specular = texture(texture_diffuse1, TexCoords).rgb;
    }
    else
    {
        Diffuse = material.diffuse;
        Specular = material.specular;
    }

    vec3 currColor = Diffuse * 0.1; // ambient 
    vec3 viewDir = normalize(viewPos - FragPos);
 
    currColor = addStationaryLights(currColor, FragPos, Normal, Diffuse, Specular, viewDir);
    currColor = addDirectionalLights(currColor, Normal, Diffuse, Specular, viewDir);
    currColor = addSpotlights(currColor, FragPos, Normal, Diffuse, Specular, viewDir);
    currColor = addFog(currColor, FragPos);

    FragColor = vec4(currColor, 1.0);
}