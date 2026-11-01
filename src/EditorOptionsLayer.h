#pragma once

#include "cocos2d.h"
#include "GJOptionsLayer.h"
#include "FLAlertLayerProtocol.h"
#include "TextInputDelegate.h"
#include "SliderDelegate.h"
#include "ColorSelectDelegate.h"
#include "HSVWidgetDelegate.h"

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