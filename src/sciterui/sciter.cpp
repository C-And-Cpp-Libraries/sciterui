#include "sciter.h"
#include "sciter_window.h"

#include "sciter-x-api.h"

#undef HWINDOW

namespace SciterUI
{

Sciter::Sciter(const char * languageDir) :
    m_resourceManager(languageDir)
{
}

bool Sciter::Initialize(const char * baseLanguage, const char * currentLanguage, bool /*console*/)
{
    if (!m_resourceManager.Initialize(baseLanguage, currentLanguage))
    {
        return false;
    }
    SciterExec(SCITER_APP_INIT, (UINT_PTR)0, (UINT_PTR) nullptr);
    return true;
}

void Sciter::WindowCreated(SciterWindow * window)
{
    m_windows.insert(window);
}

void Sciter::WindowDestroyed(SciterWindow * window)
{
    WindowSet::iterator itr = m_windows.find(window);
    if (itr != m_windows.end())
    {
        if (!window->GetDestroyed())
        {
            window->SetDestroyed();
        }
        m_windows.erase(itr);
    }
}

bool Sciter::WindowCreate(HWINDOW parent, const char * baseHtml, int x, int y, int width, int height, ISciterWindow *& window)
{
    std::unique_ptr<SciterWindow> sciterWindow(new SciterWindow(*this));
    if (!sciterWindow->Create(parent, baseHtml, x, y, width, height))
    {
        return false;
    }
    window = sciterWindow.get();
    m_CreatedWindows.emplace_back(std::move(sciterWindow));
    return true;
}

void Sciter::Run()
{
    SciterExec(SCITER_APP_LOOP, 0, 0);
}

void Sciter::Stop()
{
    SciterExec(SCITER_APP_STOP, 0, 0);
}

void Sciter::Shutdown()
{
    SciterExec(SCITER_APP_SHUTDOWN, 0, 0);
}

ResourceManager & Sciter::GetResourceManager(void)
{
    return m_resourceManager;
}

} // namespace SciterUI
