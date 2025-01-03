#pragma once

#include "pesukarhu/graphics/CommandBuffer.h"
#include "pesukarhu/graphics/platform/opengl/OpenglPipeline.h"

namespace pk
{
    namespace web
    {
        // Attempt forward declaring RenderCommand here..
        // NOTE: NOT TESTED, THIS MAY GET FUCKED!
        class WebRenderCommand;

        class WebCommandBuffer : public CommandBuffer
        {
        private:
            friend class CommandBuffer;
            friend class WebRenderCommand;

            // On all opengl shit we need to know about currently bound pipeline
            // because of vertex buffer and descriptorset layouts
            // NOTE: we can use OpenglPipeline here, since atm it seems on emscripten(webgl1)
            // and "desktop" opengl we can use the same shit.. may require changing in future!
            //
            // NOTE: This makes static analysis error since unused here.
            // This will be assigned when calling RenderCommand's bindPipeline function
            opengl::OpenglPipeline* _pPipeline = nullptr; // Might cause some problems tho.. figure that out.. bitch..

            // Used to determine glDrawElements' type
            IndexType _drawIndexedType = IndexType::INDEX_TYPE_NONE;

        public:
            WebCommandBuffer(const WebCommandBuffer&) = delete;
            ~WebCommandBuffer() {}

            virtual void allocate() {}
            virtual void free() {}

        private:
            WebCommandBuffer() {}
        };
    }
}
