precision mediump float;

varying vec3 var_normal;
varying vec2 var_uvCoord;
varying vec4 var_ambientColor;
varying vec3 var_fragPos;
varying vec3 var_camPos;

varying vec4 var_dirLightDir;
varying vec4 var_dirLightColor;

varying float var_distToCam;


struct Material
{
    sampler2D diffuseTexSampler0;
    sampler2D specularTexSampler;
    vec4 color;
    vec4 properties;
};
uniform Material material;

// For gui to properly get rendered we have to discard some fragments
// here... otherwise the 3d bleeds through gui when model's face
// "impales" the camera (clipping due to zNear)
// TODO: Some better way of dealing with this!
const float overrideZNear = 1.0;

void main()
{
    if (var_distToCam <= overrideZNear)
        discard;

    float shadeless = material.properties.z;
    vec4 diffuseTexColor0 = texture2D(material.diffuseTexSampler0, var_uvCoord) * material.color;

    if (shadeless == 0.0)
    {
        float specularStrength = material.properties.x;
        float specularShininess = material.properties.y;

        //float specularStrength = 1.0;
        //float specularShininess = 0.0;

        vec3 normal = normalize(var_normal);
        vec3 lightDir = normalize(var_dirLightDir.xyz);

        float diffFactor = max(dot(normal, -lightDir), 0.0);
        vec4 diffuseColor = diffFactor * var_dirLightColor;


        //vec3 toCam = normalize(var_fragPos - var_camPos);
        //vec3 lightReflection = reflect(lightDir, normal);
        //float specularFactor = pow(max(dot(-toCam, lightReflection), 0.0), specularShininess);

        // That Blinn-Phong thing... BUT! I have no idea why the fuck need to *-1 the light dir
        // ..its fucked otherwise but.. it doesn't make any sense!!!
        vec3 viewDir = normalize(var_camPos - var_fragPos);
        vec3 halfWay = normalize(var_dirLightDir.xyz * -1.0 + viewDir);
        float specularFactor = pow(max(dot(normal, halfWay), 0.0), specularShininess);

        vec4 specularTexColor = texture2D(material.specularTexSampler, var_uvCoord);
        vec4 specularColor = specularStrength * specularFactor * var_dirLightColor * specularTexColor;

        vec4 finalColor = (var_ambientColor + diffuseColor + specularColor) * diffuseTexColor0;
        gl_FragColor = finalColor;
    }
    else
    {
        gl_FragColor = diffuseTexColor0;
    }
}
