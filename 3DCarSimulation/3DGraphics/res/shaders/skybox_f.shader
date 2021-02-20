#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float day_n_night_factor;

uniform int fogEnabled_f;
uniform float fogFactor_f;

const float lowerLimit = 0.0;
const float upperLimit = 0.45;

void main()
{
    vec4 finalColor =  day_n_night_factor * texture(skybox, TexCoords);
    if (fogEnabled_f == 1)
    {
        vec3 fogColor = vec3(day_n_night_factor, day_n_night_factor, day_n_night_factor);
        float factor = (TexCoords.y - lowerLimit) / (upperLimit - lowerLimit);
        factor = clamp(factor, 0.0, 1.0);
        float x = (1 - factor)  * fogFactor_f;
        FragColor = mix(vec4(fogColor, 1.0), finalColor,  1-x);
    }
    else
        FragColor = finalColor;
}