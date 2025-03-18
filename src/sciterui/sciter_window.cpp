#include "sciter_window.h"
#include "sciter.h"
#include "std_string.h"
#include <sciter-x-api.h>
#include <sciter-x-def.h>
#include <stdint.h>

#undef HWINDOW

namespace SciterUI
{

SciterWindow::SciterWindow(Sciter & sciter) :
    m_sciter(sciter),
    m_hWnd(nullptr),
    m_bound(false),
    m_destroyed(false)
{
}

SciterWindow::~SciterWindow()
{
}

bool SciterWindow::Create(HWINDOW parentWinow, const char * htmlFile, int x, int y, int width, int height)
{
    RECT Frame;
    Frame.left = x;
    Frame.top = y;
    Frame.right = x + width;
    Frame.bottom = y + height;
    UINT creationFlags = 0;
    m_hWnd = ::SciterCreateWindow(creationFlags, (Frame.right - Frame.left) > 0 ? &Frame : nullptr, nullptr, this, (HWND)parentWinow);
    if (m_hWnd != nullptr)
    {
        m_sciter.WindowCreated(this);
        LoadHtml(htmlFile);
        ::SciterWindowExec((HWND)m_hWnd, SCITER_WINDOW_SET_STATE, SCITER_WINDOW_STATE_SHOWN, 0);
    }
    return m_hWnd != nullptr;
}

void SciterWindow::CenterWindow(void)
{
    int32_t xScreen = GetSystemMetrics(SM_CXSCREEN), x;
    int32_t yScreen = GetSystemMetrics(SM_CYSCREEN), y;
    RECT rc;
    if (!GetWindowRect((HWND)m_hWnd, &rc))
    {
        return;
    }
    HWND parent = GetParent((HWND)m_hWnd);
    RECT parentRect;
    if (parent != nullptr && !::IsIconic(parent) && ::IsWindowVisible(parent) && GetWindowRect(parent, &parentRect))
    {
        int32_t width = rc.right - rc.left;
        int32_t height = rc.bottom - rc.top;
        x = ((parentRect.right - parentRect.left) - (width)) / 2 + parentRect.left;
        y = ((parentRect.bottom - parentRect.top) - (height)) / 2 + parentRect.top;
        if (x < 0 && width < xScreen)
        {
            x = 0;
        }
        if (y < 0 && height < yScreen)
        {
            y = 0;
        }
        if (x + width > xScreen)
        {
            x = xScreen - width;
        }
        if (y + height > yScreen)
        {
            y = yScreen - height;
        }
    }
    else
    {
        x = (xScreen - (rc.right - rc.left)) / 2;
        y = (yScreen - (rc.bottom - rc.top)) / 2;
    }
    SetWindowPos((HWND)m_hWnd, 0, x, y, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);
}

HWINDOW SciterWindow::GetHandle() const
{
    return m_hWnd;
}

SCITER_ELEMENT SciterWindow::GetRootElement(void) const
{
    HELEMENT h = 0;
    SciterGetRootElement((HWND)m_hWnd, &h);
    return h;
}

void SciterWindow::OnDestroySinkAdd(IWindowDestroySink * Sink)
{
    m_onDestroySink.insert(Sink);
}

void SciterWindow::OnDestroySinkRemove(IWindowDestroySink * Sink)
{
    WinDestroySinks::iterator itr = m_onDestroySink.find(Sink);
    if (itr != m_onDestroySink.end())
    {
        m_onDestroySink.erase(itr);
    }
}

bool SciterWindow::GetDestroyed(void) const
{
    return m_destroyed;
}

void SciterWindow::SetDestroyed(void)
{
    if (m_destroyed)
    {
        return;
    }
    m_destroyed = true;
}

void SciterWindow::Bind()
{
    if (m_hWnd && !m_bound)
    {
        m_bound = true;
        SciterSetCallback((HWND)m_hWnd, (LPSciterHostCallback)SciterCallback, this);
    }
}

bool SciterWindow::LoadHtml(const char * url)
{
    Bind();
    std::wstring loadUrl = stdstr_f(_strnicmp(url, "file://", 7) == 0 ? "%s" : "file://%s", url).ToUTF16();
    return FALSE != ::SciterLoadFile((HWND)m_hWnd, loadUrl.c_str());
}


LRESULT SciterWindow::HandleNotification(LPSCITER_CALLBACK_NOTIFICATION pnm)
{
    switch (pnm->code)
    {
    case SC_LOAD_DATA:
        return OnLoadData((LPSCN_LOAD_DATA)pnm);
    case SC_ENGINE_DESTROYED:
        return OnEngineDestroyed();
    }
    return 0;
}

LRESULT SciterWindow::OnLoadData(LPSCN_LOAD_DATA pnmld)
{
    if (pnmld == nullptr)
    {
        return LOAD_DISCARD;
    }

    ResourceManager & manager = m_sciter.GetResourceManager();
    std::unique_ptr<uint8_t> data;
    uint32_t dataSize = 0;
    if (!manager.LoadResource(pnmld->uri, data, dataSize))
    {
        return LOAD_DISCARD;
    }
    ::SciterDataReady((HWND)pnmld->hwnd, pnmld->uri, data.get(), dataSize);
    return LOAD_OK;
}

LRESULT SciterWindow::OnEngineDestroyed(void)
{
    for (WinDestroySinks::const_iterator itr = m_onDestroySink.begin(); itr != m_onDestroySink.end(); itr++)
    {
        (*itr)->OnWindowDestroy(m_hWnd);
    }
    m_sciter.WindowDestroyed(this);
    return 0;
}

UINT SciterWindow::SciterCallback(LPSCITER_CALLBACK_NOTIFICATION pnm, LPVOID param)
{
    SciterWindow * Self = (SciterWindow *)param;
    return (UINT)Self->HandleNotification(pnm);
}
} // namespace SciterUI
