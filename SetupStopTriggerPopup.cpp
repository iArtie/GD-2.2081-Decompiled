#include "SetupStopTriggerPopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"
#include "TriggerControlGameObject.h"

SetupStopTriggerPopup* SetupStopTriggerPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupStopTriggerPopup* instance = new SetupStopTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupStopTriggerPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 320.0f, 240.0f, 1))
        return false;

    preSetup();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    addTitle("Stop Trigger");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "<cg>Stops</c> triggers from being active permanently.\n"
        "<cy>TargetID</c> must be a group on the trigger(s) that you want to stop. If you only want to stop certain trigger"
        "s within a group, use <cl>Control ID</c>.\n"
        "Hint: Unlike Group ID, Control ID can be remapped.\n"
        "Pause/Resume works on: Pulse, Col, Alpha, Move, Rotate, Scale, Spawn, Follow, FollowPY, ADVFollow",
        1.0f);

    createMultiTriggerItemsDefault();

    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCPoint targetIDPos(winSize.width * 0.5f + -70.0f, winSize.height * 0.5f + 45.0f);
    createValueControlAdvanced(
        51, "TargetID", cocos2d::CCPoint(0.8f, 0.0f), 9999.0f, &targetIDPos,
        Float, 2, 10, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 0);

    cocos2d::CCPoint useControlPos(winSize.width * 0.5f + 45.0f, winSize.height * 0.5f + 45.0f);
    createToggleValueControl(535, "Use\nControl ID", cocos2d::CCPoint(1.0f, 0.0f), &useControlPos, 0, 0, 0.0f);

    cocos2d::CCPoint actionBase(center.x, center.y - 20.0f);

    int stopMode = (int)getValue(580);

    cocos2d::CCPoint stopPos = actionBase + cocos2d::CCPoint(-75.0f, 0.0f);
    createCustomToggleValueControl(0, stopMode == 0, 1, "Stop",
        cocos2d::CCPoint(1.0f, 0.0f), &stopPos, 1, 0);

    cocos2d::CCPoint pausePos = actionBase;
    createCustomToggleValueControl(1, stopMode == 1, 1, "Pause",
        cocos2d::CCPoint(1.0f, 0.0f), &pausePos, 1, 0);

    cocos2d::CCPoint resumePos = actionBase + cocos2d::CCPoint(75.0f, 0.0f);
    createCustomToggleValueControl(2, stopMode == 2, 1, "Resume",
        cocos2d::CCPoint(1.0f, 0.0f), &resumePos, 1, 0);

    postSetup();

    return true;
}

void SetupStopTriggerPopup::onClose(cocos2d::CCObject* sender)
{
    if (m_gameObject)
        m_gameObjects = cocos2d::CCArray::createWithObject(m_gameObject);

    if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            TriggerControlGameObject* obj = (TriggerControlGameObject*)m_gameObjects->objectAtIndex(i);
            obj->updateTriggerControlFrame();
        }
    }

    SetupTriggerPopup::onClose(sender);
}

void SetupStopTriggerPopup::onCustomToggleTriggerValue(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    updateValue(580, (float)tag);
    updateCustomToggleTrigger(0, tag == 0);
    updateCustomToggleTrigger(1, tag == 1);
    updateCustomToggleTrigger(2, tag == 2);
}

SetupStopTriggerPopup::~SetupStopTriggerPopup()
{
}