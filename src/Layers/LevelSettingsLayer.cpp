#include "Layers/LevelSettingsLayer.h"
#include "Layers/FLAlertLayer.h"
#include "Models/LevelSettingsObject.h"
#include "Layers/LevelEditorLayer.h"
#include "Delegates/LevelSettingsDelegate.h"
#include "Nodes/SongSelectNode.h"
#include "Layers/SelectArtLayer.h"
#include "Layers/SelectSettingLayer.h"
#include "Objects/ColorAction.h"
#include "Editor/ColorChannelSprite.h"
#include "Cocos/CCTextInputNode.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCMenuItemToggler.h"
#include "Sprites/ButtonSprite.h"
#include "Layers/GJColorSetupLayer.h"
#include "Layers/LevelOptionsLayer.h"
#include "Layers/SelectFontLayer.h"
#include "Popups/ColorSelectPopup.h"
#include "Managers/GJEffectManager.h"
#include "Editor/EditorUI.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"
#include "CCScale9Sprite.h"

void LevelSettingsLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    layer->getTag();
}

LevelSettingsLayer* LevelSettingsLayer::create(LevelSettingsObject* object, LevelEditorLayer* layer)
{
    LevelSettingsLayer* ret = new LevelSettingsLayer();
    if (ret->init(object, layer)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool LevelSettingsLayer::init(LevelSettingsObject* object, LevelEditorLayer* layer)
{
    if (!FLAlertLayer::init(150))
        return false;

    m_editorLayer = layer;
    m_noElasticity = true;
    m_settingsObject = object;
    m_settingsObject->retain();
    m_speed = m_settingsObject->m_startSpeed;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCRect rect = cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f);

    cocos2d::extension::CCScale9Sprite* mainBG = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", rect);
    mainBG->setContentSize(cocos2d::CCSize(455.0f, 305.0f));
    m_mainLayer->addChild(mainBG, -2);
    mainBG->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    float leftX;
    if (object->m_startsWithStartPos) {
        leftX = (winSize.width * 0.5f) - 185.0f;
    } else {
        cocos2d::CCLabelBMFont* colorLabel = cocos2d::CCLabelBMFont::create("Colors", "bigFont.fnt");
        colorLabel->setScale(0.8f);
        m_mainLayer->addChild(colorLabel);
        colorLabel->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f + 135.0f));

        cocos2d::CCLabelBMFont* bgLabel = cocos2d::CCLabelBMFont::create("BG", "bigFont.fnt");
        bgLabel->setScale(0.35f);
        m_mainLayer->addChild(bgLabel);

        cocos2d::CCLabelBMFont* gLabel = cocos2d::CCLabelBMFont::create("G", "bigFont.fnt");
        gLabel->setScale(0.35f);
        m_mainLayer->addChild(gLabel);

        cocos2d::CCLabelBMFont* g2Label = cocos2d::CCLabelBMFont::create("G2", "bigFont.fnt");
        g2Label->setScale(0.35f);
        m_mainLayer->addChild(g2Label);

        cocos2d::CCLabelBMFont* lineLabel = cocos2d::CCLabelBMFont::create("Line", "bigFont.fnt");
        lineLabel->setScale(0.35f);
        m_mainLayer->addChild(lineLabel);

        cocos2d::CCLabelBMFont* mgLabel = cocos2d::CCLabelBMFont::create("MG", "bigFont.fnt");
        mgLabel->setScale(0.35f);
        m_mainLayer->addChild(mgLabel);

        cocos2d::CCLabelBMFont* mg2Label = cocos2d::CCLabelBMFont::create("MG2", "bigFont.fnt");
        mg2Label->setScale(0.35f);
        m_mainLayer->addChild(mg2Label);

        cocos2d::CCLabelBMFont* moreLabel = cocos2d::CCLabelBMFont::create("...", "bigFont.fnt");
        moreLabel->setScale(0.35f);
        m_mainLayer->addChild(moreLabel);

        float labelY = colorLabel->getPosition().y - 20.0f;
        bgLabel->setPosition(ccp(winSize.width * 0.5f - 138.0f, labelY));
        gLabel->setPosition(ccp(winSize.width * 0.5f - 92.0f, labelY));
        g2Label->setPosition(ccp(winSize.width * 0.5f - 46.0f, labelY));
        lineLabel->setPosition(ccp(winSize.width * 0.5f, labelY));
        mgLabel->setPosition(ccp(winSize.width * 0.5f + 46.0f, labelY));
        mg2Label->setPosition(ccp(winSize.width * 0.5f + 92.0f, labelY));
        moreLabel->setPosition(ccp(winSize.width * 0.5f + 138.0f, labelY));

        m_bgColorSprite = ColorChannelSprite::create();
        m_bgColorSprite->setScale(0.8f);
        CCMenuItemSpriteExtra* bgColorItem = CCMenuItemSpriteExtra::create(
            m_bgColorSprite, nullptr, this, menu_selector(LevelSettingsLayer::onShowPicker));
        bgColorItem->setTag(1000);
        m_bgColorSprite->setTag(1000);
        m_buttonMenu->addChild(bgColorItem);

        m_gColorSprite = ColorChannelSprite::create();
        m_gColorSprite->setScale(0.8f);
        CCMenuItemSpriteExtra* gColorItem = CCMenuItemSpriteExtra::create(
            m_gColorSprite, nullptr, this, menu_selector(LevelSettingsLayer::onShowPicker));
        gColorItem->setTag(1001);
        m_gColorSprite->setTag(1001);
        m_buttonMenu->addChild(gColorItem);

        m_g2ColorSprite = ColorChannelSprite::create();
        m_g2ColorSprite->setScale(0.8f);
        CCMenuItemSpriteExtra* g2ColorItem = CCMenuItemSpriteExtra::create(
            m_g2ColorSprite, nullptr, this, menu_selector(LevelSettingsLayer::onShowPicker));
        g2ColorItem->setTag(1009);
        m_g2ColorSprite->setTag(1009);
        m_buttonMenu->addChild(g2ColorItem);

        m_lineColorSprite = ColorChannelSprite::create();
        m_lineColorSprite->setScale(0.8f);
        CCMenuItemSpriteExtra* lineColorItem = CCMenuItemSpriteExtra::create(
            m_lineColorSprite, nullptr, this, menu_selector(LevelSettingsLayer::onShowPicker));
        lineColorItem->setTag(1002);
        m_lineColorSprite->setTag(1002);
        m_buttonMenu->addChild(lineColorItem);

        m_mgColorSprite = ColorChannelSprite::create();
        m_mgColorSprite->setScale(0.8f);
        CCMenuItemSpriteExtra* mgColorItem = CCMenuItemSpriteExtra::create(
            m_mgColorSprite, nullptr, this, menu_selector(LevelSettingsLayer::onShowPicker));
        mgColorItem->setTag(1013);
        m_mgColorSprite->setTag(1013);
        m_buttonMenu->addChild(mgColorItem);

        m_mg2ColorSprite = ColorChannelSprite::create();
        m_mg2ColorSprite->setScale(0.8f);
        CCMenuItemSpriteExtra* mg2ColorItem = CCMenuItemSpriteExtra::create(
            m_mg2ColorSprite, nullptr, this, menu_selector(LevelSettingsLayer::onShowPicker));
        mg2ColorItem->setTag(1014);
        m_mg2ColorSprite->setTag(1014);
        m_buttonMenu->addChild(mg2ColorItem);

        m_moreColorsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
        m_moreColorsSprite->setScale(0.8f);
        m_moreColorsSprite->setColor(ccc3(100, 100, 100));
        cocos2d::CCLabelBMFont* moreBtnLabel = cocos2d::CCLabelBMFont::create("+", "bigFont.fnt");
        m_moreColorsSprite->addChild(moreBtnLabel, 1);
        moreBtnLabel->setPosition(ccp(
            m_moreColorsSprite->getContentSize().width * 0.5f + 1.0f,
            m_moreColorsSprite->getContentSize().height * 0.5f + 2.0f));
        CCMenuItemSpriteExtra* moreColorsItem = CCMenuItemSpriteExtra::create(
            m_moreColorsSprite, nullptr, this, menu_selector(LevelSettingsLayer::onCol));
        m_buttonMenu->addChild(moreColorsItem);

        bgColorItem->setPosition(m_buttonMenu->convertToNodeSpace(
            bgLabel->getPosition() + ccp(0.0f, -22.0f)));
        gColorItem->setPosition(m_buttonMenu->convertToNodeSpace(
            gLabel->getPosition() + ccp(0.0f, -22.0f)));
        g2ColorItem->setPosition(m_buttonMenu->convertToNodeSpace(
            g2Label->getPosition() + ccp(0.0f, -22.0f)));
        lineColorItem->setPosition(m_buttonMenu->convertToNodeSpace(
            lineLabel->getPosition() + ccp(0.0f, -22.0f)));
        mgColorItem->setPosition(m_buttonMenu->convertToNodeSpace(
            mgLabel->getPosition() + ccp(0.0f, -22.0f)));
        mg2ColorItem->setPosition(m_buttonMenu->convertToNodeSpace(
            mg2Label->getPosition() + ccp(0.0f, -22.0f)));
        moreColorsItem->setPosition(m_buttonMenu->convertToNodeSpace(
            moreLabel->getPosition() + ccp(0.0f, -22.0f)));

        updateColorSprites();

        CCMenuItemSpriteExtra* colorItems[6] = {
            bgColorItem, gColorItem, g2ColorItem,
            lineColorItem, mgColorItem, mg2ColorItem
        };
        int colorTags[6] = { 1000, 1001, 1009, 1002, 1013, 1014 };
        float colorOffsets[6] = { 0.0f, -3.2f, 0.0f, 2.4f, 0.0f, 2.4f };

        for (int i = 0; i < 6; ++i) {
            cocos2d::CCPoint itemPos = colorItems[i]->getPosition();
            cocos2d::CCSprite* liveSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
            liveSprite->setColor(ccc3(100, 100, 100));
            liveSprite->setOpacity(100);
            cocos2d::CCLabelBMFont* liveLabel = cocos2d::CCLabelBMFont::create("E", "bigFont.fnt");
            liveSprite->addChild(liveLabel);
            liveLabel->setScale(1.4f);
            liveLabel->setOpacity(100);
            liveLabel->setPosition(liveSprite->convertToNodeSpace(ccp(2.0f, 2.5f)));
            liveSprite->setScale(0.3f);

            CCMenuItemSpriteExtra* liveItem = CCMenuItemSpriteExtra::create(
                liveSprite, nullptr, this, menu_selector(LevelSettingsLayer::onLiveEdit));
            m_buttonMenu->addChild(liveItem);
            liveItem->setTag(colorTags[i]);
            liveItem->setPosition(itemPos + ccp(colorOffsets[i] + 18.0f, 22.0f));
            liveItem->setSizeMult(2.0f);
        }

        if (!object->m_startsWithStartPos) {
            cocos2d::CCPoint modePos = ccp(winSize.width * 0.5f, winSize.height * 0.5f + 31.0f);
            cocos2d::CCLabelBMFont* modeLabel = cocos2d::CCLabelBMFont::create("Mode", "bigFont.fnt");
            modeLabel->setScale(0.7f);
            m_mainLayer->addChild(modeLabel, 10);
            modeLabel->setPosition(modePos + ccp(0.0f, 28.0f));

            cocos2d::CCNode* classicNode = ButtonSprite::create(
                "Classic", 0, true, "bigFont.fnt", "GJ_button_04.png", 24.0f);
            m_classicButton = CCMenuItemSpriteExtra::create(
                classicNode, nullptr, this, menu_selector(LevelSettingsLayer::onGameplayMode));
            m_classicButton->setTag(3);
            m_buttonMenu->addChild(m_classicButton);
            m_classicButton->setPosition(m_buttonMenu->convertToNodeSpace(modePos + ccp(-65.0f, 0.0f)));

            cocos2d::CCNode* platformerNode = ButtonSprite::create(
                "Platformer", 0, true, "bigFont.fnt", "GJ_button_04.png", 24.0f);
            m_platformerButton = CCMenuItemSpriteExtra::create(
                platformerNode, nullptr, this, menu_selector(LevelSettingsLayer::onGameplayMode));
            m_platformerButton->setTag(4);
            m_buttonMenu->addChild(m_platformerButton);
            m_platformerButton->setPosition(m_buttonMenu->convertToNodeSpace(modePos + ccp(65.0f, 0.0f)));

            updateGameplayModeButtons();

            leftX = (winSize.width * 0.5f) - 185.0f;

            if (!object->m_startsWithStartPos) {
                float rightX = (winSize.width * 0.5f) + 185.0f;

                cocos2d::CCLabelBMFont* bgArtLabel = cocos2d::CCLabelBMFont::create("BG", "bigFont.fnt");
                bgArtLabel->setScale(0.6f);
                m_mainLayer->addChild(bgArtLabel);
                bgArtLabel->setPosition(ccp(rightX, winSize.height * 0.5f + 135.0f));

                int bgIndex = object->m_backgroundIndex;
                int bgFrame = 59;
                if (bgIndex <= 58) {
                    bgFrame = (bgIndex <= 0) ? 1 : bgIndex;
                }
                cocos2d::CCString* bgFrameStr = cocos2d::CCString::createWithFormat("bgIcon_%02d_001.png", bgFrame);
                m_backgroundSprite = cocos2d::CCSprite::createWithSpriteFrameName(bgFrameStr->getCString());
                CCMenuItemSpriteExtra* bgArtItem = CCMenuItemSpriteExtra::create(
                    m_backgroundSprite, nullptr, this, menu_selector(LevelSettingsLayer::onBGArt));
                m_buttonMenu->addChild(bgArtItem);
                bgArtItem->setPosition(m_buttonMenu->convertToNodeSpace(
                    bgArtLabel->getPosition() + ccp(0.0f, -32.0f)));

                cocos2d::CCLabelBMFont* gArtLabel = cocos2d::CCLabelBMFont::create("G", "bigFont.fnt");
                gArtLabel->setScale(0.6f);
                m_mainLayer->addChild(gArtLabel);
                gArtLabel->setPosition(ccp(rightX, winSize.height * 0.5f + 70.0f));

                int gIndex = object->m_groundIndex;
                int gFrame = 22;
                if (gIndex <= 21) {
                    gFrame = (gIndex <= 0) ? 1 : gIndex;
                }
                cocos2d::CCString* gFrameStr = cocos2d::CCString::createWithFormat("gIcon_%02d_001.png", gFrame);
                m_groundSprite = cocos2d::CCSprite::createWithSpriteFrameName(gFrameStr->getCString());
                CCMenuItemSpriteExtra* gArtItem = CCMenuItemSpriteExtra::create(
                    m_groundSprite, nullptr, this, menu_selector(LevelSettingsLayer::onGArt));
                m_buttonMenu->addChild(gArtItem);
                gArtItem->setPosition(m_buttonMenu->convertToNodeSpace(
                    gArtLabel->getPosition() + ccp(0.0f, -32.0f)));

                cocos2d::CCLabelBMFont* mgArtLabel = cocos2d::CCLabelBMFont::create("MG", "bigFont.fnt");
                mgArtLabel->setScale(0.6f);
                m_mainLayer->addChild(mgArtLabel);
                mgArtLabel->setPosition(ccp(rightX, winSize.height * 0.5f + 5.0f));

                int mgIndex = object->m_middleGroundIndex;
                int mgFrame = 3;
                if (mgIndex <= 2) {
                    mgFrame = (mgIndex < 0) ? 0 : mgIndex;
                }
                cocos2d::CCString* mgFrameStr = cocos2d::CCString::createWithFormat("mgIcon_%02d_001.png", mgFrame);
                m_middlegroundSprite = cocos2d::CCSprite::createWithSpriteFrameName(mgFrameStr->getCString());
                CCMenuItemSpriteExtra* mgArtItem = CCMenuItemSpriteExtra::create(
                    m_middlegroundSprite, nullptr, this, menu_selector(LevelSettingsLayer::onFGArt));
                m_buttonMenu->addChild(mgArtItem);
                mgArtItem->setPosition(m_buttonMenu->convertToNodeSpace(
                    mgArtLabel->getPosition() + ccp(0.0f, -32.0f)));

                cocos2d::CCNode* fontNode = ButtonSprite::create(
                    "Font", 0, true, "bigFont.fnt", "GJ_button_04.png", 28.0f);
                CCMenuItemSpriteExtra* fontItem = CCMenuItemSpriteExtra::create(
                    fontNode, nullptr, this, menu_selector(LevelSettingsLayer::onSelectFont));
                m_buttonMenu->addChild(fontItem);
                fontItem->setPosition(m_buttonMenu->convertToNodeSpace(
                    ccp(rightX, winSize.height * 0.5f) + ccp(0.0f, -70.0f)));
            }
        }
    }

    cocos2d::CCNode* okNode = ButtonSprite::create(
        "OK", 0, true, "bigFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okBtn = CCMenuItemSpriteExtra::create(
        okNode, nullptr, this, menu_selector(LevelSettingsLayer::onClose));
    m_buttonMenu->addChild(okBtn);
    okBtn->setPosition(m_buttonMenu->convertToNodeSpace(
        ccp(winSize.width * 0.5f, winSize.height * 0.5f - 130.0f)));

    if (object->m_startsWithStartPos) {
        cocos2d::CCLabelBMFont* startPosLabel = cocos2d::CCLabelBMFont::create("Start Pos", "bigFont.fnt");
        startPosLabel->setScale(0.6f);
        m_mainLayer->addChild(startPosLabel);
        startPosLabel->setPosition(ccp(leftX + 270.0f, winSize.height * 0.5f - 90.0f));

        GameToolbox::createToggleButton(
            "", this, menu_selector(LevelSettingsLayer::onDisable),
            0, !m_settingsObject->m_disableStartPos, m_buttonMenu,
            startPosLabel->getPosition() + ccp(0.0f, -30.0f), this, m_mainLayer);

        CCMenuItemToggler* resetCamBtn = GameToolbox::createToggleButton(
            "Reset\nCamera", this, menu_selector(LevelSettingsLayer::onOptionToggle),
            1, object->m_resetCamera, m_buttonMenu,
            ccp(winSize.width * 0.5f, winSize.height * 0.5f), this, m_mainLayer);
        resetCamBtn->setTag(1);

        cocos2d::CCPoint orderPos = ccp(winSize.width * 0.5f - 100.0f, winSize.height * 0.5f - 60.0f);

        cocos2d::CCLabelBMFont* orderLabel = cocos2d::CCLabelBMFont::create("Order", "bigFont.fnt");
        orderLabel->setScale(0.6f);
        m_mainLayer->addChild(orderLabel);
        orderLabel->setPosition(orderPos + ccp(0.0f, 30.0f));

        cocos2d::CCRect smallRect = cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f);
        cocos2d::extension::CCScale9Sprite* orderBG = cocos2d::extension::CCScale9Sprite::create(
            "square02b_small.png", smallRect);
        orderBG->setColor(ccc3(0, 0, 0));
        orderBG->setOpacity(100);
        orderBG->setContentSize(cocos2d::CCSize(50.0f, 30.0f));
        m_mainLayer->addChild(orderBG, -1);
        orderBG->setPosition(orderPos);

        m_orderInput = CCTextInputNode::create(50.0f, 40.0f, "Order", "Thonburi");
        m_mainLayer->addChild(m_orderInput, 20);
        m_orderInput->setPosition(orderPos);
        m_orderInput->setMaxLabelScale(0.6f);
        m_orderInput->setMaxLabelWidth(200.0f);
        m_orderInput->m_textField->setVisible(false);
        m_orderInput->m_maxLabelLength = 6;
        m_orderInput->setAllowedChars("0123456789");
        m_orderInput->setLabelPlaceholderColor(ccc3(120, 170, 240));
        m_orderInput->setLabelPlaceholderScale(0.5f);
        m_orderInput->m_delegate = this;
        m_orderInput->m_numberInput = true;
        m_orderInput->setTag(1);

        if (m_settingsObject->m_targetOrder > 0)
            m_orderInput->setString(GameToolbox::intToString(m_settingsObject->m_targetOrder));

        cocos2d::CCPoint channelPos = orderPos + ccp(200.0f, 0.0f);

        cocos2d::CCLabelBMFont* channelLabel = cocos2d::CCLabelBMFont::create("Channel", "bigFont.fnt");
        channelLabel->setScale(0.6f);
        m_mainLayer->addChild(channelLabel);
        channelLabel->setPosition(channelPos + ccp(0.0f, 30.0f));

        cocos2d::extension::CCScale9Sprite* channelBG = cocos2d::extension::CCScale9Sprite::create(
            "square02b_small.png", smallRect);
        channelBG->setColor(ccc3(0, 0, 0));
        channelBG->setOpacity(100);
        channelBG->setContentSize(cocos2d::CCSize(50.0f, 30.0f));
        m_mainLayer->addChild(channelBG, -1);
        channelBG->setPosition(channelPos);

        m_channelInput = CCTextInputNode::create(50.0f, 40.0f, "Channel", "Thonburi");
        m_mainLayer->addChild(m_channelInput, 20);
        m_channelInput->setPosition(channelPos);
        m_channelInput->setMaxLabelScale(0.6f);
        m_channelInput->setMaxLabelWidth(200.0f);
        m_channelInput->m_textField->setVisible(false);
        m_channelInput->m_maxLabelLength = 6;
        m_channelInput->setAllowedChars("0123456789");
        m_channelInput->setLabelPlaceholderColor(ccc3(120, 170, 240));
        m_channelInput->setLabelPlaceholderScale(0.5f);
        m_channelInput->m_delegate = this;
        m_channelInput->m_numberInput = true;
        m_channelInput->setTag(2);

        if (m_settingsObject->m_targetChannel > 0)
            m_channelInput->setString(GameToolbox::intToString(m_settingsObject->m_targetChannel));
    } else {
        GJGameLevel* level = m_settingsObject->m_level;
        int songID = level->m_songID;
        bool isCustom = songID != 0;
        if (!songID)
            songID = level->m_audioTrack;
        m_songSelectNode = SongSelectNode::create(
            songID, isCustom, m_settingsObject, SongSelectType::Default,
            ccp(winSize.width * 0.5f, winSize.height * 0.5f - 67.0f),
            m_mainLayer, m_buttonMenu);
        m_mainLayer->addChild(m_songSelectNode, 5);
    }

    cocos2d::CCPoint speedPos = ccp(leftX, winSize.height * 0.5f + 135.0f);

    cocos2d::CCLabelBMFont* speedLabel = cocos2d::CCLabelBMFont::create("Speed", "bigFont.fnt");
    speedLabel->setScale(0.6f);
    m_mainLayer->addChild(speedLabel);
    speedLabel->setPosition(speedPos);

    m_speedSprite = cocos2d::CCSprite::createWithSpriteFrameName(
        SelectSettingLayer::frameForValue(SelectSettingType::StartingSpeed, m_settingsObject->m_startSpeed).c_str());
    m_speedSprite->setScale(0.6f);
    CCMenuItemSpriteExtra* speedItem = CCMenuItemSpriteExtra::create(
        m_speedSprite, nullptr, this, menu_selector(LevelSettingsLayer::onSpeed));
    m_buttonMenu->addChild(speedItem);
    speedItem->setPosition(m_buttonMenu->convertToNodeSpace(speedPos + ccp(0.0f, -34.0f)));

    cocos2d::CCPoint modePos2 = ccp(leftX, winSize.height * 0.5f + 135.0f - 80.0f);

    cocos2d::CCLabelBMFont* modeLabel2 = cocos2d::CCLabelBMFont::create("Mode", "bigFont.fnt");
    modeLabel2->setScale(0.6f);
    m_mainLayer->addChild(modeLabel2);
    modeLabel2->setPosition(modePos2);

    m_modeSprite = cocos2d::CCSprite::createWithSpriteFrameName(
        SelectSettingLayer::frameForValue(
            (SelectSettingType)m_settingsObject->m_platformerMode,
            m_settingsObject->m_startMode).c_str());
    m_modeSprite->setScale(1.0f);
    CCMenuItemSpriteExtra* modeItem = CCMenuItemSpriteExtra::create(
        m_modeSprite, nullptr, this, menu_selector(LevelSettingsLayer::onMode));
    m_buttonMenu->addChild(modeItem);
    modeItem->setPosition(m_buttonMenu->convertToNodeSpace(modePos2 + ccp(0.0f, -32.0f)));

    cocos2d::CCPoint optionsPos = ccp(leftX, winSize.height * 0.5f + 135.0f - 160.0f);

    cocos2d::CCLabelBMFont* optionsLabel = cocos2d::CCLabelBMFont::create("Options", "bigFont.fnt");
    optionsLabel->setScale(0.6f);
    m_mainLayer->addChild(optionsLabel);
    optionsLabel->setPosition(optionsPos);

    cocos2d::CCSprite* optionsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    optionsSprite->setScale(0.6f);
    CCMenuItemSpriteExtra* optionsItem = CCMenuItemSpriteExtra::create(
        optionsSprite, nullptr, this, menu_selector(LevelSettingsLayer::onSettings));
    m_buttonMenu->addChild(optionsItem);
    optionsItem->setPosition(m_buttonMenu->convertToNodeSpace(optionsPos + ccp(0.0f, -32.0f)));

    return true;
}

