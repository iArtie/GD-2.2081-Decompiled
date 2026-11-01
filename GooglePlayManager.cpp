#include "GooglePlayManager.h"
#include "GooglePlayDelegate.h"

GooglePlayManager* GooglePlayManager::s_sharedInstance = nullptr;

bool GooglePlayManager::init() {
    return true;
}

GooglePlayManager* GooglePlayManager::sharedState() {
    if (!s_sharedInstance) {
        s_sharedInstance = new GooglePlayManager();
        s_sharedInstance->init();
    }
    return s_sharedInstance;
}

void GooglePlayManager::googlePlaySignedIn() {
    if (m_delegate1) {
        m_delegate1->googlePlaySignedIn();
    }
    if (m_delegate2) {
        m_delegate2->googlePlaySignedIn();
    }
}

GooglePlayManager::~GooglePlayManager() {
}