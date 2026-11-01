#pragma once

#include "cocos2d.h"
#include "DialogDelegate.h"
#include "TextAreaDelegate.h"
#include "DialogObject.h"
#include "GJEnums.h"

class DialogLayer : public cocos2d::CCLayerColor, public TextAreaDelegate {
public:
    static DialogLayer* create(DialogObject* object, int background);
    static DialogLayer* createWithObjects(cocos2d::CCArray* objects, int background);
    static DialogLayer* createDialogLayer(DialogObject* object, cocos2d::CCArray* objects, int background);
    
    bool init(DialogObject* object, cocos2d::CCArray* objects, int background);
    
    void addToMainScene();
    void animateIn(DialogAnimationType type);
    void animateInDialog();
    void animateInRandomSide();
    void displayDialogObject(DialogObject* object);
    void displayNextObject();
    void fadeInTextFinished(TextArea* textArea) override;
    void finishCurrentAnimation();
    void handleDialogTap();
    void keyDown(cocos2d::enumKeyCodes key, double timestamp) override;
    void onClose();
    void registerWithTouchDispatcher() override;
    void updateChatPlacement(DialogChatPlacement placement);
    void updateNavButtonFrame();
    
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    
    void setDelegate(DialogDelegate* delegate) { m_delegate = delegate; }
    
    virtual ~DialogLayer();

protected:
    DialogDelegate* m_delegate = nullptr;
    cocos2d::CCLayer* m_mainLayer = nullptr;
    cocos2d::CCArray* m_dialogObjects = nullptr;
    cocos2d::CCSprite* m_characterSprite = nullptr;
    cocos2d::CCSprite* m_navButtonSprite = nullptr;
    cocos2d::CCLabelBMFont* m_characterLabel = nullptr;
    TextArea* m_textArea = nullptr;
    float m_animateTime = 0.0f;
    bool m_animating = false;
    bool m_skippable = false;
    bool m_handleTap = false;
    bool m_noRemove = false;
    int m_touchID = -1;
    DialogAnimationType m_animationType = DialogAnimationType::FromCenter;
};