#pragma once

typedef const void * SCITER_ELEMENT;
typedef const void * HWINDOW;

__interface IWindowDestroySink
{
    void OnWindowDestroy(HWINDOW hWnd);
};

__interface ISciterWindow
{
    void CenterWindow(void) = 0;
    HWINDOW GetHandle() const = 0;
    SCITER_ELEMENT GetRootElement(void) const = 0;
    void OnDestroySinkAdd(IWindowDestroySink * Sink) = 0;
    void OnDestroySinkRemove(IWindowDestroySink * Sink) = 0;
};

__interface IBaseElement
{
    bool GetInterface(const char * riid, void ** object) = 0;
};

__interface IWidget
{
    void Attached(SCITER_ELEMENT element, IBaseElement * baseElement) = 0;
    void Detached(SCITER_ELEMENT element) = 0;
    bool GetInterface(const char * riid, void ** object) = 0;
};

__interface ISciterUI;
typedef IWidget *(__stdcall * tyCreateWidget)(ISciterUI & SciterUI);

__interface ISciterUI
{
    bool AttachHandler(SCITER_ELEMENT elemHandle, const char * riid, void * pinterface) = 0;
    bool GetElementInterface(SCITER_ELEMENT elemHandle, const char * riid, void ** pinterface) = 0;
    bool WindowCreate(HWINDOW parent, const char * baseHtml, int x, int y, int width, int height, ISciterWindow *& window) = 0;
    bool RegisterWidgetType(const char * name, tyCreateWidget createWidget, const char * widgetCss) = 0;
    void Run() = 0;
    void Stop() = 0;
    void Shutdown() = 0;
};

bool SciterUIInit(const char * languageDir, const char * baseLanguage, const char * currentLanguage, bool Console, ISciterUI *& sciterUI);
