#pragma once

#include "Platform/cocos2d.h"
#include "Layers/GJOptionsLayer.h"
#include "Delegates/FLAlertLayerProtocol.h"
#include "Delegates/TextInputDelegate.h"
#include "Delegates/SliderDelegate.h"
#include "Editor/ColorSelectDelegate.h"
#include "Editor/HSVWidgetDelegate.h"

class EditorOptionsLayer : public GJOptionsLayer {
public:
    static EditorOptionsLayer* create();
    bool init();
    
    void onButtonRows(cocos2d::CCObject* sender);
    void onButtonsPerRow(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender) override;
    void setupOptions() override;
    
    virtual ~EditorOptionsLayer();

protected:
    int m_buttonsPerRow;
    int m_buttonRows;
    cocos2d::CCLabelBMFont* m_buttonsPerRowLabel;
    cocos2d::CCLabelBMFont* m_buttonRowsLabel;
    float m_gap;
    float m_maxLabelScale;
    int m_togglesPerPage;
    float m_offset;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
};