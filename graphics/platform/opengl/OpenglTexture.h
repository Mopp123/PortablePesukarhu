#include "graphics/Texture.h"


namespace pk
{
    namespace opengl
    {
        class OpenglTexture : public Texture_new
        {
        private:
            friend class Texture_new;

            uint32_t _id = 0;

        public:
            ~OpenglTexture();
            inline uint32_t getID() const { return _id; }

        protected:
            OpenglTexture(
                TextureSampler sampler,
                ImageData* pImgData,
                int tiling,
                bool saveDataHostSide
            );
        };
    }
}
