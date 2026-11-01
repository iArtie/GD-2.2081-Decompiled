#pragma once

#include "Platform/cocos2d.h"
#include <string>

class GJSmartBlockPreview : public cocos2d::CCNode {
public:
    void addChance(int count, int total);
    void addCount(int count);
    static GJSmartBlockPreview* create(const std::string& key);
    void createInfoLabel();
    bool init(const std::string& key) override;
    void toggleSelectItem(bool selected);
    virtual ~GJSmartBlockPreview();

private:
    std::string m_prefabKey;
    cocos2d::CCLabelBMFont* m_infoLabel;
    bool m_selected;
    cocos2d::CCSprite* m_selectSprite;
};