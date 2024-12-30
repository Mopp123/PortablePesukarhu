#version 330

precision mediump float;

attribute vec3 vertexPos;
attribute vec3 normal;
attribute vec2 uvCoord;

// Instanced stuff
attribute mat4 transformationMatrix;

struct Common3D
{
    mat4 projMat;
    mat4 viewMat;
    vec4 camPos;
};
uniform Common3D commonProperties;

struct Environment
{
    vec4 ambientColor;
};
uniform Environment environment;

out vec3 var_normal;
out vec2 var_uvCoord;
out vec4 var_ambientColor;
out vec3 var_fragPos;
out vec3 var_camPos;

out float var_distToCam;

void main()
{
    vec4 worldPos = transformationMatrix * vec4(vertexPos, 1.0);
    gl_Position = commonProperties.projMat * commonProperties.viewMat * worldPos;

    vec4 rotatedNormal = transformationMatrix * vec4(normal, 0.0);
    var_normal = rotatedNormal.xyz;
    var_uvCoord = uvCoord;
    var_ambientColor = environment.ambientColor;

    var_fragPos = worldPos.xyz;

    var_camPos = commonProperties.camPos.xyz;
    var_distToCam = gl_Position.w;
}
