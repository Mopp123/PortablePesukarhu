#include "Descriptors.h"
#include "core/Debug.h"


namespace pk
{
    bool DescriptorSet::isValid(const DescriptorSetLayout& layout) const
    {
        const size_t buffersCount = _pBuffers.size();
        const size_t texturesCount = _pTextures.size();
        const size_t totalCount = buffersCount + texturesCount;
        bool isValid = totalCount == layout.getBindings().size();
        if (!isValid)
            Debug::log(
                "___TEST___invalid descriptor set! "
                "desc item count: " + std::to_string(totalCount) + " "
                "layout bindings: " + std::to_string(layout.getBindings().size())
            );
        return isValid;
    }
}
