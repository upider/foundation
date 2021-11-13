#include "concurrency/mutex/null_mutex.hpp"

NullMutex::NullMutex() {}
NullMutex::~NullMutex() {}
void NullMutex::lock() {}
void NullMutex::unlock() {}