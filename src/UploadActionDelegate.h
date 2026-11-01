#pragma once

class UploadActionDelegate {
public:
    void uploadActionFailed(int id, int response);
    void uploadActionFinished(int id, int response);
};
