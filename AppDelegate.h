#pragma once

#include "cocos2d.h"
#include <string>

class AppDelegate {
public:
    void applicationDidEnterBackground();
    bool applicationDidFinishLaunching();
    void applicationWillBecomeActive();
    void applicationWillEnterForeground();
    float bgScale();
    void checkSound();
    void hideLoadingCircle();
    void loadingIsFinished();
    bool musicTest();
    void pauseGame();
    void pauseSound();
    void platformShutdown();
    void resumeSound();
    void setIdleTimerDisabled(bool disabled);
    void setupGLView();
    void showLoadingCircle(bool unk1, bool unk2, bool unk3);
    void trySaveGame(bool force);
    void willSwitchToScene(cocos2d::CCScene* scene);
    ~AppDelegate();

private:
    float m_lastSaveTime;
    bool m_glViewSetup;
    std::string m_startupScriptFilename;
    bool m_gamePaused;
    bool m_isIphoneX;
    bool m_isLowMemoryDevice;
};