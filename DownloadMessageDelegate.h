#pragma once

class GJUserMessage;

class DownloadMessageDelegate {
public:
    virtual void downloadMessageFailed(int id);
    virtual void downloadMessageFinished(GJUserMessage* message);
    virtual ~DownloadMessageDelegate() {}
};