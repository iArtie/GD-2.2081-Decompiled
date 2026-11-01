#pragma once

class LevelDownloadDelegate {
public:
    void levelDownloadFailed(int response);
    void levelDownloadFinished(GJGameLevel *level);
};
