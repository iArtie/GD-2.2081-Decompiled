#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"

class LevelEditorLayer;

class SelectFontLayer : public FLAlertLayer {
public:
    static SelectFontLayer* create(LevelEditorLayer* editorLayer);
    bool init(LevelEditorLayer* layer) override;

    void keyBackClicked() override;
    void onChangeFont(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender) override;
    void updateFontLabel();

    ~SelectFontLayer();

protected:
    LevelEditorLayer* m_editorLayer = nullptr;
    int m_font = -1;
    cocos2d::CCLabelBMFont* m_fontLabel = nullptr;
};