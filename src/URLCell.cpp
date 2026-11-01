#include "URLCell.h"
#include "CCURLObject.h"
#include "CCMenuItemSpriteExtra.h"

URLCell::URLCell(char* charPtr, float width, float height)
{
    m_width = width;
    m_height = height;
    init();
}

void URLCell::draw()
{
    cocos2d::CCSize size(m_width, m_height);
    cocos2d::ccDrawColor4B(0, 0, 0, 0x4B);
    glLineWidth(1.0f);

    cocos2d::CCPoint point1(1.0f, size.height);
    cocos2d::CCPoint point2(size.width - 1.0f, size.height);
    cocos2d::ccDrawLine(point1, point2);

    cocos2d::CCPoint point3(1.0f, 0.0f);
    cocos2d::CCPoint point4(size.width - 1.0f, 0.0f);
    cocos2d::ccDrawLine(point3, point4);
}

bool URLCell::init()
{
    m_urlObject = nullptr;
    return true;
}

void URLCell::loadFromObject(CCURLObject* object)
{
    m_mainLayer->removeAllChildrenWithCleanup(true);
    m_urlObject = object;

    cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create(m_urlObject->m_url.c_str(), "bigFont.fnt");
    label->limitLabelWidth(250.0f, 0.5f, 0.0f);
    m_mainLayer->addChild(label);
    label->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    label->setPosition(15.0f, m_height * 0.5f);

    cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(menu, 3);

    cocos2d::CCNode* openSprite = ButtonSprite::create("Open", 0, 0, 0.7f, true, "bigFont.fnt", "GJ_button_01.png", 30.0f);
    openSprite->setScale(0.6f);
    CCMenuItemSpriteExtra* openButton = CCMenuItemSpriteExtra::create(openSprite, nullptr, menu_selector(URLCell::onURL));
    openButton->setSizeMult(1.2f);
    menu->addChild(openButton);

    cocos2d::CCPoint openPos(m_width - 30.0f, m_height * 0.5f);
    openButton->setPosition(menu->convertToNodeSpace(openPos));
}

void URLCell::onURL(cocos2d::CCObject* sender)
{
    if (m_urlObject)
    {
        cocos2d::CCApplication::sharedApplication()->openURL(m_urlObject->m_url.c_str());
    }
}

void URLCell::updateBGColor(int index)
{
    if ((index & 1) != 0)
    {
        m_backgroundLayer->setColor(cocos2d::ccc3(194, 114, 62));
    }
    else
    {
        m_backgroundLayer->setColor(cocos2d::ccc3(161, 88, 44));
    }
    m_backgroundLayer->setOpacity(255);
}

URLCell::~URLCell()
{
}