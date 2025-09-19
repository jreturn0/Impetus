#include "Clock.h"
#include "Debug.h"
#include <thread>

namespace chrn = std::chrono;
imp::Clock::Clock(unsigned short variableFpsCap, unsigned short fixedFpsCap, unsigned short maxAccumulated) :
    m_targetFps(static_cast<uint64_t>(variableFpsCap)),
    m_targetFrameDuration(Duration(m_targetFps != 0 ? 1'000'000'000ll * 1000 / (1000 * variableFpsCap + 50) : 0)), //TODO: division rounding?
    m_fixedTargetFps(static_cast<uint64_t>(fixedFpsCap)),
    m_fixedTargetFrameDuration(Duration(m_fixedTargetFps != 0 ? 1'000'000'000 / m_fixedTargetFps : 0)),
    m_maxAccumulatedTime(Duration(m_fixedTargetFrameDuration* maxAccumulated)),
    m_accumulatedTime(Duration::zero()),
    m_fixedFrame(false),
    m_lastFrameTime(),
    m_nextFrameTime(),
    m_elapsedTime(Duration::zero()),
    m_deltaTime(Duration::zero())
{
    // Constructor body
}


void imp::Clock::update() noexcept
{
    TimePoint now = SystemClock::now();
    // Skip sleeping if behind 
    if ((now < m_nextFrameTime) && (m_targetFps > 0)) {
        auto remaining = m_nextFrameTime - now;
        if (remaining > std::chrono::microseconds(200)) {
            auto coarseSleep = remaining - remaining / 5;
            std::this_thread::sleep_for(coarseSleep);
        }
        while ((now = SystemClock::now()) < m_nextFrameTime) {}
    }
    //TODO: Cap delta?
    m_deltaTime = now - m_lastFrameTime;
    m_lastFrameTime = now;
    if (now > m_nextFrameTime + m_targetFrameDuration) {
        // We've missed multiple frames. Resync to now.
        m_nextFrameTime = now + m_targetFrameDuration;
    }
    else {
        // Stay on schedule
        m_nextFrameTime += m_targetFrameDuration;
    }

    m_elapsedTime += m_deltaTime;

    m_accumulatedTime += m_deltaTime;
    if (m_accumulatedTime > m_maxAccumulatedTime) {
        m_accumulatedTime = m_maxAccumulatedTime; // Prevent death spiral
    }
}
bool imp::Clock::fixedUpdate()  noexcept
{
    if (m_accumulatedTime >= m_fixedTargetFrameDuration) {
        m_accumulatedTime -= m_fixedTargetFrameDuration;
        return true;
    }
    return false;
}
