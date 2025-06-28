#pragma once
#include <sciter_ui.h>
#include <string>

__interface IPagesSink
{
    bool PageNavChangeFrom(const std::string & PageName, SCITER_ELEMENT PageNav);
    bool PageNavChangeTo(const std::string & PageName, SCITER_ELEMENT PageNav);
    void PageNavCreatedPage(const std::string & PageName, SCITER_ELEMENT Page);
    void PageNavPageChanged(const std::string & PageName, SCITER_ELEMENT PageNav);
};

static const char * IID_IPAGENAV = "A1FD4FA4-6BEE-4166-AD9D-D7BF867B0B3E";

__interface IPageNav
{
    std::string GetCurrentPage();
    void AddSink(IPagesSink * Sink);
    void RemoveSink(IPagesSink * Sink);
};

bool Register_WidgetPageNav(ISciterUI& SciterUI);