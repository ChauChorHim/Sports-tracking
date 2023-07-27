#include "timer.h"
#include <iostream>

namespace st
{
    ModuleTimer::~ModuleTimer() {
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> duration {endTime - initTime_};
        runTime_ = duration.count() * 1000.0f;
        std::cout << timer_name << " cost time: " << runTime_ << " ms\n";
    }
} // namespace st
