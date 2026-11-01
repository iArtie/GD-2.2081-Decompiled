#include "Layers/URLViewLayer.h"
#include "UI/CustomListView.h"

URLViewLayer* URLViewLayer::create(std::string title, cocos2d::CCArray* objects)
{
    URLViewLayer* layer = new URLViewLayer();

    if (layer->init(title, objects))
    {
        layer->autorelease();
        return layer;
    }

    delete layer;
    return nullptr;
}

bool URLViewLayer::init(std::string title, cocos2d::CCArray* objects)
{
    if (!GJDropDownLayer::init(title))
        return false;

    if (objects)
    {
        m_urlObjects = objects;
        m_urlObjects->retain();
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    m_listLayer = CustomListView::create(m_urlObjects, BoomListType::Default, 220.0f, 356.0f);
    m_listLayer->setTag(9);
    m_listLayer->addChild(m_listLayer, 6);

    return true;
}

URLViewLayer::~URLViewLayer()
{
}