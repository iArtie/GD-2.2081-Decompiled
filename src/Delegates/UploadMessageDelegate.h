#pragma once

class UploadMessageDelegate {
public:
    void uploadMessageFailed(int accountID);
    void uploadMessageFinished(int accountID);
};