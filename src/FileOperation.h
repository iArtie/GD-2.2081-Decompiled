#pragma once

#include <string>

class FileOperation {
public:
    static std::string getFilePath();
    static void readFile();
    static void saveFile();
};