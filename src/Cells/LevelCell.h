#pragma once

#include "Platform/cocos2d.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCMenuItemToggler.h"
#include "Models/GJGameLevel.h"
#include "Cells/TableViewCell.h"
#include "Sprites/GJDifficultySprite.h"
#include "Sprites/ButtonSprite.h"

class LevelCell : public TableViewCell {
public:
    static LevelCell* create(float width, float height);

    virtual bool init() override;
    virtual void draw() override;
    virtual ~LevelCell() override;

    void loadFromLevel(GJGameLevel* level);
    void loadCustomLevelCell();
    void loadLocalLevelCell();
    void onClick(cocos2d::CCObject* sender);
    void onToggle(cocos2d::CCObject* sender);
    void onViewProfile(cocos2d::CCObject* sender);
    void updateBGColor(int index);
    void updateCellMode(int mode);
    void updateToggle();

private:
    GJGameLevel* m_level = nullptr;
    CCMenuItemSpriteExtra* m_button = nullptr;
    cocos2d::CCMenu* m_mainMenu = nullptr;
    CCMenuItemToggler* m_toggler = nullptr;
    cocos2d::CCLayerColor* m_backgroundLayer = nullptr;
    int m_cellMode = 0;
    int m_listType = 0;
    float m_width = 0.0f;
    float m_height = 0.0f;
    bool m_compactView = false;
    bool m_cellDrawn = false;
    cocos2d::CCPoint m_point;
    float m_unkFloat = 0.0f;
};