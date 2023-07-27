#ifndef __TIMER__
#define __TIMER__

#include <chrono>
#include <string>

namespace st
{
    class ModuleTimer {
        private:
            float runTime_;
            std::chrono::time_point<std::chrono::high_resolution_clock> initTime_;
            std::string timer_name;

        public:
            ModuleTimer(const std::string& timer_name_) : runTime_{0}, initTime_{std::chrono::high_resolution_clock::now()}, timer_name(timer_name_) {}
            virtual ~ModuleTimer(); 
    };
} // namespace st


#endif