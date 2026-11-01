#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "CustomSongDelegate.h"
#include "CCTextInputNode.h"
#include "AudioGuidelinesType.h"

class CreateGuidelinesLayer : public FLAlertLayer, public cocos2d::CCTouchDelegate {
public:
    static CreateGuidelinesLayer* create(CustomSongDelegate* delegate, AudioGuidelinesType type);
    bool init(CustomSongDelegate* delegate, AudioGuidelinesType type);
    
    void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) override;
    void keyBackClicked() override;
    void keyUp(cocos2d::enumKeyCodes key, double timestamp) override;
    void registerWithTouchDispatcher() override;
    void update(float dt) override;
    
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    
    void doClearGuidelines();
    void onClearGuidelines(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onInfo(cocos2d::CCObject* sender);
    void onRecord(cocos2d::CCObject* sender);
    void onStop(cocos2d::CCObject* sender);
    void playMusic();
    void recordingDidStop();
    void registerTouch();
    void toggleItems(bool visible);
    
    virtual ~CreateGuidelinesLayer();

protected:
    CustomSongDelegate* m_delegate;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCLabelBMFont* m_infoLabel;
    cocos2d::CCSprite* m_circleSprite;
    CCTextInputNode* m_offsetInput;
    
    cocos2d::CCArray* m_nonRecordingObjects;
    cocos2d::CCArray* m_recordingObjects;
    
    std::string m_guidelineString;
    float m_elapsed;
    int m_guidelines;
    bool m_recording;
    
    AudioGuidelinesType m_type;
};

enum AudioGuidelinesType {
    BPMFinder = 0,
    GuidelineCreator = 1
};