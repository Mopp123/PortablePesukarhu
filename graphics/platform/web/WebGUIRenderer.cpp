
#include "WebGUIRenderer.h"
#include "shaders/sources/WebTestShader.h"
#include "WebFontRenderer.h"
#include "../../../core/Timing.h"
#include "../../../Common.h"


namespace pk
{
    namespace web
    {
        static std::string s_vertexSource = R"(
            precision mediump float;

            attribute vec2 position;
            attribute vec2 uv;
            attribute vec4 color;
            attribute vec4 additionalProperties;

            uniform mat4 projectionMatrix;

            varying vec2 var_uv;
            varying vec4 var_color;
            varying vec4 var_additionalProperties;

            void main()
            {
                gl_Position = projectionMatrix * vec4(position, 0, 1.0);
                var_uv = uv;
                var_color = color;
                var_additionalProperties = additionalProperties;
            }
        )";


        static std::string s_fragmentSource = R"(
            precision mediump float;

            varying vec2 var_uv;
            varying vec4 var_color;
            varying vec4 var_additionalProperties;

            uniform sampler2D textureSampler;

            const float borderThickness = 1.0;

            #define RECT_WIDTH (var_additionalProperties.x)
            #define RECT_HEIGHT (var_additionalProperties.y)
            #define DRAW_BORDER (var_additionalProperties.z == 1.0)

