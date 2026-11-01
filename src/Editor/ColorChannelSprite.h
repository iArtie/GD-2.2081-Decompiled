#pragma once

#include "Platform/cocos2d.h"

class ColorAction;

class ColorChannelSprite : public cocos2d::CCSprite {
public:
    static ColorChannelSprite* create();

    bool init();

    void updateBlending(bool enabled);
    void updateCopyLabel(int channelID, bool copyColorLoop);
    void updateOpacity(float alpha);
    void updateValues(ColorAction* action);

    ~ColorChannelSprite();

private:
    cocos2d::CCLabelBMFont* m_copyLabel;
    cocos2d::CCLabelBMFont* m_opacityLabel;
    cocos2d::CCLabelBMFont* m_blendingLabel;
};