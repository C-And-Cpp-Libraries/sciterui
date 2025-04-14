#pragma once
#include <memory>

namespace SciterUI
{
__interface IModuleResource
{
    bool LoadResource(const wchar_t * name, const wchar_t * type, std::unique_ptr<uint8_t> & data, uint32_t & size);
};
} // namespace SciterUI