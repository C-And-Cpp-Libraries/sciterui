#include <widgets/combo_box.h>
#include <map>
#include <sciter_element.h>
#include <vector>

namespace SciterUI
{
    class WidgetComboBox :
        public std::enable_shared_from_this<WidgetComboBox>,
        public IWidget,
        public IComboBox
    {
        typedef std::map<WidgetComboBox*, std::shared_ptr<WidgetComboBox>> ComboBoxes;
        typedef std::vector<std::string> Items;

    public:
        static bool Register(ISciterUI& SciterUI);

    private:
        WidgetComboBox() = delete;
        WidgetComboBox(const WidgetComboBox&) = delete;
        WidgetComboBox& operator=(const WidgetComboBox&) = delete;

        WidgetComboBox(ISciterUI& SciterUI);

        //IWidget
        void Attached(SCITER_ELEMENT element, IBaseElement* baseElement);
        void Detached(SCITER_ELEMENT element);
        std::shared_ptr<void> GetInterface(const char* riid);

        //IComboBox
        int32_t AddItem(const char* item, const char * value) override;
        SCITER_ELEMENT GetSelectedItem() const override;
        bool SelectItem(int32_t index) override;

        static IWidget * __stdcall CreateWidget(ISciterUI & SciterUI);

        static ComboBoxes m_instances;

        ISciterUI & m_sciterUI;
        IBaseElement * m_baseElement;
        SciterElement m_comboBoxElem;
        SciterElement m_select;
        Items m_items;
    };

    WidgetComboBox::ComboBoxes WidgetComboBox::m_instances;

    WidgetComboBox::WidgetComboBox(ISciterUI& sciterUI) :
        m_sciterUI(sciterUI)
    {
    }

    bool WidgetComboBox::Register(ISciterUI& SciterUI)
    {
        const char * WidgetCss =
            "ComboBox {"
            "    display-model: block-inside;"
            "    display: inline-block;"
            "    behavior: ComboBox;"
            "}";

        return SciterUI.RegisterWidgetType("ComboBox", WidgetComboBox::CreateWidget, WidgetCss);
    }

    void WidgetComboBox::Attached(SCITER_ELEMENT element, IBaseElement* baseElement)
    {
        m_baseElement = baseElement;
        m_comboBoxElem = element;

        m_select.Create("select", "");
        m_comboBoxElem.Insert(m_select, m_comboBoxElem.GetChildCount());
    }

    void WidgetComboBox::Detached(SCITER_ELEMENT /*Element*/)
    {
        m_baseElement = nullptr;
        m_comboBoxElem = nullptr;
    }

    std::shared_ptr<void> WidgetComboBox::GetInterface(const char* riid)
    {
        if (strcmp(riid, IID_ICOMBOBOX) == 0)
        {
            return std::static_pointer_cast<IComboBox>(shared_from_this());
        }
        return nullptr;
    }

    int32_t WidgetComboBox::AddItem(const char * item, const char * value)
    {
        int32_t index = -1;
        Items::iterator itr = find(m_items.begin(), m_items.end(), item);
        if (itr != m_items.end())
        {
            return index;
        }

        index = (int32_t)m_items.size();
        m_items.push_back(item);
        SciterElement optionElement;
        optionElement.Create("option", item);
        if (value != nullptr)
        {
            optionElement.SetAttribute("value", value);
        }
        SciterElement popup = m_select.FindFirst("popup");
        if (popup.IsValid())
        {
            popup.Insert(optionElement, popup.GetChildCount());
        }
        else
        {
            m_select.Insert(optionElement, m_select.GetChildCount());
        }
        return index;
    }

    SCITER_ELEMENT WidgetComboBox::GetSelectedItem() const
    {
        if (!m_select)
        {
            return nullptr;
        }
        SciterElement list = m_select.FindFirst("popup");
        if (!list)
        {
            list = m_select;
        }

        for (uint32_t i = 0, n = list.GetChildCount(); i < n; i++)
        {
            SciterElement option = list.GetChild(i);
            if (option && (option.GetState() & SciterElement::STATE_CURRENT) != 0)
            {
                return option;
            }
        }
        return nullptr;
    }

    bool WidgetComboBox::SelectItem(int32_t index)
    {
        if (!m_select || index < 0 || index >= (int32_t)m_items.size())
        {
            return false;
        }
        SciterElement list = m_select.FindFirst("popup");
        if (!list)
        {
            list = m_select;
        }
        SciterElement option = list.GetChild(index);
        if (!option)
        {
            return false;
        }
        option.SetState(SciterElement::STATE_CURRENT, 0, true);
        SciterElement caption = m_select.FindFirst("caption");
        if (caption)
        {
            caption.SetHTML((const uint8_t *)m_items[index].c_str(), m_items[index].size(), SciterElement::SIH_REPLACE_CONTENT);
        }
        return true;
    }

    IWidget* __stdcall WidgetComboBox::CreateWidget(ISciterUI& sciterUI)
    {
        std::shared_ptr<WidgetComboBox> instance(new WidgetComboBox(sciterUI));
        IWidget * widget = (IWidget*)instance.get();
        WidgetComboBox * combobox = instance.get();
        m_instances.insert(ComboBoxes::value_type(combobox, std::move(instance)));
        return widget;
    }
}

bool Register_WidgetComboBox(ISciterUI & sciterUI)
{
    return SciterUI::WidgetComboBox::Register(sciterUI);
}