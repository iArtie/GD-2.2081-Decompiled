#pragma once

#include "cocos2d.h"
#include <string>

class GJBaseGameLayer;

class LabelGameObject : public GameObject {
public:
    void addMainSpriteToParent(bool reorder) override;
    static LabelGameObject* create();
    void createLabel(const std::string& text);
    std::string getSaveString(GJBaseGameLayer* layer) override;
    int getTextKerning();
    bool init() override;
    void queueUpdateLabel(const std::string& text);
    void removeLabel();
    void resetObject() override;
    void setObjectColor(const cocos2d::ccColor3B* color) override;
    void setOpacity(unsigned char opacity) override;
    void setupCustomSprites(const std::string& frameName) override;
    void unlockLabelColor();
    void updateLabel(float value);
    void updateLabelAlign(int alignment);
    void updateLabelIfDirty();
    void updatePreviewLabel();
    void updateTextKerning(int kerning);
    virtual ~LabelGameObject();

    int m_kerning;
    cocos2d::CCLabelBMFont* m_label;
    bool m_bUnkBool2;
    bool m_labelColorLocked;
    cocos2d::ccColor3B _realColor;
    bool m_updateLabel;
    unsigned char _realOpacity;
    bool m_shouldBlendBase;
    int m_alignment;
    std::string m_labelString;
    bool m_labelDirty;
    int m_shownSpecial;
    bool m_isTimeCounter;
    int m_itemID;
    int m_showSecondsOnly;
};