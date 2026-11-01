#include "SetupAudioTriggerPopup.h"
#include "SetupTriggerPopup.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"

void SetupAudioTriggerPopup::addProxVolControls(int page)
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);
    cocos2d::CCPoint rowBase(winSize.width * 0.5f, winSize.height * 0.5f + 42.0f);

    cocos2d::CCPoint pos1 = center + cocos2d::CCPoint(-80.0f, 80.0f);
    createValueControlAdvanced(
        51, "Group ID 1:", cocos2d::CCPoint(0.65f, 0.0f), 9999.0f, &pos1,
        Float, 2, 10, 0.0f, 0.0f, 1, page, GoldLabel, 0, 0);

    cocos2d::CCPoint pos2 = center + cocos2d::CCPoint(80.0f, 80.0f);
    createValueControlAdvanced(
        71, "Group ID 2:", cocos2d::CCPoint(0.65f, 0.0f), 9999.0f, &pos2,
        Float, 3, 10, 0.0f, 0.0f, 1, page, (GJInputStyle)9, 0, 0);

    cocos2d::CCPoint pos3 = rowBase + cocos2d::CCPoint(-91.0f, 0.0f);
    createValueControlAdvanced(
        421, "VolNear:", cocos2d::CCPoint(0.65f, 0.0f), 1.0f, &pos3,
        (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, page, GoldLabel, 1, 2);

    cocos2d::CCPoint pos4 = rowBase + cocos2d::CCPoint(91.0f, 0.0f);
    createValueControlAdvanced(
        424, "MinDist:", cocos2d::CCPoint(0.65f, 0.0f), 100.0f, &pos4,
        (InputValueType)0, 2, 6, 0.0f, 0.0f, 0, page, GoldLabel, 1, 0);

    cocos2d::CCPoint pos5 = rowBase + cocos2d::CCPoint(-91.0f, -30.0f);
    createValueControlAdvanced(
        422, "VolMed:", cocos2d::CCPoint(0.65f, 0.0f), 1.0f, &pos5,
        (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, page, GoldLabel, 1, 2);

    cocos2d::CCPoint pos6 = rowBase + cocos2d::CCPoint(91.0f, -30.0f);
    createValueControlAdvanced(
        425, "Dist2:", cocos2d::CCPoint(0.65f, 0.0f), 100.0f, &pos6,
        (InputValueType)0, 2, 6, 0.0f, 0.0f, 0, page, GoldLabel, 1, 0);

    cocos2d::CCPoint pos7 = rowBase + cocos2d::CCPoint(-91.0f, -60.0f);
    createValueControlAdvanced(
        423, "VolFar:", cocos2d::CCPoint(0.65f, 0.0f), 1.0f, &pos7,
        (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, page, GoldLabel, 1, 2);

    cocos2d::CCPoint pos8 = rowBase + cocos2d::CCPoint(91.0f, -60.0f);
    createValueControlAdvanced(
        426, "Dist3:", cocos2d::CCPoint(0.65f, 0.0f), 100.0f, &pos8,
        (InputValueType)0, 2, 6, 0.0f, 0.0f, 0, page, GoldLabel, 1, 0);

    cocos2d::CCPoint pos9 = rowBase + cocos2d::CCPoint(-170.0f, -95.0f);
    createToggleValueControl(138, "P1", cocos2d::CCPoint(1.0f, 0.0f), &pos9, 0, page, 0.0f);

    cocos2d::CCPoint pos10 = rowBase + cocos2d::CCPoint(-110.0f, -95.0f);
    createToggleValueControl(200, "P2", cocos2d::CCPoint(1.0f, 0.0f), &pos10, 0, page, 0.0f);

    cocos2d::CCPoint pos11 = rowBase + cocos2d::CCPoint(-50.0f, -95.0f);
    createToggleValueControl(428, "Cam", cocos2d::CCPoint(1.0f, 0.0f), &pos11, 0, page, 0.0f);

    m_proximityButtons = cocos2d::CCArray::create();
    m_proximityButtons->retain();

    int currentProxMode = (int)getValue(458);
    cocos2d::CCNode* pageContainer = getPageContainer(page);

    for (int i = 0; i < 7; i++)
    {
        const char* frameName;
        float rotation = 0.0f;

        switch (i)
        {
            case 1:
                frameName = "edit_areaModeBtn01_001.png";
                break;
            case 2:
                frameName = "edit_areaModeBtn03_001.png";
                rotation = 180.0f;
                break;
            case 3:
                frameName = "edit_areaModeBtn03_001.png";
                break;
            case 4:
                frameName = "edit_areaModeBtn01_001.png";
                rotation = 90.0f;
                break;
            case 5:
                frameName = "edit_areaModeBtn03_001.png";
                rotation = 90.0f;
                break;
            case 6:
                frameName = "edit_areaModeBtn03_001.png";
                rotation = -90.0f;
                break;
            default:
                frameName = "edit_areaModeBtn04_001.png";
                break;
        }

        cocos2d::CCSprite* iconSprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName);
        iconSprite->setRotation(rotation);

        const char* bgImage = (currentProxMode == i) ? "GJ_button_02.png" : "GJ_button_04.png";
        ButtonSprite* buttonSprite = ButtonSprite::create(iconSprite, 0, 0, 1.0f, bgImage);

        CCMenuItemSpriteExtra* menuItem = CCMenuItemSpriteExtra::create(
            buttonSprite, nullptr, menu_selector(SetupAudioTriggerPopup::onProxMode));
        menuItem->setTag(i);

        pageContainer->addObject(menuItem);
        m_proximityButtons->addObject(menuItem);

        buttonSprite->setScale(1.8f);
        buttonSprite->setPosition(buttonSprite->getContentSize().width * 0.5f,
                                  buttonSprite->getContentSize().height * 0.5f);

        menuItem->setScale(0.6f);
        menuItem->m_baseScale = 0.6f;
        m_buttonMenu->addChild(menuItem);

        float yOffset = 0.0f;
        if (i > 3)
            yOffset = -30.0f;

        cocos2d::CCPoint menuPos(
            (winSize.width * 0.5f + 60.0f) + (i & 3) * 30.0f,
            winSize.height * 0.5f - 54.0f + yOffset);
        cocos2d::CCPoint localPos = m_buttonMenu->convertToNodeSpace(menuPos);
        menuItem->setPosition(localPos);
    }

    bool camEnabled = getValue(428) != 0.0f;
    toggleGroup(428, camEnabled);
    updateSpecialGroup();
}

void SetupAudioTriggerPopup::onProxMode(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    updateValue(458, (float)tag);

    for (unsigned int i = 0; i < m_proximityButtons->count(); i++)
    {
        CCMenuItemSpriteExtra* item = (CCMenuItemSpriteExtra*)m_proximityButtons->objectAtIndex(i);
        ButtonSprite* buttonSprite = (ButtonSprite*)item->getNormalImage();
        bool selected = (tag == item->getTag());
        buttonSprite->updateBGImage(selected ? "GJ_button_02.png" : "GJ_button_04.png");
    }
}

void SetupAudioTriggerPopup::updateSpecialGroup()
{
    if (getValue(138) == 0.0f && getValue(200) == 0.0f)
    {
        bool camOff = getValue(428) == 0.0f;
        toggleGroup(9, camOff);
    }
    else
    {
        toggleGroup(9, false);
    }
}

void SetupAudioTriggerPopup::valueDidChange(int tag, float value)
{
    if (tag == 200 || tag == 138)
    {
        if (value != 0.0f)
        {
            updateValue(428, 0.0f);
            updateSpecialGroup();
            return;
        }
    }
    else
    {
        if (tag != 428)
            return;

        bool camOn = value != 0.0f;
        if (camOn)
        {
            updateValue(138, 0.0f);
            updateValue(200, 0.0f);
        }
    }
    updateSpecialGroup();
}

SetupAudioTriggerPopup::~SetupAudioTriggerPopup()
{
}