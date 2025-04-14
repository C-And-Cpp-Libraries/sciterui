#include "base_element.h"

namespace SciterUI
{

BaseElement::BaseElement(Sciter & sciter, SCITER_ELEMENT element) :
    m_sciter(sciter),
    m_element(element)
{
}

void BaseElement::AddWidget(IWidget * widget)
{
    m_Widgets.push_back(widget);
}

bool BaseElement::RemoveWidget(IWidget * /*widget*/)
{
    return false;
}

bool BaseElement::GetInterface(const char * riid, void ** interfacePtr)
{
    for (size_t i = 0; i < m_Widgets.size(); i++)
    {
        if (m_Widgets[i]->GetInterface(riid, interfacePtr))
        {
            return true;
        }
    }
    return false;
}

} // namespace SciterUI