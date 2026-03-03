#pragma once

#include <atomic>
#include <thread>
#include <stdexcept>

constexpr unsigned maxHazardPointers = 100;

struct HazardPointer {
    std::atomic<std::thread::id> id;
    std::atomic<void * > pointer;
};

extern HazardPointer hazardPointers[maxHazardPointers];

class HazardPointerOwner {
    private:
        HazardPointer * hp;
    public:
        HazardPointerOwner(HazardPointerOwner const & ) = delete;
        HazardPointerOwner operator=(HazardPointerOwner const & ) = delete;
        HazardPointerOwner();
        ~HazardPointerOwner();

        std::atomic<void * > & getPointer();
};

std::atomic<void * > & getHazardPointerForCurrentThread();
bool outstandingHazardPointersFor(void * );