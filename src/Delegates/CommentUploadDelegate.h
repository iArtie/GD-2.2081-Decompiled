#pragma once

enum class CommentError {
    // Error values will be defined elsewhere
};

class CommentUploadDelegate {
public:
    virtual ~CommentUploadDelegate() {}

    virtual void commentDeleteFailed(int id, int parentID);
    virtual void commentUploadFailed(int parentID, CommentError errorType);
    virtual void commentUploadFinished(int parentID);
};