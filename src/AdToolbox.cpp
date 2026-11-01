#include "AdToolbox.h"
#include <jni.h>
#include "cocos2d.h"

void AdToolbox::cacheInterstitial()
{
    cocos2d::JniMethodInfo minfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(minfo,
        "com/customRobTop/BaseRobTopActivity",
        "cacheInterstitial",
        "()V"))
    {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

void AdToolbox::cacheRewardedVideo()
{
    cocos2d::JniMethodInfo minfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(minfo,
        "com/customRobTop/BaseRobTopActivity",
        "cacheRewardedVideo",
        "()V"))
    {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

void AdToolbox::disableBanner()
{
    cocos2d::JniMethodInfo minfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(minfo,
        "com/customRobTop/BaseRobTopActivity",
        "disableBanner",
        "()V"))
    {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

void AdToolbox::enableBanner()
{
    cocos2d::JniMethodInfo minfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(minfo,
        "com/customRobTop/BaseRobTopActivity",
        "enableBanner",
        "()V"))
    {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

void AdToolbox::enableBannerNoRefresh()
{
    cocos2d::JniMethodInfo minfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(minfo,
        "com/customRobTop/BaseRobTopActivity",
        "enableBannerNoRefresh",
        "()V"))
    {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

bool AdToolbox::hasCachedInterstitial()
{
    cocos2d::JniMethodInfo minfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(minfo,
        "com/customRobTop/BaseRobTopActivity",
        "hasCachedInterstitial",
        "()Z"))
    {
        bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
        return result;
    }
    return false;
}

bool AdToolbox::hasCachedRewardedVideo()
{
    cocos2d::JniMethodInfo minfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(minfo,
        "com/customRobTop/BaseRobTopActivity",
        "hasCachedRewardedVideo",
        "()Z"))
    {
        bool result = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
        return result;
    }
    return false;
}

bool AdToolbox::isShowingAd()
{
    return false;
}

void AdToolbox::queueRefreshBanner()
{
    cocos2d::JniMethodInfo minfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(minfo,
        "com/customRobTop/BaseRobTopActivity",
        "queueRefreshBanner",
        "()V"))
    {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

void AdToolbox::setupAds()
{
}

void AdToolbox::showDebug()
{
    cocos2d::JniMethodInfo minfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(minfo,
        "com/customRobTop/BaseRobTopActivity",
        "showAdDebug",
        "()V"))
    {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

void AdToolbox::showInterstitial()
{
    cocos2d::JniMethodInfo minfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(minfo,
        "com/customRobTop/BaseRobTopActivity",
        "showInterstitial",
        "()V"))
    {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

void AdToolbox::showRewardedVideo()
{
    cocos2d::JniMethodInfo minfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(minfo,
        "com/customRobTop/BaseRobTopActivity",
        "showRewardedVideo",
        "()V"))
    {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}