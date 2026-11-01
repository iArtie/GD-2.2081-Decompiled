#pragma once

class GJLevelList;

class ListUploadDelegate {
public:
    void listUploadFailed(GJLevelList* list, int response);
    void listUploadFinished(GJLevelList* list);
};