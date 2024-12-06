#include "GUIImage.h"
#include "pesukarhu/core/Application.h"


namespace pk
{
    namespace ui
    {
        GUIImage::GUIImage(const GUIImage& other) :
            _entity(other._entity)
        {
        }

        void GUIImage::create(ImgCreationProperties creationProperties)
        {
        }

        GUIRenderable* getGUIRenderable()
        {
            return nullptr;
        }
    }
}
