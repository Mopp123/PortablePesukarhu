#version 330

precision mediump float;

attribute vec2 vertexPos;

attribute vec3 pos;
attribute vec2 scale;
attribute vec2 texOffset;
attribute vec4 color;

struct PushConstantData
{
    mat4 projMat;
    float atlasRows;
};
uniform PushConstantData pushConstants;

out vec2 var_uvCoord;
out vec4 var_color;

void main()
{
    vec2 scaledVertex = vertexPos * scale;
    vec3 positionedVertex = vec3(scaledVertex, 0.0) + pos;

    gl_Position = pushConstants.projMat * vec4(positionedVertex, 1.0);
    var_uvCoord = (vertexPos * vec2(1.0, -1.0) + texOffset) / pushConstants.atlasRows;
    var_color = color;
}
