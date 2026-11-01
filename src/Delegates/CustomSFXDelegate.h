#include "Delegates/CustomSFXDelegate.h"
#include "Models/SFXInfoObject.h"

int CustomSFXDelegate::getActiveSFXID()
{
    return 0;
}

bool CustomSFXDelegate::overridePlaySFX(SFXInfoObject* object)
{
    return false;
}

void CustomSFXDelegate::sfxObjectSelected(SFXInfoObject* object)
{
    // Empty implementation - meant to be overridden
}