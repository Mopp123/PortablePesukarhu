#version 330

precision mediump float;

in vec2 var_uvCoord;
in vec4 var_color;

uniform sampler2D texSampler;

void main()
{
    vec4 texColor = texture2D(texSampler, var_uvCoord);
    // NOTE: opengl switches alpha channel to red unlike webgl side!
    vec4 finalColor = var_color * texColor.r * 1.0; // The last * 1.0 is thickness..

    if(texColor.r <= 0.0)
        discard;

    //gl_FragColor = vec4(finalColor.rgb, 1.0);
    gl_FragColor = vec4(var_color.rgb, texColor.r);
}
