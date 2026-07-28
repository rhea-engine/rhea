#pragma once

#include <chrono>
#include <Psio/Log.h>

class ScopedTimer {
public:
    ScopedTimer(std::string name, float threshold = 500.0f) : m_name(name), m_threshold(threshold), m_start(std::chrono::steady_clock::now()) {}

    ~ScopedTimer() {
        auto end = std::chrono::steady_clock::now();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - m_start).count();

        if (us > m_threshold) {
            LOG(LOG_INFO) << m_name << ": " << us / 1000.0 << " ms";
        }
    }

private:
    std::string m_name;
    float m_threshold;
    std::chrono::steady_clock::time_point m_start;
};