void LevelSettingsLayer::keyBackClicked()
{
    onClose(nullptr);
}

void LevelSettingsLayer::onBGArt(cocos2d::CCObject* sender)
{
    SelectArtLayer* layer = SelectArtLayer::create(SelectArtType::Background, m_settingsObject->m_backgroundIndex);
    layer->m_delegate = this;
    layer->show();
}

void LevelSettingsLayer::onClose(cocos2d::CCObject* sender)
{
    if (m_orderInput) {
        m_orderInput->m_delegate = nullptr;
        m_orderInput->onClickTrackNode(false);
    }
    if (m_channelInput) {
        m_channelInput->m_delegate = nullptr;
        m_channelInput->onClickTrackNode(false);
    }
    if (!m_settingsObject->m_startsWithStartPos) {
        if (m_songSelectNode && m_songSelectNode->m_songChanged) {
            if (m_songSelectNode->m_isCustomSong) {
                m_settingsObject->m_level->m_songID = m_songSelectNode->m_selectedSongID;
            } else {
                GJGameLevel* level = m_settingsObject->m_level;
                level->m_songID = 0;
                level->m_audioTrack = m_songSelectNode->m_selectedSongID;
            }
        } else {
            if (!(m_settingsObject->m_startSpeed == m_speed && !m_settingsObject->m_guidelinesUpdated))
                m_settingsObject->m_guidelinesUpdated = false;
            if (m_delegate)
                m_delegate->levelSettingsUpdated();
        }
    }
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void LevelSettingsLayer::onCol(cocos2d::CCObject* sender)
{
    GJColorSetupLayer* layer = GJColorSetupLayer::create(m_settingsObject);
    layer->show();
}

void LevelSettingsLayer::onDisable(cocos2d::CCObject* sender)
{
    m_settingsObject->m_disableStartPos ^= 1;
}

void LevelSettingsLayer::onFGArt(cocos2d::CCObject* sender)
{
    SelectArtLayer* layer = SelectArtLayer::create(SelectArtType::Middleground, m_settingsObject->m_middleGroundIndex);
    layer->m_delegate = this;
    layer->show();
}

void LevelSettingsLayer::onGArt(cocos2d::CCObject* sender)
{
    SelectArtLayer* layer = SelectArtLayer::create(SelectArtType::Ground, m_settingsObject->m_groundIndex);
    layer->m_delegate = this;
    layer->show();
}

void LevelSettingsLayer::onGameplayMode(cocos2d::CCObject* sender)
{
    m_settingsObject->m_platformerMode = sender->getTag() == 4;
    updateGameplayModeButtons();
}

void LevelSettingsLayer::onLiveEdit(cocos2d::CCObject* sender)
{
    onClose(nullptr);
    int tag = sender->getTag();
    m_editorLayer->m_editorUI->showLiveColorSelectForMode(tag);
}

void LevelSettingsLayer::onMode(cocos2d::CCObject* sender)
{
    SelectSettingLayer* layer = SelectSettingLayer::create(
        (SelectSettingType)m_settingsObject->m_platformerMode,
        m_settingsObject->m_startMode);
    layer->m_delegate = this;
    layer->show();
}

void LevelSettingsLayer::onOptionToggle(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 1)
        m_settingsObject->m_resetCamera ^= 1;
}

