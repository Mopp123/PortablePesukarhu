#include "graphics/Texture.h"


namespace pk
{
    namespace opengl
    {
        class OpenglTexture : public Texture_new
        {
        private:
            friend class Texture_new;
            uint32_t _glTexID = 0;

        public:
            ~OpenglTexture();
            inline uint32_t getGLTexID() const { return _glTexID; }

        protected:
            OpenglTexture(
                uint32_t imgResourceID,
                TextureSampler sampler
            );
        };
    }
}
