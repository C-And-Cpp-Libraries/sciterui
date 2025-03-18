#pragma once
#include <sciter_ui.h>
#include <set>

namespace SciterUI
{
class Sciter;

#define SC_CALLBACK __stdcall

class SciterWindow :
    public ISciterWindow
{
    /**Notification callback structure.**/
    typedef struct SCITER_CALLBACK_NOTIFICATION
    {
        uint32_t code; /**< [in] one of the codes above.*/
        HWINDOW hwnd;  /**< [in] HWINDOW of the window this callback was attached to.*/
    } SCITER_CALLBACK_NOTIFICATION;

    typedef SCITER_CALLBACK_NOTIFICATION * LPSCITER_CALLBACK_NOTIFICATION;

    typedef struct SCN_LOAD_DATA
    {
        uint32_t code; /**< [in] one of the codes above.*/
        HWINDOW hwnd;  /**< [in] HWINDOW of the window this callback was attached to.*/

        const wchar_t * uri; /**< [in] Zero terminated string, fully qualified uri, for example "http://server/folder/file.ext".*/

        const uint8_t * outData; /**< [in,out] pointer to loaded data to return. if data exists in the cache then this field contain pointer to it*/
        uint32_t outDataSize;    /**< [in,out] loaded data size to return.*/
        uint32_t dataType;       /**< [in] SciterResourceType */

        void * requestId; /**< [in] request handle that can be used with sciter-x-request API */

        SCITER_ELEMENT principal;
        SCITER_ELEMENT initiator;
    } SCN_LOAD_DATA;

    typedef SCN_LOAD_DATA * LPSCN_LOAD_DATA;
    typedef std::set<IWindowDestroySink *> WinDestroySinks;

public:
    SciterWindow(Sciter & sciter);
    ~SciterWindow();

    bool Create(HWINDOW parentWinow, const char * htmlFile, int x, int y, int width, int height);
    bool GetDestroyed(void) const;
    void SetDestroyed(void);

    //ISciterWindow
    void CenterWindow(void);
    HWINDOW GetHandle() const;
    SCITER_ELEMENT GetRootElement(void) const;
    void OnDestroySinkAdd(IWindowDestroySink * Sink);
    void OnDestroySinkRemove(IWindowDestroySink * Sink);

private:
    SciterWindow(void) = delete;
    SciterWindow(const SciterWindow &) = delete;
    SciterWindow & operator=(const SciterWindow &) = delete;

    void Bind();
    bool LoadHtml(const char * url);
    int64_t HandleNotification(LPSCITER_CALLBACK_NOTIFICATION pnm);
    int64_t OnLoadData(LPSCN_LOAD_DATA pnmld);
    int64_t OnEngineDestroyed(void);

    static uint32_t SC_CALLBACK SciterCallback(LPSCITER_CALLBACK_NOTIFICATION pnm, void * param);

    Sciter & m_sciter;
    HWINDOW m_hWnd;
    WinDestroySinks m_onDestroySink;
    bool m_bound;
    bool m_destroyed;
};

} // namespace SciterUI
