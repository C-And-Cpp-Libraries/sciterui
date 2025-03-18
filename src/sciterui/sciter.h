#pragma once

#include "resource_manager.h"
#include "sciter_window.h"
#include <memory>
#include <sciter_ui.h>
#include <set>
#include <stdint.h>
#include <vector>

namespace SciterUI
{

class Sciter :
    public ISciterUI
{
    typedef std::set<SciterWindow *> WindowSet;
    typedef std::vector<std::unique_ptr<SciterWindow>> WindowList;

public:

    Sciter(const char * languageDir);

    bool Initialize(const char * baseLanguage, const char * currentLanguage, bool console);
    void WindowCreated(SciterWindow * window);
    void WindowDestroyed(SciterWindow * window);

    // ISciterUI
    bool WindowCreate(HWINDOW parent, const char * baseHtml, int x, int y, int width, int height, ISciterWindow *& window);
    void Run();
    void Stop();
    void Shutdown();

    ResourceManager & GetResourceManager(void);

private:
    Sciter() = delete;
    Sciter(const Sciter &) = delete;
    Sciter & operator=(const Sciter &) = delete;

    ResourceManager m_resourceManager;
    WindowSet m_windows;
    WindowList m_CreatedWindows;
};

} // namespace SciterUI