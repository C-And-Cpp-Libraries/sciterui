#include "sciter.h"
#include "sciter_element.h"
#include "sciter_window.h"
#include "std_string.h"

#include "sciter-x-api.h"

#undef HWINDOW

namespace SciterUI
{

Sciter::Sciter(const char * languageDir) :
    m_resourceManager(languageDir),
    m_nextWidgetId(1)
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

void Sciter::UpdateWindow(HWINDOW hwnd)
{
    SciterUpdateWindow((HWND)hwnd);
}

bool Sciter::AttachHandler(SCITER_ELEMENT elemHandle, const char * riid, void * pinterface)
{
    if (elemHandle == nullptr || riid == nullptr || pinterface == nullptr)
    {
        return false;
    }
    HWINDOW hWnd = SciterElement(elemHandle).GetElementHwnd(true);
    for (WindowSet::const_iterator itr = m_windows.begin(); itr != m_windows.end(); itr++)
    {
        SciterWindow * window = *itr;
        if (window->GetHandle() != hWnd)
        {
            continue;
        }
        return window->AttachHandler(elemHandle, riid, pinterface);
    }
    return false;
}

bool Sciter::GetElementInterface(SCITER_ELEMENT he, const char * riid, void ** pinterface)
{
    if (he == nullptr)
    {
        return false;
    }
    std::string widgetValue = SciterElement(he).GetAttribute("data-widget");
    if (widgetValue.size() == 0)
    {
        return false;
    }
    int windgetId = std::stoi(widgetValue, nullptr, 10);
    ElementMap::iterator itr = m_elementBases.find(windgetId);
    if (itr != m_elementBases.end())
    {
        return itr->second->GetInterface(riid, pinterface);
    }
    return false;
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

bool Sciter::SetElementHtmlFromResource(SCITER_ELEMENT elemHandle, const char * uri)
{
    SciterElement element(elemHandle);
    if (!element.IsValid() || uri == nullptr)
    {
        return false;
    }
    std::unique_ptr<uint8_t> Data;
    uint32_t Size = 0;
    if (!m_resourceManager.LoadResource(stdstr(uri).ToUTF16().c_str(), Data, Size))
    {
        element.SetHTML((uint8_t *)"", 0, SciterElement::SIH_REPLACE_CONTENT);
        return false;
    }
    element.SetHTML(Data.get(), Size, SciterElement::SIH_REPLACE_CONTENT);
    return true;
}

bool Sciter::WindowCreate(HWINDOW parent, const char * baseHtml, int x, int y, int width, int height, unsigned int flags, ISciterWindow *& window)
{
    std::unique_ptr<SciterWindow> sciterWindow(new SciterWindow(*this));
    if (!sciterWindow->Create(parent, baseHtml, x, y, width, height, flags))
    {
        return false;
    }
    window = sciterWindow.get();
    m_CreatedWindows.emplace_back(std::move(sciterWindow));
    return true;
}

bool Sciter::RegisterWidgetType(const char * name, tyCreateWidget createWidget, const char * widgetCss)
{
    if (name == nullptr || createWidget == nullptr)
    {
        return false;
    }
    WidgetMap::iterator iter = m_widgetFactory.find(name);
    if (iter != m_widgetFactory.end())
    {
        return false;
    }
    WidgetCallbackInfo widgetCallbackInfo;
    widgetCallbackInfo.sciter = this;
    widgetCallbackInfo.callback = createWidget;
    m_widgetFactory.insert(WidgetMap::value_type(name, widgetCallbackInfo));
    if (widgetCss != nullptr && strlen(widgetCss) > 0)
    {
        if (!SciterAppendMasterCSS((LPCBYTE)widgetCss, (uint32_t)strlen(widgetCss)))
        {
            return false;
        }
    }
    return true;
}

uint32_t Sciter::AttachWidget(LPSCN_ATTACH_BEHAVIOR lpab)
{
    if (lpab == nullptr)
    {
        return 0;
    }
    WidgetMap::iterator itr = m_widgetFactory.find(lpab->behaviorName);
    if (itr == m_widgetFactory.end())
    {
        return 0;
    }
    IWidget * pWidget = (itr->second.callback)(*this);
    if (pWidget == nullptr)
    {
        return 0;
    }
    uint32_t widgetID = m_nextWidgetId++;
    SciterElement(lpab->element).SetAttribute("data-widget", stdstr_f("%d", widgetID).c_str());
    itr->second.widgets.insert(IWidgetMap::value_type(widgetID, pWidget));
    lpab->elementTag = (void *)&itr->second;
    lpab->elementProc = (ElementEventProc *)stAttachWidgetProc;
    return 1;
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

int Sciter::AttachWidgetProc(WidgetCallbackInfo * info, SCITER_ELEMENT he, uint32_t evtg, void * prms)
{
    if (info == nullptr)
    {
        return 0;
    }

    if (evtg == HANDLE_INITIALIZATION)
    {
        INITIALIZATION_PARAMS * p = (INITIALIZATION_PARAMS *)prms;
        if (p->cmd == BEHAVIOR_ATTACH)
        {
            std::string widgetValue = SciterElement(he).GetAttribute("data-widget");
            if (widgetValue.size() == 0)
            {
                return 0;
            }
            int windgetId = std::stoi(widgetValue, nullptr, 10);
            IWidgetMap::const_iterator IWidgetIter = info->widgets.find(windgetId);
            if (IWidgetIter == info->widgets.end())
            {
                return 0;
            }

            std::shared_ptr<BaseElement> elementBase;
            ElementMap::iterator BaseIter = m_elementBases.find(windgetId);
            if (BaseIter != m_elementBases.end())
            {
                elementBase = BaseIter->second;
            }
            else
            {
                std::pair<ElementMap::iterator, bool> ret = m_elementBases.emplace(ElementMap::value_type(windgetId, std::make_shared<BaseElement>(*this, he)));
                elementBase = ret.first->second;
            }

            if (elementBase != nullptr)
            {
                IWidget * widget = IWidgetIter->second;
                elementBase->AddWidget(widget);
                widget->Attached(he, elementBase.get());
            }
        }
        else
        {
            __debugbreak();
        }
    }
    return 0;
}

int Sciter::stAttachWidgetProc(void * tag, SCITER_ELEMENT he, uint32_t evtg, void * prms)
{
    WidgetCallbackInfo * info = (WidgetCallbackInfo *)tag;
    if (info != nullptr)
    {
        return info->sciter->AttachWidgetProc(info, he, evtg, prms);
    }
    return false;
}

} // namespace SciterUI
