#pragma once

#include "GJAccountBackupDelegate.h"

class GJAccountSyncDelegate
{
public:
    virtual void syncAccountFailed(BackupAccountError errorType, int response);
    virtual void syncAccountFinished();
    
    virtual ~GJAccountSyncDelegate() {}
};