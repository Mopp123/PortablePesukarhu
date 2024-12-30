#version 330

precision mediump float;

attribute vec2 vertexPos;
attribute vec2 uvCoord;

attribute vec3 pos;
attribute vec2 scale;
attribute vec4 color;
attribute float useFilter;
attribute vec4 textureCropping;

struct Common
{
    mat4 projMat;
};

uniform Common commonProperties;

out vec2 var_uvCoord;
out vec4 var_color;
out vec2 var_fragPos;
out vec2 var_scale;
out float var_useFilter;

void main()
{
    vec2 scaledVertex = vertexPos * scale;
    vec3 positionedVertex = vec3(scaledVertex, 0.0) + pos;
    gl_Position = commonProperties.projMat * vec4(positionedVertex, 1.0);

    vec2 croppingPos = textureCropping.xy;
    vec2 croppingScale = textureCropping.zw;
    var_uvCoord = (uvCoord + croppingPos) * croppingScale;

    var_color = color;
    vec4 transformedPos = vec4((vertexPos * scale), 0, 1.0);
    var_fragPos = transformedPos.xy;
    var_scale = scale;
    var_useFilter = useFilter;
}
