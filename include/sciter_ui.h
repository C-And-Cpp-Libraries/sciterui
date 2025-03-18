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

__interface ISciterUI
{
    bool WindowCreate(HWINDOW parent, const char * baseHtml, int x, int y, int width, int height, ISciterWindow *& window) = 0;
    void Run() = 0;
    void Stop() = 0;
    void Shutdown() = 0;
};

bool SciterUIInit(const char * languageDir, const char * baseLanguage, const char * currentLanguage, bool Console, ISciterUI *& sciterUI);
