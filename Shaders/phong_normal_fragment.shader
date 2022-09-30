#version 330 core
uniform vec3 u_color;
out vec4 FragColor;
in vec2 UV;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;

uniform vec3 u_light_pos;
uniform vec3 u_camera_pos;
uniform vec3 u_ambient_color;

float attenuate(float value, float maximum) {
    float clampedValue = min(value, maximum);
    return 1.0 / (pow(5 * clampedValue / maximum, 2) + 1);
}

void main()
{
    vec3 lightVector = normalize(u_light_pos - FragPos);

    float distance = length(u_light_pos - FragPos);

    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    vec3 tbnNormal = texture(texture_normal, UV).rgb;

    tbnNormal = tbnNormal * 2.0 - 1.0;
    tbnNormal = normalize(TBN * tbnNormal);

    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_camera_pos - FragPos);

    //phong shading
    vec3 reflectDir = reflect(-lightVector, tbnNormal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.1), 16);

    //blinn phong shading
    //float spec = 0.0;
    //if (diff)

    vec3 specular = specularStrength * spec * lightColor;

    vec3 diffuse = vec3(max(dot(tbnNormal, lightVector), 0.0)) * lightColor;
    vec3 ambient = u_ambient_color * lightColor;

    float gradient = attenuate(distance, 10.0);

    //FragColor = vec4(tbnNormal,1.0);
    //FragColor = texture(texture_diffuse, UV) * vec4(ambient + (diffuse + specular) * gradient, 1.0);
    FragColor = texture(texture_diffuse, UV) * vec4(ambient + (diffuse + specular), 1.0);
}