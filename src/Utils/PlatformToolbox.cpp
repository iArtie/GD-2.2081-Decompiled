#include "Utils/PlatformToolbox.h"
#include "Platform/cocos2d.h"
#include <cstring>

void PlatformToolbox::activateGameCenter()
{
}

bool PlatformToolbox::copyToClipboard(std::string str)
{
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(
            methodInfo,
            "com/customRobTop/BaseRobTopActivity",
            "copyToClipboard",
            "(Ljava/lang/String;)V"))
    {
        jstring jStr = methodInfo.env->NewStringUTF(str.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jStr);
        methodInfo.env->DeleteLocalRef(jStr);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    return true;
}

bool PlatformToolbox::doesFileExist(std::string path)
{
    cocos2d::CCFileUtils* fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
    return fileUtils->isFileExist(path);
}

void PlatformToolbox::downloadAndSavePromoImage(std::string url, std::string path)
{
}

void PlatformToolbox::gameDidSave()
{
}

std::string PlatformToolbox::getClipboardString()
{
    return "";
}

float PlatformToolbox::getDeviceRefreshRate()
{
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(
            methodInfo,
            "com/customRobTop/BaseRobTopActivity",
            "getDeviceRefreshRate",
            "()F"))
    {
        return methodInfo.env->CallStaticFloatMethod(methodInfo.classID, methodInfo.methodID);
    }
    return 0.0f;
}

cocos2d::CCSize PlatformToolbox::getDisplaySize()
{
    return cocos2d::CCSize(0.0f, 0.0f);
}

std::string PlatformToolbox::getRawPath(const char* path)
{
    return "";
}

std::string PlatformToolbox::getUniqueUserID()
{
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(
            methodInfo,
            "com/customRobTop/BaseRobTopActivity",
            "getUserID",
            "()Ljava/lang/String;"))
    {
        jstring jStr = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        const char* cStr = methodInfo.env->GetStringUTFChars(jStr, nullptr);

        std::string result = cStr;

        methodInfo.env->ReleaseStringUTFChars(jStr, cStr);
        methodInfo.env->DeleteLocalRef(jStr);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);

        return result;
    }
    return "";
}

std::string PlatformToolbox::getUserID()
{
    return getUniqueUserID();
}

void PlatformToolbox::hideCursor()
{
}

bool PlatformToolbox::isControllerConnected()
{
    return false;
}

bool PlatformToolbox::isHD()
{
    return false;
}

bool PlatformToolbox::isLocalPlayerAuthenticated()
{
    return false;
}

bool PlatformToolbox::isLowMemoryDevice()
{
    return false;
}

bool PlatformToolbox::isNetworkAvailable()
{
    return false;
}

bool PlatformToolbox::isSignedInGooglePlay()
{
    return false;
}

void PlatformToolbox::loadAndDecryptFileToString(const char* dirPath, const char* fileName, std::string* str)
{
    *str = "";
}

void PlatformToolbox::logEvent(const char* event)
{
}

void PlatformToolbox::onGameLaunch()
{
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(
            methodInfo,
            "com/customRobTop/BaseRobTopActivity",
            "onGameLaunch",
            "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void PlatformToolbox::onNativePause()
{
}

void PlatformToolbox::onNativeResume()
{
}

void PlatformToolbox::onToggleKeyboard()
{
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(
            methodInfo,
            "com/customRobTop/BaseRobTopActivity",
            "onToggleKeyboard",
            "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}


void PlatformToolbox::openAppPage()
{
}

void PlatformToolbox::platformShutdown()
{
}

void PlatformToolbox::refreshWindow()
{
}

void PlatformToolbox::reportAchievementWithID(const char* key, int percent)
{
    if (percent <= 99)
        return;

    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(
            methodInfo,
            "com/customRobTop/BaseRobTopActivity",
            "unlockAchievement",
            "(Ljava/lang/String;)V"))
    {
        jstring jKey = methodInfo.env->NewStringUTF(key);
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jKey);
        methodInfo.env->DeleteLocalRef(jKey);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void PlatformToolbox::reportLoadingFinished()
{
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(
            methodInfo,
            "com/customRobTop/BaseRobTopActivity",
            "loadingFinished",
            "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void PlatformToolbox::resizeWindow(float width, float height)
{
}

void PlatformToolbox::saveAndEncryptStringToFile(std::string* str, const char* fileName, const char* dirPath)
{
}

void PlatformToolbox::sendMail(const char* title, const char* content, const char* address)
{
}

void PlatformToolbox::setBlockBackButton(bool block)
{
}

void PlatformToolbox::setKeyboardState(bool state)
{
}

bool PlatformToolbox::shouldResumeSound()
{
    return false;
}

void PlatformToolbox::showAchievements()
{
}

void PlatformToolbox::showCursor()
{
}

void PlatformToolbox::signInGooglePlay()
{
}

void PlatformToolbox::signOutGooglePlay()
{
}

cocos2d::CCSprite* PlatformToolbox::spriteFromSavedFile(std::string path)
{
    if (isHD())
    {
        std::string hdPath = path;
        size_t pos = 0;
        while ((pos = hdPath.find(".png", pos)) != std::string::npos)
        {
            hdPath.replace(pos, 4, "-hd.png");
            pos += 7;
        }
        path = hdPath;
    }

    cocos2d::CCSprite* sprite = cocos2d::CCSprite::create(path.c_str());
    if (!sprite)
    {
        std::string fullPath = "/data/data/com.robtopx.geometryjump/" + path;
        sprite = cocos2d::CCSprite::create(fullPath.c_str());
    }
    return sprite;
}

void PlatformToolbox::toggleCPUSleepMode(bool enable)
{
}

void PlatformToolbox::toggleCallGLFinish(bool enable)
{
}

void PlatformToolbox::toggleForceTimer(bool enable)
{
}

void PlatformToolbox::toggleFullScreen(bool fullscreen, bool borderless, bool fix)
{
}

void PlatformToolbox::toggleLockCursor(bool isLocked)
{
}

void PlatformToolbox::toggleMouseControl(bool enable)
{
}

void PlatformToolbox::toggleSmoothFix(bool enable)
{
    cocos2d::CCDirector::sharedDirector()->m_bSmoothFix = enable;
}

void PlatformToolbox::toggleVerticalSync(bool enable)
{
}

void PlatformToolbox::tryShowRateDialog(std::string gameName)
{
}

void PlatformToolbox::updateMouseControl()
{
}

void PlatformToolbox::updateWindowedSize(float width, float height)
{
}