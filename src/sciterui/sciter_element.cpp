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

void SciterElement::Clear()
{
    SCDOM_RESULT r = SciterSetElementText((HELEMENT)m_he, 0, 0);
    assert(r == SCDOM_OK); (void)r;
}

bool SciterElement::Create(const char * tagName, const char* text)
{
    SciterElement e;
    SCDOM_RESULT r = SciterCreateElement(tagName, SciterUI::stdstr(text).ToUTF16().c_str(), (HELEMENT*)&e.m_he); // don't need 'use' here, as it is already "addrefed"
    if (r != SCDOM_OK)
    {
        assert(r == SCDOM_OK);
        return false;
    }
    *this = e;
    return r == SCDOM_OK;
}

void SciterElement::Detach() const
{
    SCDOM_RESULT r = SciterDetachElement((HELEMENT)m_he);
    assert(r == SCDOM_OK);
    (void)r;
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

std::string SciterElement::GetAttribute(const char * name) const
{
    sciter::string s;
    SCDOM_RESULT r = SciterGetAttributeByNameCB((HELEMENT)m_he, name, &_LPCWSTR2STRING, &s);
    if (r == SCDOM_OK_NOT_HANDLED)
    {
        return "";
    }
    return SciterUI::stdstr().FromUTF16(s.c_str());
}

void SciterElement::SetAttribute(const char * name, const char * value) const
{
    SciterSetAttributeByName((HELEMENT)m_he, name, SciterUI::stdstr(value).ToUTF16().c_str());
}

HWINDOW SciterElement::GetElementHwnd(bool RootWindow) const
{
    HWINDOW hwnd = 0;
    SCDOM_RESULT r = SciterGetElementHwnd((HELEMENT)m_he, (HWND *)&hwnd, RootWindow);
    assert(r == SCDOM_OK);
    (void)r;
    return hwnd;
}

std::string SciterElement::GetAttributeByName(const char* Name) const
{
    sciter::string s;
    SCDOM_RESULT r = SciterGetAttributeByNameCB((HELEMENT)m_he, Name, &_LPCWSTR2STRING, &s);
    if (r == SCDOM_OK_NOT_HANDLED)
    {
        return "";
    }
    return SciterUI::stdstr().FromUTF16(s.c_str());
}

SCITER_ELEMENT SciterElement::GetChild(uint32_t Index) const
{
    HELEMENT Child = 0;
    SciterGetNthChild((HELEMENT)m_he, Index, &Child);
    return (SCITER_ELEMENT)Child;
}

uint32_t SciterElement::GetChildCount(void) const
{
    uint32_t Count = 0;
    SciterGetChildrenCount((HELEMENT)m_he, &Count);
    return Count;
}

SciterElement SciterElement::GetElementByID(const char * id) const
{
    if (id == nullptr)
    {
        return SciterElement();
    }
    return FindFirst("[id='%s']", id);
}

std::string SciterElement::GetHTML(bool OuterHtml) const
{
    sciter::astring s;
    SCDOM_RESULT r = SciterGetElementHtmlCB((HELEMENT)m_he, SBOOL(OuterHtml), &_LPCBYTE2ASTRING, &s);
    assert(r == SCDOM_OK);
    (void)r;
    return s;
}

SciterElement SciterElement::GetParent() const
{
    HELEMENT hParent = 0;
    SciterGetParentElement((HELEMENT)m_he, &hParent);
    return SciterElement(hParent);
}

SciterElement SciterElement::GetRoot() const
{
    SciterElement ParentElement(GetParent());
    if (ParentElement.IsValid())
    {
        return ParentElement.GetRoot();
    }
    return SciterElement(m_he);
}

uint32_t SciterElement::GetState() const
{
    UINT state = 0;
    SCDOM_RESULT r = SciterGetElementState((HELEMENT)m_he, &state);
    assert(r == SCDOM_OK);
    (void)r;
    return state; /*ELEMENT_STATE_BITS*/
}

SciterValue SciterElement::GetValue() const
{
    SCITER_VALUE rv;
    SCDOM_RESULT r = SciterGetValue((HELEMENT)m_he, &rv);
    assert(r == SCDOM_OK); (void)r;
    return ConvertToSciterValue(rv);
}

void SciterElement::HidePopup() const
{
    UINT State = 0;
    SCDOM_RESULT r = SciterGetElementState((HELEMENT)m_he, &State);
    if (r == SCDOM_OK && ((State & STATE_POPUP) == STATE_POPUP))
    {
        SciterHidePopup((HELEMENT)m_he);
    }
}

void SciterElement::Insert(const SCITER_ELEMENT& e, uint32_t Index)
{
    SCDOM_RESULT r = SciterInsertElement((HELEMENT)e, (HELEMENT)m_he, Index);
    assert(r == SCDOM_OK);
    (void)r;
}

bool SciterElement::ReleaseCapture(void) const
{
    return SciterReleaseCapture((HELEMENT)m_he) == SCDOM_OK;
}

void SciterElement::SelectElements(ISciterElementCallback * pcall, const char * selectors) const
{
    SciterSelectElements((HELEMENT)m_he, selectors, callback_func, pcall);
}

bool SciterElement::SetCapture(void) const
{
    return SciterSetCapture((HELEMENT)m_he) == SCDOM_OK;
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

SciterElement::RECT SciterElement::GetLocation(uint32_t Area) const
{
    SciterElement::RECT rc = {0, 0, 0, 0};
    SciterGetElementLocation((HELEMENT)m_he, (::RECT *)&rc, Area);
    return rc;
}

void SciterElement::SetState(uint32_t BitsToSet, uint32_t BitsToClear, bool Update) const
{
    SCDOM_RESULT r = SciterSetElementState((HELEMENT)m_he, BitsToSet, BitsToClear, SBOOL(Update));
    assert(r == SCDOM_OK);
    (void)r;
}

void SciterElement::SetStyleAttribute(const char* Name, const wchar_t* Value) const
{
    SCDOM_RESULT r = SciterSetStyleAttribute((HELEMENT)m_he, Name, Value);
    assert(r == SCDOM_OK);
    (void)r;
}

void SciterElement::SetValue(SciterValue value) const
{
    SCITER_VALUE rv = ConvertFromSciterValue(value);
    SCDOM_RESULT r = SciterSetValue((HELEMENT)m_he, &rv);
    assert(r == SCDOM_OK); (void)r;
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
