#include "FileOperation.h"
#include "cocos2d.h"
#include <cstdio>

std::string FileOperation::getFilePath() {
    std::string path = "file.txt"; // Default filename from decompiled code (byte_F288A0)
    
    cocos2d::CCFileUtils* fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
    std::string fullPath = fileUtils->getWritablePath() + path;
    
    return fullPath;
}

void FileOperation::readFile() {
    std::string filePath = getFilePath();
    FILE* file = fopen(filePath.c_str(), "r");
    
    if (file) {
        char buffer[50];
        fgets(buffer, sizeof(buffer), file);
        fclose(file);
    }
}

void FileOperation::saveFile() {
    std::string filePath = getFilePath();
    FILE* file = fopen(filePath.c_str(), "w");
    
    if (file) {
        fwrite("file example", 1, 12, file);
        fclose(file);
    }
}