void LevelSettingsLayer::onSelectFont(cocos2d::CCObject* sender)
{
    SelectFontLayer* layer = SelectFontLayer::create(m_editorLayer);
    layer->show();
}

void LevelSettingsLayer::onSelectMode(cocos2d::CCObject* sender)
{
    if (sender)
        m_settingsObject->m_startMode = sender->getTag();
    int startMode = m_settingsObject->m_startMode;
    for (unsigned int i = 0; i < m_modeToggles->count(); ++i) {
        CCMenuItemToggler* toggler = (CCMenuItemToggler*)m_modeToggles->objectAtIndex(i);
        bool isSelected = startMode == toggler->getTag();
        toggler->toggle(isSelected);
        toggler->setEnabled(startMode != toggler->getTag());
    }
}

void LevelSettingsLayer::onSelectSpeed(cocos2d::CCObject* sender)
{
    if (sender)
        m_settingsObject->m_startSpeed = sender->getTag();
    Speed startSpeed = m_settingsObject->m_startSpeed;
    for (unsigned int i = 0; i < m_speedButtons->count(); ++i) {
        cocos2d::CCObject* obj = m_speedButtons->objectAtIndex(i);
        cocos2d::CCSprite* sprite = (cocos2d::CCSprite*)obj;
        ccColor3B color = startSpeed == obj->getTag() ? ccc3(255, 255, 255) : ccc3(0, 0, 0);
        sprite->setColor(color);
        sprite->setOpacity(startSpeed == obj->getTag() ? 255 : 85);
    }
}

