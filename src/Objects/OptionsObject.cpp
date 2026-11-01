#include "Objects/OptionsObject.h"
#include "Objects/OptionsObjectDelegate.h"

OptionsObject* OptionsObject::create(int id, bool enabled, std::string name, OptionsObjectDelegate* delegate)
{
    OptionsObject* returnVal = new OptionsObject();
    if (returnVal->init(id, enabled, name, delegate))
    {
        returnVal->autorelease();
    }
    else
    {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

bool OptionsObject::init(int id, bool enabled, std::string name, OptionsObjectDelegate* delegate)
{
    m_optionID = id;
    m_enabled = enabled;
    m_name = name;
    m_delegate = delegate;
    return true;
}

void OptionsObject::toggleState()
{
    m_enabled = !m_enabled;
    if (m_delegate)
        m_delegate->stateChanged(this);
}

OptionsObject::~OptionsObject()
{
}