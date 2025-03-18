#include "sciter.h"
#include <sciter_ui.h>
#include <memory>

std::unique_ptr<SciterUI::Sciter> g_sciter;

bool SciterUIInit(const char * LanguageDir, const char * BaseLanguage, const char * CurrentLanguage, bool Console, ISciterUI *& sciterUI)
{
    if (g_sciter.get() != nullptr)
    {
        return false;
    }
    g_sciter.reset(new SciterUI::Sciter(LanguageDir));
    if (!g_sciter->Initialize(BaseLanguage, CurrentLanguage, Console))
    {
        g_sciter.reset(nullptr);
        return false;
    }
    sciterUI = g_sciter.get();
    return true;
}
