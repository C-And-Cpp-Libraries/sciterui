#pragma once
#include <sciter_ui.h>
#include <vector>

namespace SciterUI
{

class Sciter;

class BaseElement :
    public IBaseElement
{
public:
    BaseElement(Sciter & sciter, SCITER_ELEMENT element);

    void AddWidget(IWidget * widget);
    bool RemoveWidget(IWidget * widget);

    //IBaseElement
    bool GetInterface(const char * riid, void ** interfacePtr);

private:
    BaseElement() = delete;
    BaseElement(const BaseElement &) = delete;
    BaseElement & operator=(const BaseElement &) = delete;

    Sciter & m_sciter;
    SCITER_ELEMENT m_element;
    std::vector<IWidget *> m_Widgets;
};

} // namespace SciterUI