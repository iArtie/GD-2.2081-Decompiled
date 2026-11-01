#pragma once

#include "Platform/cocos2d.h"

class MusicDownloadDelegate;

class MusicDelegateHandler : public cocos2d::CCNode {
public:
    static MusicDelegateHandler* create(MusicDownloadDelegate* delegate);

    bool init(MusicDownloadDelegate* delegate);

    virtual ~MusicDelegateHandler();

protected:
    MusicDownloadDelegate* m_delegate = nullptr;
};