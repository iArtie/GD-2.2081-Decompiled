#pragma once

#include "Platform/cocos2d.h"

class NumberInputDelegate;
class ButtonSprite;

class NumberInputLayer : public FLAlertLayer {
public:
    static NumberInputLayer* create();

    bool init() override;
    void deleteLast();
    void inputNumber(int num);
    void updateNumberState();

    virtual void keyBackClicked() override;
    virtual void registerWithTouchDispatcher() override;

    void onClose(cocos2d::CCObject* sender);
    void onDone(cocos2d::CCObject* sender);
    void onNumber(cocos2d::CCObject* sender);

    virtual ~NumberInputLayer();

protected:
    int m_minimum = 4;
    std::string m_inputString;
    int m_maximum = 4;
    cocos2d::CCMenuItemSpriteExtra* m_okButton = nullptr;
    cocos2d::CCLabelBMFont* m_inputLabel = nullptr;
    NumberInputDelegate* m_delegate = nullptr;
};