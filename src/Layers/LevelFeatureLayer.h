#pragma once

#include "Layers/FLAlertLayer.h"
#include "Platform/cocos2d.h"

class LevelFeatureLayer : public FLAlertLayer {
public:
    static LevelFeatureLayer* create(int levelID);
    bool init(int levelID);
    virtual void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onDown(cocos2d::CCObject* sender);
    void onDown2(cocos2d::CCObject* sender);
    void onRemoveValues(cocos2d::CCObject* sender);
    void onSetEpicOnly(cocos2d::CCObject* sender);
    void onSetFeatured(cocos2d::CCObject* sender);
    void onToggleEpic(cocos2d::CCObject* sender);
    void onUp(cocos2d::CCObject* sender);
    void onUp2(cocos2d::CCObject* sender);
    void updateStars();
    virtual ~LevelFeatureLayer() override;

private:
    cocos2d::CCSprite* m_epicSprite = nullptr;
    bool m_epicOnly = false;
    int m_levelID = 0;
    int m_featurePosition = 0;
    cocos2d::CCLabelBMFont* m_featureLabel = nullptr;
};