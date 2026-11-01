#include "Delegates/CommentUploadDelegate.h"

void CommentUploadDelegate::commentDeleteFailed(int id, int parentID) {
    // Empty implementation - to be overridden by subclasses
}

void CommentUploadDelegate::commentUploadFailed(int parentID, CommentError errorType) {
    // Empty implementation - to be overridden by subclasses
}

void CommentUploadDelegate::commentUploadFinished(int parentID) {
    // Empty implementation - to be overridden by subclasses
}