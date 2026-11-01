#pragma once

enum class GJMPErrorCode {
    // Error codes for multiplayer
    Unknown = 0
};

class GJMPDelegate {
public:
    virtual void didUploadMPComment(int id);
    virtual void joinLobbyFailed(int id, GJMPErrorCode errorType);
    virtual void joinLobbyFinished(int id);
    virtual void updateComments();
    virtual ~GJMPDelegate() = default;
};