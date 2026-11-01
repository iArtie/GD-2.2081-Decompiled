#pragma once

#include "cocos2d.h"
#include "EnhancedGameObject.h"
#include <string>

class GJBaseGameLayer;

class SpecialAnimGameObject : public EnhancedGameObject {
public:
    static SpecialAnimGameObject* create(const char* frame);
    bool init(const char* frame) override;
    std::string getSaveString(GJBaseGameLayer* layer) override;
    void resetObject() override;
    void updateMainColor(const cocos2d::ccColor3B* color) override;
    void updateSecondaryColor(const cocos2d::ccColor3B* color) override;
    void updateSyncedAnimation(float totalTime, int frameIndex) override;
    virtual ~SpecialAnimGameObject();

protected:
    bool m_skipMainColorUpdate = false;
    bool m_skipSecondaryColorUpdate = false;
    bool m_bUnkBool2 = false;
    bool m_disableAnimShine = false;
    int m_maybeAnimationVariableXInt = 0;
};