#pragma once

#include "cocos2d.h"
#include "GJEnums.h"

class SelectArtDelegate;
class ButtonSprite;

class SelectArtLayer : public cocos2d::CCLayerColor {
public:
    static SelectArtLayer* create(SelectArtType type, int index);
    bool init(SelectArtType type, int index) override;

    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onSelectCustom(cocos2d::CCObject* sender);
    void selectArt(cocos2d::CCObject* sender);
    void updateSelectedCustom(int idx);

    ~SelectArtLayer();

protected:
    SelectArtDelegate* m_delegate = nullptr;
    int m_art = 0;
    SelectArtType m_type = SelectArtType::Ground;
    cocos2d::CCArray* m_artSprites = nullptr;
    int m_line = 0;
    cocos2d::CCArray* m_lineSprites = nullptr;
    cocos2d::CCLayer* m_mainLayer = nullptr;
    cocos2d::CCMenu* m_buttonMenu = nullptr;
    bool m_noElasticity = false;
};