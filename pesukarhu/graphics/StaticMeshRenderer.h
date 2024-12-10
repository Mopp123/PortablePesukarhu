#pragma once

#include "Renderer.h"

#include "pesukarhu/core/Debug.h"

#include <vector>
#include <unordered_map>

/*
namespace pk
{
    namespace web
    {
        class StaticMeshRenderer : public Renderer
        {
        private:

            WebShader _shader;

            PK_int _vertexAttribLocation_pos = -1;
            PK_int _vertexAttribLocation_worldPos = -1;
            PK_int _vertexAttribLocation_uv = -1;

            PK_int _uniformLocation_projMat = -1;
            PK_int _uniformLocation_viewMat = -1;
            PK_int _uniformLocation_texSampler = -1;

            PK_int _uniformLocation_dirLight_color = -1;
            PK_int _uniformLocation_dirLight_dir = -1;

            // Width of texture atlas(in pixels, but casted to float)
            const float _texAtlasWidth = 256.0f;
            WebTexture* _textureAtlas = nullptr;

            std::vector<BatchData> _batches;

        public:
            StaticMeshRenderer();
            ~StaticMeshRenderer();

            // submit renderable component for rendering (batch preparing, before rendering)
            virtual void submit(const Component* const renderableComponent, const mat4& transformation);

            virtual void render(const Camera& cam);

            virtual void resize(int w, int h) {}

        private:

            void allocateBatches(int maxBatchCount, int maxBatchLength, int entryLength);
        };
    }

}
*/
