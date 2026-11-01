#pragma once

class PlatformDownloadDelegate {
public:
    void downloadFailed(const char* key);
    void downloadFinished(const char* key);
};