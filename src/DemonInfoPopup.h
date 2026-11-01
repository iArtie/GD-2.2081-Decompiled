#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include <string>

class DemonInfoPopup : public FLAlertLayer {
public:
    static DemonInfoPopup* create(int easyClassic, int mediumClassic, int hardClassic, int insaneClassic, int extremeClassic,
                                   int easyPlatformer, int mediumPlatformer, int hardPlatformer, int insanePlatformer, int extremePlatformer,
                                   int demons, int weeklyDemons);
    static DemonInfoPopup* createFromString(std::string str);
    
    bool init(int easyClassic, int mediumClassic, int hardClassic, int insaneClassic, int extremeClassic,
              int easyPlatformer, int mediumPlatformer, int hardPlatformer, int insanePlatformer, int extremePlatformer,
              int demons, int weeklyDemons);
    
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    
    virtual ~DemonInfoPopup();

protected:
    int m_easyClassic;
    int m_mediumClassic;
    int m_hardClassic;
    int m_insaneClassic;
    int m_extremeClassic;
    int m_easyPlatformer;
    int m_mediumPlatformer;
    int m_hardPlatformer;
    int m_insanePlatformer;
    int m_extremePlatformer;
    int m_demons;
    int m_weeklyDemons;
};