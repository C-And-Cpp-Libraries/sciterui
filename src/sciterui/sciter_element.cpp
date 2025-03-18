#include "std_string.h"
#include <sciter_element.h>
#include <sciter_value_internal.h>

#include <sciter-x.h>

#undef HWINDOW

struct FindFirstCallback : sciter::dom::callback
{
    HELEMENT hfound;
    FindFirstCallback() :
        hfound(0)
    {
    }
    inline bool on_element(HELEMENT hfe)
    {
        hfound = hfe;
        return true; /*stop enumeration*/
    }
};

static SBOOL SC_CALLBACK callback_func(HELEMENT he, LPVOID param)
{
    sciter::dom::callback * pcall = (sciter::dom::callback *)param;
    return (SBOOL)pcall->on_element(he); // SBOOL(true) - stop enumeration
}

SciterElement::SciterElement() :
    m_he(0)
{
}

SciterElement::SciterElement(SCITER_ELEMENT h)
{
    Use(h);
}

SciterElement::SciterElement(const SciterElement & e)
{
    Use(e.m_he);
}

SciterElement::~SciterElement()
{
    Unuse();
}

SciterElement & SciterElement::operator=(SCITER_ELEMENT h)
{
    Set(h);
    return *this;
}

SciterElement & SciterElement::operator=(const SciterElement & e)
{
    Set(e.m_he);
    return *this;
}

SciterElement::operator bool() const
{
    return m_he != 0;
}

SciterElement::operator SCITER_ELEMENT() const
{
    return m_he;
}

bool SciterElement::operator==(const SciterElement & rs) const
{
    return m_he == rs.m_he;
}

bool SciterElement::operator==(SCITER_ELEMENT rs) const
{
    return m_he == rs;
}

bool SciterElement::operator!=(const SciterElement & rs) const
{
    return m_he != rs.m_he;
}

bool SciterElement::operator!=(SCITER_ELEMENT rs) const
{
    return m_he != rs;
}

bool SciterElement::IsValid() const
{
    return m_he != 0;
}

SciterElement SciterElement::FindFirst(const char * selector, ...) const
{
    SciterUI::stdstr buffer;
    va_list args;
    va_start(args, selector);
    buffer.ArgFormat(selector, args);
    va_end(args);
        
    FindFirstCallback FindFirst;
    SciterSelectElements((HELEMENT)m_he, buffer.c_str(), callback_func, &FindFirst);
    return FindFirst.hfound;
}

void SciterElement::SetHTML(const uint8_t * html, size_t htmlLength, int where) const
{
    if (html == nullptr || htmlLength == 0)
    {
        SCDOM_RESULT r = SciterSetElementText((HELEMENT)m_he, 0, 0);
        assert(r == SCDOM_OK);
        (void)r;
    }
    else
    {
        SCDOM_RESULT r = SciterSetElementHtml((HELEMENT)m_he, html, UINT(htmlLength), where);
        assert(r == SCDOM_OK);
        (void)r;
    }
}

SciterValue SciterElement::Eval(const char * script)
{
    std::wstring wScript(SciterUI::stdstr(script).ToUTF16());

    SCITER_VALUE rv;
    SCDOM_RESULT r = SciterEvalElementScript((HELEMENT)m_he, wScript.c_str(), (UINT)wScript.length(), &rv);
    assert(r == SCDOM_OK);
    (void)r;
    return ConvertToSciterValue(rv);
}

void SciterElement::Use(SCITER_ELEMENT h)
{
    m_he = (Sciter_UseElement((HELEMENT)h) == SCDOM_OK) ? h : 0;
}

void SciterElement::Unuse()
{
    if (m_he != nullptr)
    {
        Sciter_UnuseElement((HELEMENT)m_he);
    }
    m_he = 0;
}

void SciterElement::Set(SCITER_ELEMENT h)
{
    Unuse();
    Use(h);
}
