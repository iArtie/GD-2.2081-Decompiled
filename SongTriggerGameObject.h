#pragma once

#include "cocos2d.h"
#include "SFXTriggerGameObject.h"
#include <string>

class GJBaseGameLayer;

class SongTriggerGameObject : public SFXTriggerGameObject {
public:
    static SongTriggerGameObject* create(const char* frame);
    bool init(const char* frame) override;
    std::string getSaveString(GJBaseGameLayer* layer) override;
    virtual ~SongTriggerGameObject();

protected:
    int m_prep = 0;
    int m_loadPrep = 0;
    int m_songChannel = 0;
};