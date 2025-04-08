#pragma once

#include "base_element.h"
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
    typedef std::map<uint32_t, IWidget *> IWidgetMap;
    typedef std::map<uint32_t, std::shared_ptr<BaseElement>> ElementMap;
    typedef std::set<SciterWindow *> WindowSet;

    struct WidgetCallbackInfo
    {
        WidgetCallbackInfo()
        {
            sciter = nullptr;
            callback = nullptr;
        }
        IWidgetMap widgets;
        Sciter * sciter;
        tyCreateWidget callback;
    };

    typedef std::vector<std::unique_ptr<SciterWindow>> WindowList;
    typedef std::map<std::string, WidgetCallbackInfo> WidgetMap;

public:
    typedef int __stdcall ElementEventProc(void * tag, SCITER_ELEMENT he, uint32_t evtg, void * prms);

    /**This structure is used by #SC_ATTACH_BEHAVIOR notification.
     *\copydoc SC_ATTACH_BEHAVIOR **/
    typedef struct SCN_ATTACH_BEHAVIOR
    {
        uint32_t code; /**< [in] one of the codes above.*/
        void * hwnd;   /**< [in] HWINDOW of the window this callback was attached to.*/

        SCITER_ELEMENT element;    /**< [in] target DOM element handle*/
        const char * behaviorName; /**< [in] zero terminated string, string appears as value of CSS behavior:"???" attribute.*/

        ElementEventProc * elementProc; /**< [out] pointer to ElementEventProc function.*/
        void * elementTag;              /**< [out] tag value, passed as is into pointer ElementEventProc function.*/

    } SCN_ATTACH_BEHAVIOR;
    typedef SCN_ATTACH_BEHAVIOR * LPSCN_ATTACH_BEHAVIOR;

    Sciter(const char * languageDir);

    bool Initialize(const char * baseLanguage, const char * currentLanguage, bool console);
    void WindowCreated(SciterWindow * window);
    void WindowDestroyed(SciterWindow * window);

    // ISciterUI
    bool AttachHandler(SCITER_ELEMENT elemHandle, const char * riid, void * pinterface);
    bool GetElementInterface(SCITER_ELEMENT he, const char * riid, void ** pinterface);
    bool SetElementHtmlFromResource(SCITER_ELEMENT elemHandle, const char * uri);
    bool WindowCreate(HWINDOW parent, const char * baseHtml, int x, int y, int width, int height, ISciterWindow *& window);
    bool RegisterWidgetType(const char * name, tyCreateWidget createWidget, const char * widgetCss);
    uint32_t AttachWidget(LPSCN_ATTACH_BEHAVIOR lpab);
    void UpdateWindow(HWINDOW hwnd);
    void Run();
    void Stop();
    void Shutdown();

    ResourceManager & GetResourceManager(void);

private:
    Sciter() = delete;
    Sciter(const Sciter &) = delete;
    Sciter & operator=(const Sciter &) = delete;

    int AttachWidgetProc(WidgetCallbackInfo * info, SCITER_ELEMENT he, uint32_t evtg, void * prms);
    static int __stdcall stAttachWidgetProc(void * tag, SCITER_ELEMENT he, uint32_t evtg, void * prms);
    ResourceManager m_resourceManager;
    WindowSet m_windows;
    WindowList m_CreatedWindows;
    WidgetMap m_widgetFactory;
    ElementMap m_elementBases;
    uint32_t m_nextWidgetId;
};

} // namespace SciterUI