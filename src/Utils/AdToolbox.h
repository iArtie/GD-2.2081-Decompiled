#pragma once

class AdToolbox {
public:
    static void cacheInterstitial();
    static void cacheRewardedVideo();
    static void disableBanner();
    static void enableBanner();
    static void enableBannerNoRefresh();
    static bool hasCachedInterstitial();
    static bool hasCachedRewardedVideo();
    static bool isShowingAd();
    static void queueRefreshBanner();
    static void setupAds();
    static void showDebug();
    static void showInterstitial();
    static void showRewardedVideo();
};