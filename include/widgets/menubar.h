#pragma once
#include <list>
#include <map>
#include <memory>
#include <sciter_element.h>
#include <sciter_handler.h>
#include <sciter_ui.h>
#include <stdint.h>

class MenuBarItem;
typedef std::list<MenuBarItem> MenuBarItemList;

class MenuBarItem
{
public:
    enum MenuID
    {
        SPLITER = -1,
        SUB_MENU = -2,
        NO_ID = -3
    };

    MenuBarItem(int32_t id, const char * title = "", MenuBarItemList * subMenu = nullptr);
    void Reset(int32_t id, const char * title = "", MenuBarItemList * subMenu = nullptr);

    int ID() const;
    const char * Title() const;
    const MenuBarItemList * SubMenu() const;

private:
    MenuBarItem(void) = delete;
    MenuBarItem & operator=(const MenuBarItem &) = delete;

    int32_t m_id;
    std::string m_title;
    MenuBarItemList * m_subMenu;
};

__interface IMenuBarSink
{
    void OnMenuItem(int32_t id, SCITER_ELEMENT item);
};

static const char * IID_IMENUBAR = "C890DFA5-2762-4727-A9E7-00F9B972ACE0";
__interface IMenuBar
{
    void SetMenuContent(MenuBarItemList & items) const;
    void AddSink(IMenuBarSink * sink);
    void RemoveSink(IMenuBarSink * sink);
};

void Register_WidgetMenuBar(ISciterUI & sciterUI);