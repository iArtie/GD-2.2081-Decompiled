#include "UI/SearchButton.h"

SearchButton* SearchButton::create(const char* background, const char* label, float scale, const char* icon)
{
    SearchButton* returnVal = new SearchButton();

    if (returnVal->init(background, label, scale, icon))
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

bool SearchButton::init(const char* background, const char* label, float scale, const char* icon)
{
    if (!cocos2d::CCSprite::initWithSpriteFrameName(background))
        return false;

    cocos2d::CCSize contentSize = getContentSize();
    cocos2d::CCPoint anchor(-1.0f, 1.0f);

    m_label = cocos2d::CCLabelBMFont::create(label, "bigFont.fnt");
    addChild(m_label, 1);
    m_label->setPosition(cocos2d::CCPoint(
        anchor.x + contentSize.width * 0.5f,
        anchor.y + contentSize.height * 0.5f));
    m_label->setScale(scale);

    if (icon && *icon)
    {
        m_icon = cocos2d::CCSprite::createWithSpriteFrameName(icon);
        addChild(m_icon, 1);
        m_icon->setScale(1.1f);

        float iconWidth = m_icon->getContentSize().width * m_icon->getScale();
        float labelWidth = m_label->getContentSize().width * m_label->getScale();
        float spacing = labelWidth + 5.0f;
        float center = (spacing + iconWidth * 0.5f + 10.0f) * 0.5f;

        m_label->setPosition(cocos2d::CCPoint(
            spacing * 0.5f - (center - contentSize.width * 0.5f) + anchor.x,
            anchor.y + contentSize.height * 0.5f));

        m_icon->setPosition(cocos2d::CCPoint(
            center + contentSize.width * 0.5f - iconWidth * 0.5f,
            contentSize.height * 0.5f));
    }

    return true;
}

SearchButton::~SearchButton()
{
}