#pragma once

class GJUserScore;

class UserInfoDelegate {
public:
    void getUserInfoFailed(int id);
    void getUserInfoFinished(GJUserScore* score);
    void userInfoChanged(GJUserScore* score);
};