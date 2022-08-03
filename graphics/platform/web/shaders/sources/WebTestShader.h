#pragma once

#include <string>

namespace pk
{
	namespace web
	{
		struct WebTestShader
		{
            const std::string vertexSource = R"(
                precision mediump float;
                
                attribute vec2 position;
                
                void main()
                {
                   gl_Position = vec4(position, 0, 1.0);
                }
            )";

            const std::string fragmentSource = R"(
                precision mediump float;
                
                
                void main()
                {
                    gl_FragColor = vec4(0,1,0,1);
                }
            )";
		};
	}
}