#include "Popups/UpdateAccountSettingsPopup.h"
#include "Layers/GJAccountSettingsLayer.h"
#include "UI/LoadingCircle.h"
#include "UI/TextArea.h"

void UpdateAccountSettingsPopup::keyBackClicked()
{
    onClose(nullptr);
}

void UpdateAccountSettingsPopup::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);

    if (m_updateSuccess)
    {
        if (m_settingsLayer)
        {
            m_settingsLayer->updateScoreValues();
            m_settingsLayer->onClose(nullptr);
        }
    }
}

void UpdateAccountSettingsPopup::updateSettingsFailed()
{
    cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName("exMark_001.png");
    sprite->setPosition(m_loadingCircle->m_sprite->getPosition());
    m_mainLayer->addChild(sprite);

    m_loadingCircle->fadeAndRemove();

    m_textArea->setString("Update Failed.");
}

void UpdateAccountSettingsPopup::updateSettingsFinished()
{
    m_updateSuccess = true;

    cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
    sprite->setScale(1.5f);
    sprite->setPosition(m_loadingCircle->m_sprite->getPosition());
    m_mainLayer->addChild(sprite);

    m_loadingCircle->fadeAndRemove();

    m_textArea->setString("Account settings updated!");
}

UpdateAccountSettingsPopup::~UpdateAccountSettingsPopup()
{
}