#include <mutex>
#include "HierarchicalMutex.h"

HierarchicalMutex highLevelMutex(10000);
HierarchicalMutex lowLevelMutex(5000);

int doLowlevelStuff();

int lowLevelFunc() {
    std::lock_guard<HierarchicalMutex> lowLevelLock(lowLevelMutex);
    return doLowlevelStuff();
}

void highLevelStuff(int param);

void highLevelFunc() {
    std::lock_guard<HierarchicalMutex> highLevelLock(highLevelMutex);
    highLevelStuff(lowLevelFunc());
}

void threadA() {
    highLevelFunc();
}

HierarchicalMutex otherMutex(100);

void doOtherStuff();

void otherStuff() {
    highLevelFunc();
    doOtherStuff();
}

void threadB() {
    std::lock_guard<HierarchicalMutex> otherLevelLock(otherMutex);
    otherStuff;
}