void LevelSettingsLayer::onSettings(cocos2d::CCObject* sender)
{
    LevelOptionsLayer* layer = LevelOptionsLayer::create(m_settingsObject);
    layer->show();
}

void LevelSettingsLayer::onShowPicker(cocos2d::CCObject* sender)
{
    GJEffectManager* effectManager = m_settingsObject->m_effectManager;
    int tag = sender->getTag();
    ColorAction* action = effectManager->getColorAction(tag);
    showPicker(action);
}

void LevelSettingsLayer::onSpeed(cocos2d::CCObject* sender)
{
    SelectSettingLayer* layer = SelectSettingLayer::create(
        SelectSettingType::StartingSpeed, m_settingsObject->m_startSpeed);
    layer->m_delegate = this;
    layer->show();
}

void LevelSettingsLayer::registerWithTouchDispatcher()
{
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, -503, true);
}

void LevelSettingsLayer::selectArtClosed(SelectArtLayer* layer)
{
    SelectArtType type = layer->m_type;
    if (type == SelectArtType::Ground) {
        if (m_editorLayer)
            m_editorLayer->removeGroundLayer();
        m_settingsObject->m_groundIndex = layer->m_art;
        m_settingsObject->m_groundLineIndex = layer->m_line;
        GameManager::sharedState()->loadGround(layer->m_art);
        m_groundSprite->setDisplayFrame(
            cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                cocos2d::CCString::createWithFormat("gIcon_%02d_001.png", layer->m_art)->getCString()));
        if (m_editorLayer)
            m_editorLayer->createGroundLayer(m_settingsObject->m_groundIndex, m_settingsObject->m_groundLineIndex);
    } else if (type == SelectArtType::Middleground) {
        if (m_editorLayer)
            m_editorLayer->removeMiddleground();
        m_settingsObject->m_middleGroundIndex = layer->m_art;
        GameManager::sharedState()->loadMiddleground(layer->m_art);
        m_middlegroundSprite->setDisplayFrame(
            cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                cocos2d::CCString::createWithFormat("mgIcon_%02d_001.png", layer->m_art)->getCString()));
        if (m_editorLayer)
            m_editorLayer->createMiddleground(m_settingsObject->m_middleGroundIndex);
    } else {
        if (m_editorLayer)
            m_editorLayer->removeBackground();
        m_settingsObject->m_backgroundIndex = layer->m_art;
        GameManager::sharedState()->loadBackground(layer->m_art);
        m_backgroundSprite->setDisplayFrame(
            cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                cocos2d::CCString::createWithFormat("bgIcon_%02d_001.png", layer->m_art)->getCString()));
        if (m_editorLayer)
            m_editorLayer->createBackground(m_settingsObject->m_backgroundIndex);
    }
}

