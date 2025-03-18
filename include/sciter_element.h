#pragma once
#include "sciter_ui.h"
#include "sciter_value.h"

class SciterElement
{
public:
    enum SET_ELEMENT_HTML
    {
        SIH_REPLACE_CONTENT = 0,
        SIH_INSERT_AT_START = 1,
        SIH_APPEND_AFTER_LAST = 2,
        SOH_REPLACE = 3,
        SOH_INSERT_BEFORE = 4,
        SOH_INSERT_AFTER = 5
    };

    SciterElement();
    SciterElement(SCITER_ELEMENT h);
    SciterElement(const SciterElement & e);
    ~SciterElement();

    SciterElement & operator=(SCITER_ELEMENT h);
    SciterElement & operator=(const SciterElement & e);

    operator bool() const;
    operator SCITER_ELEMENT() const;
    bool operator==(const SciterElement & rs) const;
    bool operator==(SCITER_ELEMENT rs) const;
    bool operator!=(const SciterElement & rs) const;
    bool operator!=(SCITER_ELEMENT rs) const;

    bool IsValid() const;

    SciterElement FindFirst(const char * selector, ...) const;
    void SetHTML(const uint8_t * hHtml, size_t htmlLength, int where = SIH_REPLACE_CONTENT) const;
    SciterValue Eval(const char * script);

private:
    void Use(SCITER_ELEMENT h);
    void Unuse();
    void Set(SCITER_ELEMENT h);

    SCITER_ELEMENT m_he;
};
