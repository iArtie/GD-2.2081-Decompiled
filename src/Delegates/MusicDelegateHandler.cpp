#include "Delegates/MusicDelegateHandler.h"
#include "Delegates/MusicDownloadDelegate.h"

MusicDelegateHandler* MusicDelegateHandler::create(MusicDownloadDelegate* delegate)
{
    MusicDelegateHandler* ret = new MusicDelegateHandler();
    if (ret->init(delegate)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool MusicDelegateHandler::init(MusicDownloadDelegate* delegate)
{
    if (!cocos2d::CCNode::init())
        return false;
    m_delegate = delegate;
    return true;
}

MusicDelegateHandler::~MusicDelegateHandler()
{
}