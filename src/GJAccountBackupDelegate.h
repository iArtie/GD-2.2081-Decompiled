#pragma once

enum BackupAccountError
{
    BackupErrorGeneric = 0,
    BackupErrorLoginFailed = 1,
    BackupErrorFailed = 2
};

class GJAccountBackupDelegate
{
public:
    virtual void backupAccountFinished();
    virtual void backupAccountFailed(BackupAccountError errorType, int response);
    
    virtual ~GJAccountBackupDelegate() {}
};