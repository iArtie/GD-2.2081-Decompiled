#pragma once

#include "CreateGuidelinesLayer.h"
#include "cocos2d.h"

class FindBPMLayer : public CreateGuidelinesLayer {
public:
    void calculateBPM();
    static FindBPMLayer* create(int songID);
    bool init(int songID);
    void onClose(cocos2d::CCObject* sender) override;
    void onInfo(cocos2d::CCObject* sender);
    void onRecord(cocos2d::CCObject* sender) override;
    void playMusic();
    void recordingDidStop() override;
    void registerTouch();
    
    virtual ~FindBPMLayer();

private:
    int m_songID;
    int m_beats;
    int m_beatsPerMinute;
    float m_elapsed;
    float m_startOffset;
    
    cocos2d::CCLabelBMFont* m_bpmLabel;
    cocos2d::CCLabelBMFont* m_infoLabel;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCArray* m_recordingObjects;
};