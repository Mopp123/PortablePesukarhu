#include "resources/Texture.h"


namespace pk
{
    namespace opengl
    {
        class OpenglTexture : public Texture
        {
        private:
            friend class Texture;
            uint32_t _glTexID = 0;

            int _width = 0;
            int _height = 0;
            int _channels = 1;

        public:
            ~OpenglTexture();
            inline uint32_t getGLTexID() const { return _glTexID; }

            virtual void update(
                void* pData,
                size_t dataSize,
                size_t width,
                size_t height,
                uint32_t glTextureUnit
            );

        protected:
            OpenglTexture(
                uint32_t imgResourceID,
                TextureSampler sampler
            );
        };
    }
}
