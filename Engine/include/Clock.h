#pragma once
#include <chrono>

namespace imp {
    class Clock
    {
    public:
        explicit Clock(unsigned short variableFpsCap, unsigned short fixedFpsCap, unsigned short maxAccumulated = 4);
        Clock(const Clock&) = delete;
        Clock(Clock&&) = delete;
        Clock& operator=(const Clock&) = delete;
        Clock& operator=(Clock&&) = delete;

        void update() noexcept;
        bool fixedUpdate() noexcept;
        inline bool isFixed() const noexcept { return m_accumulatedTime >= m_fixedTargetFrameDuration; }

        // Getters

        inline bool isFixedFrame() const noexcept { return m_fixedFrame; }
        inline double getDelta() const noexcept { return std::chrono::duration<double>(m_deltaTime).count(); }
        inline double getElapsed() const noexcept { return std::chrono::duration<double>(m_elapsedTime).count(); }
        inline double getFixedStep() const noexcept { return std::chrono::duration<double>(m_fixedTargetFrameDuration).count(); }
        inline double getAccumulatedTime() const noexcept { return std::chrono::duration<double>(m_accumulatedTime).count(); }


    private:
        using SystemClock = std::chrono::steady_clock;
        using TimePoint = SystemClock::time_point;
        using Duration = std::chrono::nanoseconds;

        // Variable targets
        uint64_t m_targetFps;
        Duration m_targetFrameDuration;

        // Fixed targets
        uint64_t m_fixedTargetFps;
        Duration m_fixedTargetFrameDuration;

        // Accumulation
        Duration m_maxAccumulatedTime;
        Duration m_accumulatedTime;
        bool m_fixedFrame;

        // Timing
        TimePoint m_lastFrameTime;
        TimePoint m_nextFrameTime;
        Duration m_elapsedTime;
        Duration m_deltaTime;
    };


}