#pragma once

enum AccountError
{
    GenericError = 0,
    AlreadyUsedUsername = 1,
    AlreadyLinkedToDifferentSteamAccount = 2,
    AccountDisabled = 3,
    AlreadyLinkedToDifferentAccount = 4,
    TooShortLessThan3 = 5,
    TooShortLessThan6 = 6
};

class GJAccountLoginDelegate
{
public:
    virtual void loginAccountFailed(AccountError errorType);
    virtual void loginAccountFinished(int accountID, int userID);
    
    virtual ~GJAccountLoginDelegate() {}
};