void LevelSettingsLayer::selectSettingClosed(SelectSettingLayer* layer)
{
    SelectSettingType type = layer->m_type;
    int selectedValue = layer->getSelectedValue();
    if ((unsigned int)type <= (unsigned int)SelectSettingType::StartingModePlatformer) {
        m_modeSprite->setDisplayFrame(
            cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                layer->getSelectedFrame().c_str()));
        m_settingsObject->m_startMode = selectedValue;
    } else if (type == SelectSettingType::StartingSpeed) {
        m_speedSprite->setDisplayFrame(
            cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                layer->getSelectedFrame().c_str()));
        m_settingsObject->m_startSpeed = selectedValue;
    }
}

void LevelSettingsLayer::showPicker(ColorAction* action)
{
    ColorSelectPopup* popup = ColorSelectPopup::create(action);
    popup->setDelegate(this);
    popup->show();
}

void LevelSettingsLayer::textChanged(CCTextInputNode* node)
{
    if (!m_disableInput) {
        if (node->getTag() == 1) {
            int value = atoi(node->getString().c_str());
            m_settingsObject->m_targetOrder = value;
        } else if (node->getTag() == 2) {
            int value = atoi(node->getString().c_str());
            m_settingsObject->m_targetChannel = value;
        }
    }
}

