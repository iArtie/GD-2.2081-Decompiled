#include "Models/DialogObject.h"

DialogObject* DialogObject::create(std::string character, std::string text, int characterFrame, float textScale, bool unskippable, cocos2d::ccColor3B nameColor)
{
    DialogObject* pRet = new DialogObject();
    if (pRet && pRet->init(character, text, characterFrame, textScale, unskippable, nameColor))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool DialogObject::init(std::string character, std::string text, int characterFrame, float textScale, bool unskippable, cocos2d::ccColor3B nameColor)
{
    if (!cocos2d::CCObject::init())
        return false;
    
    m_character = character;
    m_text = text;
    m_characterFrame = characterFrame;
    m_textScale = textScale;
    m_unskippable = unskippable;
    m_nameColor = nameColor;
    
    return true;
}

DialogObject::~DialogObject()
{
    // Base class handles cleanup
}