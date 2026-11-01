#pragma once

class _JNIEnv {
public:
    static __int64 CallStaticBooleanMethod(__int64 int64Val, __int64 int64Val2, __int64 int64Val3, ...);
    static __int64 CallStaticDoubleMethod(__int64 int64Val, __int64 int64Val2, __int64 int64Val3, ...);
    static __int64 CallStaticFloatMethod(__int64 int64Val, __int64 int64Val2, __int64 int64Val3, ...);
    static __int64 CallStaticIntMethod(__int64 int64Val, __int64 int64Val2, __int64 int64Val3, ...);
    static __int64 CallStaticLongMethod(__int64 int64Val, __int64 int64Val2, __int64 int64Val3, ...);
    static __int64 CallStaticObjectMethod(__int64 int64Val, __int64 int64Val2, __int64 int64Val3, ...);
    static __int64 CallStaticVoidMethod(__int64 int64Val, __int64 int64Val2, __int64 int64Val3, ...);
};
