#include "entityIdHolder.h"
#include <cassert>

std::uint64_t EntityIdHolder::getEntityIdAndIncrement() {
    std::uint64_t id = idCounter;
    idCounter++;

    assert(id && id < UINT64_MAX-1 && "You somehow managed to bug out 18 quintillion ids, I'm impressed.");
    //Jokes aside, this is to avoid loading corrupted data

    return id;
}
