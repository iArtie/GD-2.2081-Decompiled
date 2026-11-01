#include "Objects/RandTriggerGameObject.h"
#include "Utils/GameToolbox.h"
#include "Layers/GJBaseGameLayer.h"

RandTriggerGameObject* RandTriggerGameObject::create()
{
    RandTriggerGameObject* returnVal = new RandTriggerGameObject();

    if (returnVal->init())
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

int RandTriggerGameObject::getRandomGroupID()
{
    int totalChance = getTotalChance();

    if (totalChance == 0 || m_chanceObjects.empty())
        return 0;

    float randomValue = GameToolbox::fast_rand_0_1();
    int targetChance = (int)llroundf(randomValue * (float)totalChance);

    int accumulatedChance = 0;

    for (size_t i = 0; i < m_chanceObjects.size(); i++)
    {
        accumulatedChance += m_chanceObjects[i].m_chance;

        if (targetChance <= accumulatedChance)
            return m_chanceObjects[i].m_groupID;
    }

    return 0;
}

std::string RandTriggerGameObject::getSaveString(GJBaseGameLayer* layer)
{
    std::string result = EffectGameObject::getSaveString(layer);

    if (m_activateGroup)
    {
        result += ",";
        result += std::to_string(56);
        result += ",";
        result += std::to_string(m_activateGroup);
    }

    result += ",";
    result += std::to_string(152);
    result += ",";

    for (size_t i = 0; i < m_chanceObjects.size(); i++)
    {
        if (i > 0)
            result += ".";

        result += std::to_string(m_chanceObjects[i].m_groupID);
        result += ".";
        result += std::to_string(m_chanceObjects[i].m_chance);
    }

    return result;
}

int RandTriggerGameObject::getTotalChance()
{
    int total = 0;

    for (size_t i = 0; i < m_chanceObjects.size(); i++)
        total += m_chanceObjects[i].m_chance;

    return total;
}

bool RandTriggerGameObject::init()
{
    return ChanceTriggerGameObject::init("edit_eAdvRandomBtn_001.png");
}

RandTriggerGameObject::~RandTriggerGameObject()
{
}