            void main()
            {
                vec2 uvCoord = var_uv;
                vec4 texColor = texture2D(textureSampler, uvCoord);
                //float fr = mix(texColor.r, var_color.r, var_color.r)

                vec4 finalColor = texColor * var_color;

                vec2 pos = vec2(uvCoord.x * RECT_WIDTH, uvCoord.y * RECT_HEIGHT);
                vec4 borderColor = vec4(var_color.rgb * 10.0, 1.0);
                if (borderColor.r > 1.0)
                    borderColor.r = 1.0;
                if (borderColor.g > 1.0)
                    borderColor.g = 1.0;
                if (borderColor.b > 1.0)
                    borderColor.b = 1.0;

                if (DRAW_BORDER)
                {
                    if (pos.x <= borderThickness || pos.y <= borderThickness ||
                        pos.x >= RECT_WIDTH - borderThickness || pos.y >= RECT_HEIGHT - borderThickness
                    )
                    {
                        finalColor = borderColor;
                    }
                }

                gl_FragColor = finalColor;
            }
        )";


        WebGUIRenderer::WebGUIRenderer() :
            _shader(s_vertexSource, s_fragmentSource)
        {
            _vertexAttribLocation_pos = _shader.getAttribLocation("position");
            _vertexAttribLocation_uv = _shader.getAttribLocation("uv");
            _vertexAttribLocation_color = _shader.getAttribLocation("color");
            _vertexAttribLocation_properties = _shader.getAttribLocation("additionalProperties");

            _uniformLocation_projMat = _shader.getUniformLocation("projectionMatrix");
            _uniformLocation_texSampler = _shader.getUniformLocation("textureSampler");

            // Create default texture, if we want to just draw a colored rect
            char* defaultTextureData = new char[1 * 4];
            memset(defaultTextureData, 255, 4);
            _defaultTexture = new WebTexture(
                defaultTextureData, 1, 1, 4,
                {
                    TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_NEAR,
                    TextureSamplerAddressMode::PK_SAMPLER_ADDRESS_MODE_REPEAT
                }
            );
            delete[] defaultTextureData;

            const int maxBatchInstanceCount = 256;
            const int batchInstanceDataLen = 12 * 4;
            allocateBatches(4, maxBatchInstanceCount * batchInstanceDataLen, batchInstanceDataLen);
        }

        WebGUIRenderer::~WebGUIRenderer()
        {
            for (BatchData& batch : _batches)
            {
                batch.destroy();
            }

            delete _defaultTexture;
        }

        // submit renderable component for rendering (batch preparing, before rendering)
        void WebGUIRenderer::submit(const Component* const renderableComponent, const mat4& transformation)
        {
            // <#M_DANGER>
            const GUIRenderable* const renderable = (const GUIRenderable* const)(renderableComponent);
            const vec2 pos(transformation[0 + 3 * 4], transformation[1 + 3 * 4]);
            const vec2 scale(transformation[0 + 0 * 4], transformation[1 + 1 * 4]);
            const vec3& color = renderable->color;
            const vec4 properties(scale.x, scale.y, (float)renderable->drawBorder, 0.0f);
            const vec4& texCropping = renderable->textureCropping;

            const vec2 uv_v0(texCropping.x, texCropping.y); // top left
            const vec2 uv_v1(texCropping.x, texCropping.y + texCropping.w); // bottom left
            const vec2 uv_v2(texCropping.x + texCropping.z, texCropping.y + texCropping.w); // bottom right
            const vec2 uv_v3(texCropping.x + texCropping.z, texCropping.y); // rop right

            std::vector<float> dataToSubmit{
                pos.x, pos.y, uv_v0.x, uv_v0.y, color.x,color.y, color.z, 1.0f, properties.x, properties.y, properties.z, properties.w,
                pos.x, pos.y - scale.y, uv_v1.x, uv_v1.y, color.x,color.y, color.z, 1.0f, properties.x, properties.y, properties.z, properties.w,
                pos.x + scale.x, pos.y - scale.y, uv_v2.x, uv_v2.y, color.x, color.y, color.z, 1.0f, properties.x, properties.y, properties.z, properties.w,
                pos.x + scale.x, pos.y, uv_v3.x, uv_v3.y, color.x, color.y, color.z, 1.0f, properties.x, properties.y, properties.z, properties.w,
            };

            // Find suitable batch
            for (int i = 0; i < _batches.size(); ++i)
            {
                BatchData& batch = _batches[i];
                if (batch.isFull)
                {
                    //Debug::log("Batch was full!");
                    continue;
                }

                /*
                if (batch.isFree)
                {
                    batch.occupy(0, renderable->texture);
                    batch.insertInstanceData(0, dataToSubmit);
                    batch.addNewInstance();
                    //Debug::log("created new batch");
                    return;
                }
                else if (batch.ID == renderable->textureID)
                {
                    batch.insertInstanceData(0, dataToSubmit);
                    batch.addNewInstance();
                    //Debug::log("assigned to existing");
                    return;
                }
                */

                // ---
                if (batch.isFree)
                {
                    batch.occupy(0, renderable->texture);
                    batch.insertInstanceData(0, dataToSubmit);
                    batch.addNewInstance();
                    return;
                }
                else if (renderable->texture == batch.texture && !batch.isFull)
                {
                    batch.insertInstanceData(0, dataToSubmit);
                    batch.addNewInstance();
                    return;
                }
            }
        }


        void WebGUIRenderer::render(const Camera& cam)
        {
            if (_batches.empty())
                return;

            mat4 projectionMatrix = cam.getProjMat2D();

            glUseProgram(_shader.getProgramID());

            // all common uniforms..
            _shader.setUniform(_uniformLocation_projMat, projectionMatrix);

            //glDisable(GL_DEPTH_TEST);
            glDepthFunc(GL_ALWAYS);

            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            for (BatchData& batch : _batches)
            {
                if (batch.isFree)
                    continue;


                WebVertexBuffer* vb = (WebVertexBuffer*)batch.vertexBuffers[0];
                WebIndexBuffer* ib = (WebIndexBuffer*)batch.indexBuffer;

                // hardcoded just as temp..
                const GLsizei instanceIndexCount = 6;

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getID());

                int stride = sizeof(float) * 12;
                glBindBuffer(GL_ARRAY_BUFFER, vb->getID());

                // position vertex attrib
                glEnableVertexAttribArray(_vertexAttribLocation_pos);
                glVertexAttribPointer(_vertexAttribLocation_pos, 2, GL_FLOAT, GL_FALSE, stride, 0);

                // uv coord vertex attrib
                glEnableVertexAttribArray(_vertexAttribLocation_uv);
                glVertexAttribPointer(_vertexAttribLocation_uv, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 2));

                // color vertex attrib
                glEnableVertexAttribArray(_vertexAttribLocation_color);
                glVertexAttribPointer(_vertexAttribLocation_color, 4, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 4));

                // additional properties attrib
                glEnableVertexAttribArray(_vertexAttribLocation_properties);
                glVertexAttribPointer(_vertexAttribLocation_properties, 4, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 8));

                glActiveTexture(GL_TEXTURE0);
                // Bind texture only if we have one...
                if (batch.texture)
                {
                    const WebTexture* texture = (const WebTexture*)batch.texture;
                    glBindTexture(GL_TEXTURE_2D, texture->getID());
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, _defaultTexture->getID());
                }

                glDrawElements(GL_TRIANGLES, instanceIndexCount * batch.getInstanceCount(), GL_UNSIGNED_SHORT, 0);

                glBindTexture(GL_TEXTURE_2D, 0);


                glEnableVertexAttribArray(_vertexAttribLocation_pos);

                glDisableVertexAttribArray(_vertexAttribLocation_uv);
                glDisableVertexAttribArray(_vertexAttribLocation_color);
                glDisableVertexAttribArray(_vertexAttribLocation_properties);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


                // JUST TEMPORARELY FREE HERE!!!
                batch.clear();
            }
        }



        void WebGUIRenderer::allocateBatches(int maxBatchCount, int maxBatchLength, int entryLength)
        {
            std::vector<float> vertexData(maxBatchLength);
            std::vector<unsigned short> indices(maxBatchLength);

            const int vertexCount = 4;
            int vertexIndex = 0;
            for (int i = 0; i < maxBatchLength; i += 6)
            {
                if (i + 5 >= indices.size())
                    break;

                indices[i] =     0 + vertexIndex;
                indices[i + 1] = 1 + vertexIndex;
                indices[i + 2] = 2 + vertexIndex;
                indices[i + 3] = 2 + vertexIndex;
                indices[i + 4] = 3 + vertexIndex;
                indices[i + 5] = 0 + vertexIndex;

                vertexIndex += vertexCount;
            }

            for (int i = 0; i < maxBatchCount; ++i)
            {
                WebVertexBuffer* vb = new WebVertexBuffer(vertexData, VertexBufferUsage::PK_BUFFER_USAGE_DYNAMIC);
                WebIndexBuffer* ib = new WebIndexBuffer(indices);

                _batches.push_back({ entryLength, maxBatchLength, {vb}, ib });
            }
        }
    }
}
