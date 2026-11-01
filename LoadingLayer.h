#pragma once

#include "cocos2d.h"

class TextArea;

class LoadingLayer : public cocos2d::CCLayer {
public:
    static LoadingLayer* create(bool refresh);
    static cocos2d::CCScene* scene(bool refresh);

    bool init(bool refresh) override;
    const char* getLoadingString();
    void loadAssets();
    void loadingFinished();
    void updateProgress(int progress);

    ~LoadingLayer();

protected:
    bool m_fromRefresh = false;
    float m_sliderGrooveXPos = 0.0f;
    cocos2d::CCSprite* m_sliderBar = nullptr;
    float m_sliderGrooveHeight = 0.0f;
    int m_loadStep = 0;
    cocos2d::CCLabelBMFont* m_caption = nullptr;
    TextArea* m_textArea = nullptr;
};