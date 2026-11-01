#pragma once

class llvm {
public:
    __int64 ConvertCodePointToUTF8(__int64 *int64Ptr, char **charPtr);
    static __int64 ConvertUTF8toWide(int intVal, const void **voidPtr, void **voidPtr2, _QWORD *qwordPtr);
    static __int64 convertUTF16ToUTF8String(char **charPtr, int **intPtr);
    bool hasUTF16ByteOrderMark(const char *charPtr);
};
