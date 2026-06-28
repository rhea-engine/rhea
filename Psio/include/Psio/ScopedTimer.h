#pragma once

#include <chrono>
#include <Psio/Log.h>

class ScopedTimer {
public:
    ScopedTimer(const char* name, float threshhold = 500.0f) : m_name(name), m_threshhold(threshhold), m_start(std::chrono::high_resolution_clock::now()) {}

    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - m_start).count();

        if (us > m_threshhold) {
            LOG(LOG_INFO) << m_name << ": " << us / 1000.0 << " ms";
        }
    }

private:
    const char* m_name;
    float m_threshhold;
    std::chrono::high_resolution_clock::time_point m_start;
};