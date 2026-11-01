#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"

class SelectListIconDelegate;

class SelectListIconLayer : public FLAlertLayer {
public:
    static SelectListIconLayer* create(int difficulty);
    bool init(int difficulty) override;

    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender) override;
    void onSelect(cocos2d::CCObject* sender);

    ~SelectListIconLayer();

protected:
    SelectListIconDelegate* m_delegate = nullptr;
    int m_currentDifficulty = 0;
    cocos2d::CCArray* m_difficulties = nullptr;
};