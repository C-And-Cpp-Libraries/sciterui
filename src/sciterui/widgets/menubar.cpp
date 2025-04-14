#include "std_string.h"
#include <memory>
#include <sciter_element.h>
#include <sciter_handler.h>
#include <sciter_ui.h>
#include <set>
#include <widgets/menubar.h>

WidgetMenuBar::MenuBars WidgetMenuBar::m_instances;

typedef std::set<IMenuBarSink *> IMenuBarSinkSet;

MenuBarItem::MenuBarItem(int32_t id, const char * title, MenuBarItemList * subMenu)
{
    Reset(id, title, subMenu);
}

void MenuBarItem::Reset(int32_t id, const char * title, MenuBarItemList * subMenu)
{
    m_id = id;
    m_title = title;
    m_subMenu = subMenu;
}

int MenuBarItem::ID() const
{
    return m_id;
}

const char * MenuBarItem::Title() const
{
    return m_title.c_str();
}

const MenuBarItemList * MenuBarItem::SubMenu() const
{
    return m_subMenu;
}

struct WidgetMenuBar::Impl
{
    explicit Impl(WidgetMenuBar & menuBar, ISciterUI & sciterUI) :
        m_menuBar(menuBar),
        m_sciterUI(sciterUI),
        m_baseElement(nullptr)
    {
    }

    WidgetMenuBar & m_menuBar;
    ISciterUI & m_sciterUI;
    IBaseElement * m_baseElement;
    SciterElement m_menuBarElem;
    IMenuBarSinkSet m_sinks;
};

void WidgetMenuBar::Register(ISciterUI & sciterUI)
{
    const char * WidgetCss =
        "mainmenu {"
        "    display-model: block-inside;"
        "    display: block;"
        "    behavior: MainMenu;"
        "}";
    sciterUI.RegisterWidgetType("MainMenu", WidgetMenuBar::CreateWidget, WidgetCss);
}

void WidgetMenuBar::SetMenuContent(MenuBarItemList & items) const
{
    std::string html("<ul id=\"menu-bar\">\n");
    for (MenuBarItemList::iterator itr = items.begin(); itr != items.end(); itr++)
    {
        html += MenuItemHtml(*itr, 2);
    }
    html += "</ul>";
    impl->m_menuBarElem.SetHTML((const uint8_t *)html.data(), html.size(), SciterElement::SIH_REPLACE_CONTENT);
    impl->m_sciterUI.AttachHandler(impl->m_menuBarElem, IID_ICLICKSINK, (IClickSink *)this);
}

void WidgetMenuBar::AddSink(IMenuBarSink * sink)
{
    impl->m_sinks.insert(sink);
}

void WidgetMenuBar::RemoveSink(IMenuBarSink * sink)
{
    IMenuBarSinkSet::iterator itr = impl->m_sinks.find(sink);
    if (itr != impl->m_sinks.end())
    {
        impl->m_sinks.erase(itr);
    }
}

void WidgetMenuBar::Attached(SCITER_ELEMENT element, IBaseElement * baseElement)
{
    impl->m_baseElement = baseElement;
    impl->m_menuBarElem = element;
}

void WidgetMenuBar::Detached(SCITER_ELEMENT /*element*/)
{
    __debugbreak();
}

bool WidgetMenuBar::GetInterface(const char * riid, void ** object)
{
    if (strcmp(riid, IID_IMENUBAR) == 0)
    {
        *object = std::static_pointer_cast<IMenuBar>(shared_from_this()).get();
        return true;
    }
    return false;
}

bool WidgetMenuBar::OnSciterElement(SCITER_ELEMENT he)
{
    std::string menuIdvalue = SciterElement(he).GetAttribute("data-menu_id");
    if (!menuIdvalue.empty())
    {
        impl->m_sciterUI.AttachHandler(he, IID_ICLICKSINK, (IClickSink *)this);
    }
    return false;
}

bool WidgetMenuBar::OnClick(SCITER_ELEMENT element, SCITER_ELEMENT source, uint32_t /*reason*/)
{
    SciterElement item(source);
    for (uint32_t i = 0; i < 5; i++)
    {
        std::string menuIdvalue = item.GetAttribute("data-menu_id");
        if (!menuIdvalue.empty() || element == item)
        {
            break;
        }
        item = item.GetParent();
    }
    std::string menuIdvalue = item.GetAttribute("data-menu_id");
    if (!menuIdvalue.empty())
    {
        SciterElement menu = SciterElement(element).FindFirst("menu[window-state=\"shown\"]");
        menu.HidePopup();

        for (IMenuBarSinkSet::iterator itr = impl->m_sinks.begin(); itr != impl->m_sinks.end(); itr++)
        {
            (*itr)->OnMenuItem(std::stoi(menuIdvalue), item);
        }
    }
    return false;
}

std::string WidgetMenuBar::MenuItemHtml(const MenuBarItem & item, uint32_t indent)
{
    if (item.ID() == MenuBarItem::SPLITER)
    {
        return SciterUI::stdstr_f("%*s<hr />\n", indent, "");
    }
    SciterUI::stdstr title(item.Title());
    title.Replace("...", "&hellip;");
    std::string submenu;
    if (item.SubMenu() != nullptr && item.SubMenu()->size() > 0)
    {
        for (MenuBarItemList::const_iterator itr = item.SubMenu()->begin(); itr != item.SubMenu()->end(); itr++)
        {
            submenu += MenuItemHtml(*itr, indent + 4);
        }
    }
    if (!submenu.empty())
    {
        submenu = SciterUI::stdstr_f("\n%*s<menu>\n%s%*s</menu>\n%*s", indent + 2, "", submenu.c_str(), indent + 2, "", indent, "");
        title = SciterUI::stdstr_f("<li>\n%*s<caption>%s</caption>", indent + 2, "", title.c_str());
    }
    else
    {
        title = SciterUI::stdstr_f("<li data-menu_id=\"%d\">%s", item.ID(), title.c_str());
    }
    return SciterUI::stdstr_f("%*s%s%s</li>\n", indent, "", title.c_str(), submenu.c_str());
}

IWidget * WidgetMenuBar::CreateWidget(ISciterUI & sciterUI)
{
    std::shared_ptr<WidgetMenuBar> instance(new WidgetMenuBar(sciterUI));
    IWidget * widget = (IWidget *)instance.get();
    WidgetMenuBar * menubar = instance.get();
    m_instances.insert(MenuBars::value_type(menubar, std::move(instance)));
    return widget;
}

WidgetMenuBar::WidgetMenuBar(ISciterUI & sciterUI) :
    impl{std::make_unique<Impl>(*this, sciterUI)}
{
}
