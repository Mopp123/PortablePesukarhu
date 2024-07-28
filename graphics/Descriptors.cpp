#include "Descriptors.h"
#include "core/Debug.h"


namespace pk
{
    bool DescriptorSet::isValid(const DescriptorSetLayout& layout) const
    {
        const size_t buffersCount = _pBuffers.size();
        const size_t texturesCount = _pTextures.size();
        const size_t totalCount = buffersCount + texturesCount;
        return totalCount == layout.getBindings().size();
    }
}
