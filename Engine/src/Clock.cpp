#include "Clock.h"
#include "Debug.h"
#include <thread>
#include <GLFW/glfw3.h>

namespace chrn = std::chrono;
imp::Clock::Clock(double currentTimeSeconds, unsigned short variableFpsCap, unsigned short fixedFpsCap, double maxAccumulatedFrames ) :
    m_targetFps(static_cast<uint64_t>(variableFpsCap)),
    m_targetFrameDuration(m_targetFps != 0 ? 1. / m_targetFps : 0),
    m_fixedTargetFps(static_cast<uint64_t>(fixedFpsCap)),
    m_fixedTargetFrameDuration(m_fixedTargetFps != 0 ? 1. / m_fixedTargetFps : 0),
    m_maxAccumulatedFrames(maxAccumulatedFrames),
    m_maxAccumulatedTime(m_fixedTargetFrameDuration* m_maxAccumulatedFrames),
    m_accumulatedTime(0),
    m_fixedFrame(false),
    m_lastFrameTime(currentTimeSeconds),
    m_nextFrameTime(m_lastFrameTime + m_targetFrameDuration),
    m_elapsedTime(0),
    m_deltaTime(0)

{

}


void imp::Clock::update() noexcept
{
    auto now = glfwGetTime();
    // If we're behind schedule, skip sleeping entirely
    if ((now < m_nextFrameTime) && (m_targetFps > 0)) {
        double remaining = m_nextFrameTime - now;
        if (remaining > 0.01) { // >10ms

            std::this_thread::sleep_for(std::chrono::duration<double>(remaining - 0.002));
        }
        while ((now = glfwGetTime()) < m_nextFrameTime) {
            std::this_thread::yield();
        }
    }
    m_deltaTime = now - m_lastFrameTime;
    m_lastFrameTime = now;


    if (now > m_nextFrameTime + 2.0 * m_targetFrameDuration) {
        m_nextFrameTime = now + m_targetFrameDuration;
    }
    else {
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
