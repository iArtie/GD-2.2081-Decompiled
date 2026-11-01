#include "SetupSequenceTriggerPopup.h"
#include "SetupTriggerPopup.h"
#include "SequenceTriggerGameObject.h"
#include "EffectGameObject.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCMenuItemToggler.h"
#include "InfoAlertButton.h"

SetupSequenceTriggerPopup* SetupSequenceTriggerPopup::create(SequenceTriggerGameObject* object)
{
    SetupSequenceTriggerPopup* instance = new SetupSequenceTriggerPopup();
    if (instance->init(object))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupSequenceTriggerPopup::init(SequenceTriggerGameObject* object)
{
    if (!SetupTriggerPopup::init(object, nullptr, 420.0f, 300.0f, 1))
        return false;

    preSetup();

    m_groupButtons = cocos2d::CCArray::create();
    m_groupButtons->retain();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.7f);
    titleLabel->setPosition(winSize.width * 0.5f, m_height * 0.5f + winSize.height * 0.5f - 18.0f);

    ButtonSprite* okButton = ButtonSprite::create("OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okItem = CCMenuItemSpriteExtra::create(
        okButton, nullptr, menu_selector(SetupSequenceTriggerPopup::onClose));
    m_buttonMenu->addChild(okItem);

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "Activates a <cg>sequence</c> of triggers. Every time it is called, the sequence is advanced one step.\n"
        "<cy>GroupID</c> is called the amount of times given by <cl>Count</c>.\n"
        "<cp>MinInt</c> is the minimum interval between each step.\n"
        "<cr>Mode Stop</c> stops the sequence at the end.\n"
        "<co>Mode Loop</c> restarts the sequence after ending.\n"
        "<ca>Mode Last</c> calls last group after the end.\n"
        "<cj>Reset Full and Reset Step</c> go back in the sequence if activation does not happen in a set time.",
        1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(18.0f - (m_width * 0.5f - winSize.width * 0.5f),
                         m_height * 0.5f + winSize.height * 0.5f - 18.0f)));

    cocos2d::CCPoint inputBase(winSize.width * 0.5f - 65.0f, winSize.height * 0.5f + 82.0f);

    createValueControlAdvanced(
        -1, "GroupID:", cocos2d::CCPoint(0.7f, 0.0f), 100.0f,
        &(inputBase + cocos2d::CCPoint(-55.0f, 0.0f)), Float, 3, 6, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 2);

    createValueControlAdvanced(
        -2, "Count:", cocos2d::CCPoint(0.7f, 0.0f), 100.0f,
        &(inputBase + cocos2d::CCPoint(55.0f, 0.0f)), Float, 2, 6, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 2);

    cocos2d::CCPoint buttonBase = m_buttonMenu->convertToNodeSpace(inputBase + cocos2d::CCPoint(130.0f, 0.0f));

    for (int i = 0; i < 4; i++)
    {
        SEL_MenuHandler handler = menu_selector(SetupSequenceTriggerPopup::onAddChance);
        const char* frameName = "edit_addCBtn_001.png";
        const char* bgName = "GJ_button_01.png";

        if (i == 1)
        {
            frameName = "edit_delCBtn_001.png";
            bgName = "GJ_button_06.png";
            handler = menu_selector(SetupSequenceTriggerPopup::onDeleteSelected);
        }
        else if (i >= 2)
        {
            frameName = "edit_upBtn_001.png";
            bgName = "GJ_button_05.png";
            handler = menu_selector(SetupSequenceTriggerPopup::onChangeOrder);
        }

        cocos2d::CCSprite* icon = cocos2d::CCSprite::createWithSpriteFrameName(frameName);
        if (i == 2) icon->setRotation(-90.0f);
        else if (i == 3) icon->setRotation(90.0f);

        ButtonSprite* btn = ButtonSprite::create(icon, 0, 0, 1.0f, bgName);
        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(btn, nullptr, handler);
        item->setScale(0.6f);
        item->m_baseScale = 0.6f;
        m_buttonMenu->addChild(item);
        item->setPosition(buttonBase + cocos2d::CCPoint(i * 32.0f, 0.0f));
        item->setTag(i);
    }

    cocos2d::extension::CCScale9Sprite* listBg = cocos2d::extension::CCScale9Sprite::create(
        "square02_001.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    listBg->setOpacity(50);
    listBg->setContentSize(cocos2d::CCSize(370.0f, 90.0f));
    m_mainLayer->addChild(listBg, -1);
    listBg->setPosition(winSize.width * 0.5f, winSize.height * 0.5f + 14.0f);

    cocos2d::CCPoint optionsBase(winSize.width * 0.5f, winSize.height * 0.5f - 50.0f);

    createValueControlAdvanced(
        437, "MinInt:", cocos2d::CCPoint(0.65f, 0.0f), 1.0f,
        &(optionsBase + cocos2d::CCPoint(-91.0f, 0.0f)), (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 2);

    createValueControlAdvanced(
        438, "Reset:", cocos2d::CCPoint(0.65f, 0.0f), 1.0f,
        &(optionsBase + cocos2d::CCPoint(91.0f, 0.0f)), (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 2);

    cocos2d::CCPoint toggleBase = optionsBase + cocos2d::CCPoint(-25.0f, -34.0f);

    int mode = (int)getValue(436);
    int resetMode = (int)getValue(439);

    createCustomToggleValueControl(0, mode == 0, 1, "Mode\nStop",
        cocos2d::CCPoint(1.0f, 0.0f), &(toggleBase + cocos2d::CCPoint(-158.0f, 0.0f)), 0, 0);
    createCustomToggleValueControl(1, mode == 1, 1, "Mode\nLoop",
        cocos2d::CCPoint(1.0f, 0.0f), &(toggleBase + cocos2d::CCPoint(-79.0f, 0.0f)), 0, 0);
    createCustomToggleValueControl(2, mode == 2, 1, "Mode\nLast",
        cocos2d::CCPoint(1.0f, 0.0f), &toggleBase, 0, 0);
    createCustomToggleValueControl(100, resetMode != 1, 1, "Reset\nFull",
        cocos2d::CCPoint(1.0f, 0.0f), &(toggleBase + cocos2d::CCPoint(79.0f, 0.0f)), 0, 0);
    createCustomToggleValueControl(101, resetMode == 1, 1, "Reset\nStep",
        cocos2d::CCPoint(1.0f, 0.0f), &(toggleBase + cocos2d::CCPoint(158.0f, 0.0f)), 0, 0);

    cocos2d::CCPoint remapPos(winSize.width * 0.5f + 55.0f, 20.0f - (m_height * 0.5f - winSize.height * 0.5f));
    createToggleValueControl(505, "Unique\nRemap", cocos2d::CCPoint(1.0f, 0.0f), &remapPos, 0, 0, 0.0f);

    createMultiTriggerItemsDefaultHorizontal();

    updateGroupIDButtons();

    postSetup();

    updateValue(-2, 1.0f);

    return true;
}

void SetupSequenceTriggerPopup::onAddChance(cocos2d::CCObject* sender)
{
    float groupID = getValue(-1);
    float count = getValue(-2);
    int countInt = (int)count;

    if (m_selectedButton)
    {
        if (!countInt) return;
        SequenceTriggerGameObject::addCount((SequenceTriggerGameObject*)m_gameObject,
            m_selectedButton->getTag(), countInt);
        updateGroupIDButtons();
        return;
    }

    int groupIDInt = (int)groupID;
    if (countInt > 0 && groupIDInt > 0)
    {
        SequenceTriggerGameObject::addTarget((SequenceTriggerGameObject*)m_gameObject, groupIDInt, countInt);
        updateGroupIDButtons();
    }
}

void SetupSequenceTriggerPopup::onChangeOrder(cocos2d::CCObject* sender)
{
    if (!m_selectedButton) return;

    bool up = sender->getTag() != 3;
    int tag = m_selectedButton->getTag();

    if (SequenceTriggerGameObject::reorderTarget((SequenceTriggerGameObject*)m_gameObject, tag, up))
    {
        m_selectedButton->setTag(tag + (up ? -1 : 1));
        updateGroupIDButtons();
    }
}

void SetupSequenceTriggerPopup::onCustomToggleTriggerValue(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();

    if (tag <= 99)
    {
        ((CCMenuItemToggler*)m_customValueToggles->objectForKey(0))->toggle(tag == 0);
        ((CCMenuItemToggler*)m_customValueToggles->objectForKey(1))->toggle(tag == 1);
        ((CCMenuItemToggler*)m_customValueToggles->objectForKey(2))->toggle(tag == 2);
        updateValue(436, (float)tag);
    }
    else
    {
        int resetTag = tag - 100;
        ((CCMenuItemToggler*)m_customValueToggles->objectForKey(100))->toggle(resetTag == 0);
        ((CCMenuItemToggler*)m_customValueToggles->objectForKey(101))->toggle(resetTag == 1);
        updateValue(439, (float)resetTag);
    }
}

void SetupSequenceTriggerPopup::onDeleteSelected(cocos2d::CCObject* sender)
{
    if (m_selectedButton)
        SequenceTriggerGameObject::deleteTarget((SequenceTriggerGameObject*)m_gameObject, m_selectedButton->getTag());
    else
        SequenceTriggerGameObject::deleteTarget((SequenceTriggerGameObject*)m_gameObject, 0);

    m_selectedButton = nullptr;
    updateGroupIDButtons();
}

void SetupSequenceTriggerPopup::onSelect(cocos2d::CCObject* sender)
{
    if (m_selectedButton)
    {
        ButtonSprite* btn = (ButtonSprite*)m_selectedButton->getNormalImage();
        btn->updateBGImage("GJ_button_04.png");
    }

    if (sender == m_selectedButton)
    {
        m_selectedButton = nullptr;
    }
    else
    {
        m_selectedButton = (CCMenuItemSpriteExtra*)sender;
        ButtonSprite* btn = (ButtonSprite*)m_selectedButton->getNormalImage();
        btn->updateBGImage("GJ_button_03.png");
    }
}

void SetupSequenceTriggerPopup::updateGroupIDButtons()
{
    int selectedTag = -1;
    if (m_selectedButton)
    {
        selectedTag = m_selectedButton->getTag();
        m_selectedButton = nullptr;
    }

    for (unsigned int i = 0; i < m_groupButtons->count(); i++)
        ((cocos2d::CCNode*)m_groupButtons->objectAtIndex(i))->removeFromParentAndCleanup(true);
    m_groupButtons->removeAllObjects();

    auto* gameObject = (SequenceTriggerGameObject*)m_gameObject;
    size_t count = gameObject->m_sequenceObjects.size();

    bool many = count > 10;
    float colWidth = many ? 35.0f : 66.0f;
    int columns = many ? 10 : 5;
    float rowHeight = many ? 38.0f : 66.0f;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint base = m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(
        -((many ? 4.5f : 2.0f) * colWidth - winSize.width * 0.5f),
        winSize.height * 0.5f + 34.0f));

    for (size_t i = 0; i < count && i < 20; i++)
    {
        auto* entry = gameObject->m_sequenceObjects[i];
        std::string text;
        if (many)
            text = cocos2d::CCString::createWithFormat("%i\n%i", entry->m_groupID, entry->m_count)->getCString();
        else
            text = cocos2d::CCString::createWithFormat("%i-%i", entry->m_groupID, entry->m_count)->getCString();

        const char* bgImage = (selectedTag == (int)i) ? "GJ_button_03.png" : "GJ_button_04.png";
        ButtonSprite* btn = ButtonSprite::create(text.c_str(), 0, 0, 0.35f, true, "bigFont.fnt", bgImage, 30.0f);
        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(
            btn, nullptr, menu_selector(SetupSequenceTriggerPopup::onSelect));
        item->setTag(i);
        m_buttonMenu->addChild(item);
        item->setPosition(base + cocos2d::CCPoint(
            colWidth * (int)(i % columns),
            -rowHeight * (int)(i / columns)));

        m_groupButtons->addObject(item);

        if (selectedTag == (int)i)
            m_selectedButton = item;
    }
}

SetupSequenceTriggerPopup::~SetupSequenceTriggerPopup()
{
}