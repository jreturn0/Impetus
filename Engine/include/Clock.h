#pragma once
#include <chrono>

namespace imp {


    class Clock
    {
                using SystemClock = std::chrono::steady_clock;
        using TimePoint = SystemClock::time_point;
        using Duration = std::chrono::nanoseconds;
    public:
        explicit Clock(double currentTimeSeconds, unsigned short variableFpsCap, unsigned short fixedFpsCap, double maxAccumulatedFrames = 3.5);
        Clock(const Clock&) = delete;
        Clock(Clock&&) = delete;
        Clock& operator=(const Clock&) = delete;
        Clock& operator=(Clock&&) = delete;

        void update() noexcept;
        bool fixedUpdate() noexcept;

        // Status queries

        inline bool isFixed() const noexcept { return m_accumulatedTime >= m_fixedTargetFrameDuration; }
        inline bool isFixedFrame() const noexcept { return m_fixedFrame; }

        // Getters

        inline double getDelta() const noexcept { return m_deltaTime; }
        inline double getElapsed() const noexcept { return m_elapsedTime; }
        inline double getFixedStep() const noexcept { return m_fixedTargetFrameDuration; }
        inline double getAccumulatedTime() const noexcept { return m_accumulatedTime; }

        inline double getTargetFps() const noexcept { return static_cast<double>(m_targetFps); }
        inline double getTargetFrameDuration() const noexcept { return m_targetFrameDuration; }

        inline double getFixedTargetFps() const noexcept { return static_cast<double>(m_fixedTargetFps); }
        inline double getFixedTargetFrameDuration() const noexcept { return m_fixedTargetFrameDuration; }

        inline double getMaxAccumulatedFrames() const noexcept { return m_maxAccumulatedFrames; }
        inline double getMaxAccumulatedTime() const noexcept { return m_maxAccumulatedTime; }

        inline double getLastFrameTime() const noexcept { return m_lastFrameTime; }
        inline double getNextFrameTime() const noexcept { return m_nextFrameTime; }


        // Setters

        void setTargetFps(double fps) noexcept
        {
            m_targetFps = static_cast<uint64_t>(fps);
            m_targetFrameDuration = (fps > 0.0) ? (1.0 / fps) : 0.0;
        }

        void setFixedTargetFps(double fps) noexcept
        {
            m_fixedTargetFps = static_cast<uint64_t>(fps);
            m_fixedTargetFrameDuration = (fps > 0.0) ? (1.0 / fps) : 0.0;
            m_maxAccumulatedTime = m_fixedTargetFrameDuration * m_maxAccumulatedFrames;
        }

        void setMaxAccumulatedFrames(double frames) noexcept
        {
            m_maxAccumulatedFrames = std::max(1.0, frames);
            m_maxAccumulatedTime = m_fixedTargetFrameDuration * m_maxAccumulatedFrames;
        }

    private:


        // Variable targets
        uint64_t m_targetFps;
        double m_targetFrameDuration;

        // Fixed targets
        uint64_t m_fixedTargetFps;
        double m_fixedTargetFrameDuration;

        // Accumulation
        double m_maxAccumulatedFrames;
        double m_maxAccumulatedTime;
        double m_accumulatedTime;
        bool m_fixedFrame;

        // Timing
        double m_lastFrameTime;
        double m_nextFrameTime;
        double m_elapsedTime;
        double m_deltaTime;
    };


}