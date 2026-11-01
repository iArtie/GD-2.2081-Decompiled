#include "SetupArtSwitchPopup.h"
#include "SetupTriggerPopup.h"
#include "SelectArtLayer.h"
#include "ArtTriggerGameObject.h"
#include "GameToolbox.h"

SetupArtSwitchPopup* SetupArtSwitchPopup::create(ArtTriggerGameObject* object, cocos2d::CCArray* objects, int id)
{
    SetupArtSwitchPopup* instance = new SetupArtSwitchPopup();
    if (instance->init(object, objects, id))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupArtSwitchPopup::onArt(cocos2d::CCObject* sender)
{
    float value = getValue(533);
    SelectArtLayer* selectArtLayer = SelectArtLayer::create((SelectArtType)m_artType, (int)value);
    selectArtLayer->m_delegate = this;
    selectArtLayer->show();
}

void SetupArtSwitchPopup::selectArtClosed(SelectArtLayer* layer)
{
    int art = layer->m_art;
    updateValue(533, (float)art);

    std::string frameName = artIconForKey(art, m_artType);
    m_artSprite->setDisplayFrame(
        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str()));
}

SetupArtSwitchPopup::~SetupArtSwitchPopup()
{
}