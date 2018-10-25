#pragma once

#include <chrono>

struct Timer {
    Timer() {
        m_start = std::chrono::steady_clock::now();
    }

    auto seconds_elapsed() {
        auto end = std::chrono::steady_clock::now();
        auto elapsed = end - m_start;
        return std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> m_start;

};

