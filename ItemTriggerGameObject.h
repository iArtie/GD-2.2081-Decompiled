#pragma once

#include <string>

class GJBaseGameLayer;

class ItemTriggerGameObject : public EffectGameObject {
public:
    static ItemTriggerGameObject* create(const char* frame);
    bool init(const char* frame);
    void customSetup() override;
    std::string getSaveString(GJBaseGameLayer* layer) override;
    virtual ~ItemTriggerGameObject();

    int m_objectID;
    int m_resultType3;
    int m_resultType1;
    int m_resultType2;
    int m_itemID;
    int m_itemID2;
    int m_targetGroupID;
    int m_centerGroupID;
    int m_item1Mode;
    int m_item2Mode;
    float m_mod1;
    float m_mod2;
    float m_tolerance;
    int m_roundType1;
    int m_roundType2;
    int m_signType1;
    int m_signType2;
    int m_persistent;
    int m_targetAll;
    int m_reset;
    int m_timer;
    int m_targetItemMode;
};