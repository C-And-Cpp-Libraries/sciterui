#pragma once
#include <sciter_ui.h>
#include <stdint.h>

#define SCITER_CALLBACK __stdcall

namespace SciterUI
{

class Sciter;

class EventHandler
{
public:
    EventHandler(Sciter & sciter, SCITER_ELEMENT element, void * interfacePtr, uint32_t subscription);

    static int SCITER_CALLBACK ClickHandler(void * tag, SCITER_ELEMENT he, uint32_t evtg, void * prms);
    static int SCITER_CALLBACK KeyHandler(void * tag, SCITER_ELEMENT he, uint32_t evtg, void * prms);
    static int SCITER_CALLBACK ResizeHandler(void * tag, SCITER_ELEMENT he, uint32_t evtg, void * prms);
    static int SCITER_CALLBACK ForwardBehaviorHandler(void* tag, SCITER_ELEMENT he, uint32_t evtg, void* prms);
    static int SCITER_CALLBACK StateChangeHandler(void* tag, SCITER_ELEMENT he, uint32_t evtg, void* prms);

private:
    EventHandler() = delete;
    EventHandler(const EventHandler &) = delete;
    EventHandler & operator=(const EventHandler &) = delete;

    Sciter & m_Sciter;
    SCITER_ELEMENT m_Element;
    void * m_Interface;
    uint32_t m_Subscription;
    bool m_MouseDown;
    bool m_InElement;
};

} // namespace SciterUI
