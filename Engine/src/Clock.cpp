#include "Clock.h"
#include "Debug.h"
#include <thread>

namespace chrn = std::chrono;
using hClock = chrn::high_resolution_clock;
Imp::Clock::Clock(int fpsCap, double fixedFps, double maxAccum) : start(hClock::now()), last(hClock::now()), realElapsed(0.), deltaTime(0.),
fpsCap(fpsCap), targetFrameTime(1.0l / fpsCap), fixedStep(1.0l / fixedFps), accumulatedTime(0.), maxAccumulatedTime(maxAccum), fixedFrame(true)
{}



void Imp::Clock::update()
{
	// Measure start time
	auto frameEndTime = hClock::now();

	// Calculate elapsed time since last frame
	chrn::duration<double> elapsed = frameEndTime - last;
	deltaTime = elapsed.count();


	// Enforce target FPS if specified
	if (fpsCap > 0) {
		 double frameTimeLeft = targetFrameTime - deltaTime;
		//sleep thread for only 85% of the frame time at to not oversleep from possibly processing time or inaccuracy
		if (frameTimeLeft >= 0) {
			auto sleepDuration = chrn::duration<double>(frameTimeLeft * 0.85); 
			std::this_thread::sleep_for(sleepDuration);
		}

		// Busy wait for the remaining time 
		while (frameTimeLeft >= 0) {
			frameEndTime = hClock::now();
			elapsed = frameEndTime - last;
			deltaTime = elapsed.count();
			frameTimeLeft = targetFrameTime - deltaTime;
		}

		// Measure time again after sleeping to correct deltaTime
		frameEndTime = hClock::now();
		elapsed = frameEndTime - last;
		deltaTime = elapsed.count();
	}

	// Update last frame time
	last = frameEndTime;
	realElapsed += deltaTime;
	// Update accumulated time
	accumulatedTime += deltaTime;
	if (accumulatedTime > maxAccumulatedTime) {
		accumulatedTime = maxAccumulatedTime; // Prevent excessive accumulation
	}
}
bool Imp::Clock::isFixed() const
{
	return accumulatedTime >= fixedStep;
}

bool Imp::Clock::fixedUpdate()
{
	if (accumulatedTime >= fixedStep) {
		accumulatedTime -= fixedStep;
		return true;
	}
	return false;
}
