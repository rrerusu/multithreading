#include "HazardPointers.h"

HazardPointer hazardPointers[maxHazardPointers];

HazardPointerOwner::HazardPointerOwner() : hp(nullptr) {
    for(unsigned i = 0; i < maxHazardPointers; ++i) {
        std::thread::id oldId;

        if(hazardPointers[i].id.compare_exchange_weak(oldId, std::this_thread::get_id())) {
            hp = & hazardPointers[i];
            break;
        }

        if(!hp)
            throw std::runtime_error("No hazard pointers available");
    }
}

std::atomic<void * > & HazardPointerOwner::getPointer() {
    return hp->pointer;
}

HazardPointerOwner::~HazardPointerOwner() {
    hp->pointer.store(nullptr);
    hp->id.store(std::thread::id());
}

std::atomic<void * > & getHazardPointerForCurrentThread() {
    thread_local static HazardPointerOwner hazard;
    return hazard.getPointer();
}

bool outstandingHazardPointersFor(void * pointer) {
    for(unsigned i = 0; i < maxHazardPointers; ++i) {
        if(hazardPointers[i].pointer.load() == pointer)
            return true;
    }
    return false;
}