void LevelSettingsLayer::textInputClosed(CCTextInputNode* node)
{
    textChanged(node);
}

void LevelSettingsLayer::updateColorSprite(ColorChannelSprite* sprite)
{
    if (m_editorLayer) {
        int tag = sprite->getTag();
        if (m_editorLayer->m_levelSettings->m_effectManager->colorExists(tag)) {
            ColorAction* action = m_editorLayer->m_levelSettings->m_effectManager->getColorAction(tag);
            sprite->updateValues(action);
        } else {
            sprite->setColor(ccc3(255, 255, 255));
        }
    }
}

void LevelSettingsLayer::updateColorSprites()
{
    updateColorSprite(m_bgColorSprite);
    updateColorSprite(m_gColorSprite);
    updateColorSprite(m_g2ColorSprite);
    updateColorSprite(m_lineColorSprite);
    updateColorSprite(m_mgColorSprite);
    updateColorSprite(m_mg2ColorSprite);
}

void LevelSettingsLayer::updateGameplayModeButtons()
{
    ButtonSprite* classicSprite = (ButtonSprite*)m_classicButton->getNormalImage();
    ButtonSprite* platformerSprite = (ButtonSprite*)m_platformerButton->getNormalImage();
    bool isPlatformer = m_settingsObject->m_platformerMode;
    m_classicButton->setEnabled(isPlatformer);
    if (isPlatformer) {
        classicSprite->updateBGImage("GJ_button_04.png");
        m_platformerButton->setEnabled(false);
        platformerSprite->updateBGImage("GJ_button_01.png");
    } else {
        classicSprite->updateBGImage("GJ_button_01.png");
        m_platformerButton->setEnabled(true);
        platformerSprite->updateBGImage("GJ_button_04.png");
    }
}

LevelSettingsLayer::~LevelSettingsLayer()
{
}