#include "resource_manager.h"
#include <algorithm>

namespace SciterUI
{

ResourceManager::ResourceManager(const char * languageDir) :
    m_languageDir(languageDir, "")
{
    m_resourceMap.emplace(RESOURCE_MAP::value_type(L"PNG", L"PNG"));
    m_resourceMap.emplace(RESOURCE_MAP::value_type(L"JPG", L"JPG"));
    m_resourceMap.emplace(RESOURCE_MAP::value_type(L"GIF", L"GIF"));
    m_resourceMap.emplace(RESOURCE_MAP::value_type(L"SVG", L"SVG"));
    m_resourceMap.emplace(RESOURCE_MAP::value_type(L"BMP", (const wchar_t *)RT_BITMAP));
    m_resourceMap.emplace(RESOURCE_MAP::value_type(L"ICO", (const wchar_t *)RT_GROUP_ICON));
    m_resourceMap.emplace(RESOURCE_MAP::value_type(L"CUR", (const wchar_t *)RT_GROUP_CURSOR));
    m_resourceMap.emplace(RESOURCE_MAP::value_type(L"HTM", (const wchar_t *)RT_HTML));
    m_resourceMap.emplace(RESOURCE_MAP::value_type(L"HTML", (const wchar_t *)RT_HTML));
    m_resourceMap.emplace(RESOURCE_MAP::value_type(L"CSS", L"CSS"));
    m_resourceMap.emplace(RESOURCE_MAP::value_type(L"INI", L"LANG"));
    m_resourceMap.emplace(RESOURCE_MAP::value_type(L"LNG", L"LANG"));
}

bool ResourceManager::Initialize(const char * baseLanguage, const char * currentLanguage)
{
    if (!m_languageDir.DirectoryExists())
    {
        return false;
    }
    m_moduleBase = LoadLanguageFile(baseLanguage);
    if (m_moduleBase == nullptr)
    {
        return false;
    }
    if (_stricmp(baseLanguage, currentLanguage) != 0)
    {
        m_moduleCurrent = LoadLanguageFile(currentLanguage);
        if (m_moduleCurrent == nullptr)
        {
            return false;
        }
    }
    else
    {
        m_moduleCurrent = m_moduleBase;
    }
    return true;
}

bool ResourceManager::LoadResource(const wchar_t * uri, std::unique_ptr<uint8_t> & data, uint32_t & size)
{
    if (m_moduleBase == nullptr || m_moduleCurrent == nullptr)
    {
        return false;
    }
    const wchar_t * ResourceType = wcsrchr(uri, '.');
    if (ResourceType == nullptr)
    {
        return false;
    }

    ResourceType += 1;
    std::wstring resourceTypeUpper = ResourceType;
    std::transform(resourceTypeUpper.begin(), resourceTypeUpper.end(), resourceTypeUpper.begin(), (wchar_t(*)(int))towupper);

    RESOURCE_MAP::const_iterator iter = m_resourceMap.find(resourceTypeUpper);
    if (iter == m_resourceMap.end())
    {
        return false;
    }
    std::unique_ptr<uint8_t> ResourceData;
    uint32_t ResourceSize = 0;
    if (!m_moduleCurrent->LoadResource(uri, iter->second, ResourceData, ResourceSize))
    {
        return false;
    }
    data.reset(ResourceData.release());
    size = ResourceSize;
    return true;
}

IModuleResource * ResourceManager::LoadLanguageFile(const char * language)
{
    Path moduleDir(m_languageDir);
    moduleDir.AppendDirectory(language);
    if (moduleDir.DirectoryExists())
    {
        std::unique_ptr<ModuleDisk> diskModule(new ModuleDisk());
        if (diskModule.get() != nullptr)
        {
            if (!diskModule->LoadModule(moduleDir))
            {
                return nullptr;
            }
            IModuleResource * module = diskModule.get();
            m_modulesDisk.emplace_back(std::move(diskModule));
            return module;
        }
    }
    return nullptr;
}

} // namespace SciterUI
