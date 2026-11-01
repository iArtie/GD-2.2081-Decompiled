#include "SetupAudioLineGuidePopup.h"
#include "SetupTriggerPopup.h"
#include "SelectSettingLayer.h"
#include "AudioLineGuideGameObject.h"

SetupAudioLineGuidePopup* SetupAudioLineGuidePopup::create(AudioLineGuideGameObject* object, cocos2d::CCArray* objects)
{
    SetupAudioLineGuidePopup* instance = new SetupAudioLineGuidePopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupAudioLineGuidePopup::onSpeed(cocos2d::CCObject* sender)
{
    float value = getValue(499);
    SelectSettingLayer* selectSettingLayer = SelectSettingLayer::create(StartingSpeed, (int)value);
    selectSettingLayer->m_delegate = this;
    selectSettingLayer->show();
}

void SetupAudioLineGuidePopup::selectSettingClosed(SelectSettingLayer* layer)
{
    int selectedValue = layer->getSelectedValue();

    std::string frameName = layer->getSelectedFrame();
    m_speedSprite->setDisplayFrame(
        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str()));

    updateValue(499, (float)selectedValue);
}

SetupAudioLineGuidePopup::~SetupAudioLineGuidePopup()
{
}