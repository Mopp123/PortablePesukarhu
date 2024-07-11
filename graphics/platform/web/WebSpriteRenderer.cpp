#include "WebSpriteRenderer.h"
#include "../../../ecs/components/renderable/Sprite3DRenderable.h"
#include "../../../ecs/components/lighting/Lights.h"
#include "shaders/sources/WebTestShader.h"
#include "../../../core/Application.h"
#include "../../../core/Timing.h"
#include "../../../Common.h"


namespace pk
{
    namespace web
    {

        static std::string s_vertexSource = R"(
            precision mediump float;

            attribute vec2 vertPos;
            attribute vec3 worldPos;
            attribute vec2 uv;

	    uniform mat4 projectionMatrix;
	    uniform mat4 viewMatrix;

	    varying vec2 var_uv;

	    // this is kind of like a "fake normal" for now.. to apply at least some kind of lighting effect(dark night time)..
	    //varying vec3 var_normal;

            void main()
            {

	    	// Dont apply "rotation part" of view mat -> sprite always faces camera

	    	vec3 camRight = vec3(viewMatrix[0][0],viewMatrix[1][0],viewMatrix[2][0]);
	    	vec3 camUp = vec3(viewMatrix[0][1],viewMatrix[1][1],viewMatrix[2][1]);

	    	vec3 finalVertexPos =
	    		worldPos +
	    		(camRight * vertPos.x) +
	    		(camUp * vertPos.y);

	    	gl_Position = projectionMatrix * viewMatrix * vec4(finalVertexPos, 1.0);
	    	var_uv = uv;
	    	//var_normal = vec3(0,1.0,0);
	    }
        )";

        static std::string s_fragmentSource = R"(
            precision mediump float;

	    varying vec2 var_uv;
	    //varying vec3 var_normal;

	    uniform sampler2D textureSampler;
	    //uniform vec3 dirLight_dir;

