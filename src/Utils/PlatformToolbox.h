#pragma once

#include "Platform/cocos2d.h"
#include <string>

class PlatformToolbox {
public:
    static void activateGameCenter();
    static bool copyToClipboard(std::string str);
    static bool doesFileExist(std::string path);
    static void downloadAndSavePromoImage(std::string url, std::string path);
    static void gameDidSave();
    static std::string getClipboardString();
    static float getDeviceRefreshRate();
    static cocos2d::CCSize getDisplaySize();
    static std::string getRawPath(const char* path);
    static std::string getUniqueUserID();
    static std::string getUserID();
    static void hideCursor();
    static bool isControllerConnected();
    static bool isHD();
    static bool isLocalPlayerAuthenticated();
    static bool isLowMemoryDevice();
    static bool isNetworkAvailable();
    static bool isSignedInGooglePlay();
    static void loadAndDecryptFileToString(const char* dirPath, const char* fileName, std::string* str);
    static void logEvent(const char* event);
    static void onGameLaunch();
    static void onNativePause();
    static void onNativeResume();
    static void onToggleKeyboard();
    static void openAppPage();
    static void platformShutdown();
    static void refreshWindow();
    static void reportAchievementWithID(const char* key, int percent);
    static void reportLoadingFinished();
    static void resizeWindow(float width, float height);
    static void saveAndEncryptStringToFile(std::string* str, const char* fileName, const char* dirPath);
    static void sendMail(const char* title, const char* content, const char* address);
    static void setBlockBackButton(bool block);
    static void setKeyboardState(bool state);
    static bool shouldResumeSound();
    static void showAchievements();
    static void showCursor();
    static void signInGooglePlay();
    static void signOutGooglePlay();
    static cocos2d::CCSprite* spriteFromSavedFile(std::string path);
    static void toggleCPUSleepMode(bool enable);
    static void toggleCallGLFinish(bool enable);
    static void toggleForceTimer(bool enable);
    static void toggleFullScreen(bool fullscreen, bool borderless, bool fix);
    static void toggleLockCursor(bool isLocked);
    static void toggleMouseControl(bool enable);
    static void toggleSmoothFix(bool enable);
    static void toggleVerticalSync(bool enable);
    static void tryShowRateDialog(std::string gameName);
    static void updateMouseControl();
    static void updateWindowedSize(float width, float height);
};