            void main()
            {
	    	//vec3 lightDir = normalize(dirLight_dir);
	    	//float diffFactor = clamp(dot(-lightDir, normalize(var_normal)), 0.0, 1.0);

	    	vec4 texColor = texture2D(textureSampler, var_uv);

	    	vec4 finalColor = texColor;

	    	gl_FragColor = finalColor;

	    	if(texColor.a <= 0.1)
	    		discard;
	    }
        )";

        WebSpriteRenderer::WebSpriteRenderer() :
            _shader(s_vertexSource, s_fragmentSource)
        {
            _vertexAttribLocation_pos = _shader.getAttribLocation("vertPos");
            _vertexAttribLocation_worldPos = _shader.getAttribLocation("worldPos");
            _vertexAttribLocation_uv = _shader.getAttribLocation("uv");

            _uniformLocation_projMat = _shader.getUniformLocation("projectionMatrix");
            _uniformLocation_viewMat = _shader.getUniformLocation("viewMatrix");
            _uniformLocation_texSampler = _shader.getUniformLocation("textureSampler");

            //_uniformLocation_dirLight_dir = _shader.getUniformLocation("dirLight_dir");
            //_uniformLocation_dirLight_color = _shader.getUniformLocation("dirLight_color");


            // Load tex atlas
            _textureAtlas = new WebTexture(
                "assets/Effects.png",
                {
                    TextureSamplerFilterMode::PK_SAMPLER_FILTER_MODE_NEAR,
                    TextureSamplerAddressMode::PK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                    2
                }
            );

            const int maxBatchInstanceCount = 512;
            const int batchInstanceDataLen = 7 * 4; // (vertexPos:vec2, worldPos: vec3, uv:vec2 --> 7 floats)

            allocateBatches(4, maxBatchInstanceCount * batchInstanceDataLen, batchInstanceDataLen);
        }

        WebSpriteRenderer::~WebSpriteRenderer()
        {
            for (BatchData& batch : _batches)
            {
                batch.destroy();
            }

            delete _textureAtlas;
        }

        // submit renderable component for rendering (batch preparing, before rendering)
        void WebSpriteRenderer::submit(const Component* const renderableComponent, const mat4& transformation)
        {
            // <#M_DANGER>
            const Sprite3DRenderable* const  renderable = (const Sprite3DRenderable* const)(renderableComponent);
            // SpriteRenderer uses texture solely for "batch identifying"
            const Texture* spriteTexture = renderable->texture;

            const vec3& pos = renderable->position;
            const vec2& scale = renderable->scale;
            const vec2& texOffset = renderable->textureOffset;

            // We need to negate a single pixel on some uvs, otherwise ugly artifacts on tiles' borders
            const float pixelUvSpace = (1.0f / _texAtlasWidth) /2.0f;
            const float texAtlasRows = (const float)spriteTexture->getTiling();

            vec2 uv_v0(0, 1);
            vec2 uv_v1(0, 0);
            vec2 uv_v2(1, 0);
            vec2 uv_v3(1, 1);

            uv_v0 = (uv_v0 + texOffset) / texAtlasRows;
            uv_v1 = (uv_v1 + texOffset) / texAtlasRows;
            uv_v2 = (uv_v2 + texOffset) / texAtlasRows;
            uv_v3 = (uv_v3 + texOffset) / texAtlasRows;

            vec2 v_0(-0.5f, 1.0f);
            vec2 v_1(-0.5f, 0.0f);
            vec2 v_2(0.5f, 0.0f);
            vec2 v_3(0.5f, 1.0f);

            const vec2 halfScale = scale * 0.5f;

            // ..dont remember why we are like.. flipped on y here..
            std::vector<float> dataToSubmit{
                -0.5f * scale.x,	0.0f,			pos.x, pos.y, pos.z,		uv_v0.x,		uv_v0.y - pixelUvSpace,
                    -0.5f * scale.x,	1.0f * scale.y,		pos.x, pos.y, pos.z,		uv_v1.x,		uv_v1.y,
                    0.5f * scale.x,	1.0f * scale.y,		pos.x, pos.y, pos.z,		uv_v2.x - pixelUvSpace, uv_v2.y,
                    0.5f * scale.x,	0.0f,			pos.x, pos.y, pos.z,		uv_v3.x - pixelUvSpace, uv_v3.y - pixelUvSpace
            };

            // Find suitable batch
            for (int i = 0; i < _batches.size(); ++i)
            {
                BatchData& batch = _batches[i];

                if (batch.isFree)
                {
                    batch.occupy(0, spriteTexture);
                    batch.insertInstanceData(0, dataToSubmit);
                    batch.addNewInstance();
                    return;
                }
                else if (spriteTexture == batch.texture && !batch.isFull)
                {
                    batch.insertInstanceData(0, dataToSubmit);
                    batch.addNewInstance();
                    return;
                }
                /*else
                  {
                  Debug::log("Batch was full!");
                  }*/
            }
        }


        void WebSpriteRenderer::render(const Camera& cam)
        {

            const mat4& projectionMatrix = cam.getProjMat3D();

            Scene* scene = Application::get()->accessCurrentScene();

            // Find scene's active camera's transform
            // This quite disgusting, just atm for testing purposes..
            const Transform * const camTransform = (Transform*)scene->getComponent(cam.getEntity(), ComponentType::PK_TRANSFORM);
            mat4 viewMat = camTransform->getTransformationMatrix();
            viewMat.inverse(); // omg this quite heavy operation ...

            glUseProgram(_shader.getProgramID());

            // all common uniforms..
            _shader.setUniform(_uniformLocation_projMat, projectionMatrix);
            _shader.setUniform(_uniformLocation_viewMat, viewMat);

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            glDisable(GL_CULL_FACE);
            //glCullFace(GL_FRONT);

            for (BatchData& batch : _batches)
            {
                if (batch.isFree)
                    continue;

                WebVertexBuffer* vb = (WebVertexBuffer*)batch.vertexBuffers[0];
                WebIndexBuffer* ib = (WebIndexBuffer*)batch.indexBuffer;

                // hardcoded just as temp..
                const GLsizei instanceIndexCount = 6;

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->getID());

                int stride = sizeof(float) * 7;
                glBindBuffer(GL_ARRAY_BUFFER, vb->getID());

                // vertex pos vertex attrib
                glEnableVertexAttribArray(_vertexAttribLocation_pos);
                glVertexAttribPointer(_vertexAttribLocation_pos, 2, GL_FLOAT, GL_FALSE, stride, 0);

                // world pos vertex attrib
                glEnableVertexAttribArray(_vertexAttribLocation_worldPos);
                glVertexAttribPointer(_vertexAttribLocation_worldPos, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 2));

                // uv coord vertex attrib
                glEnableVertexAttribArray(_vertexAttribLocation_uv);
                glVertexAttribPointer(_vertexAttribLocation_uv, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 5));



                // Bind texture
                if (batch.texture)
                {
                    const WebTexture* webTexture = (const WebTexture*)(batch.texture);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, webTexture->getID());
                }
                else
                {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, _textureAtlas->getID());
                }

                glDrawElements(GL_TRIANGLES, instanceIndexCount * batch.getInstanceCount(), GL_UNSIGNED_SHORT, 0);

                glBindTexture(GL_TEXTURE_2D, 0);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                // JUST TEMPORARELY FREE HERE!!!
                batch.clear();
            }
        }



        void WebSpriteRenderer::allocateBatches(int maxBatchCount, int maxBatchLength, int entryLength)
        {
            std::vector<float> vertexData(maxBatchLength);
            std::vector<unsigned short> indices(maxBatchLength);

            const int vertexCount = 4;
            int vertexIndex = 0;
            for (int i = 0; i < maxBatchLength; i += 6)
            {
                if (i + 5 >= indices.size())
                    break;

                indices[i] =	 0 + vertexIndex;
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
