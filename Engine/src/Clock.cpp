#include "Clock.h"
#include "Debug.h"
#include <thread>

namespace chrn = std::chrono;
Imp::Clock::Clock(unsigned short variableFpsCap, unsigned short fixedFpsCap, unsigned short maxAccumulated) :
	variableTarget(variableFpsCap),
	variableTargetDuration(variableTarget > 0
						   ? Duration(1'000'000'000ll * 1000 / (1000 * variableTarget + 50))  // Adjust to simulate a tiny increase in FPS
						   : Duration(0)),
	fixedTarget(fixedFpsCap),
	fixedTargetDuration(fixedTarget > 0
						? Duration(1'000'000'000ll / fixedTarget)
						: Duration(0)),
	lastFrameEnd(SystemClock::now()),
	deltaTime(0),
	realElapsed(0),
	accumulatedTime(0),
	maxAccumulatedTime(Duration((1'000'000'000ll / fixedFpsCap)* maxAccumulated)),
	fixedFrame(false)
{
	// Constructor body
}







void Imp::Clock::update()
{
	// Measure start time
	TimePoint frameEndTime = SystemClock::now();

	// Calculate elapsed time since last frame
	deltaTime = frameEndTime - lastFrameEnd;


	// Enforce target FPS if specified
	if (variableTarget > 0) {
		const Duration frameTimeLeft = variableTargetDuration - deltaTime;
		constexpr auto zero = Duration(0ll);
		Duration sleepDuration = frameTimeLeft * 60 / 100;
		if (sleepDuration > zero) {
			std::this_thread::sleep_for(sleepDuration);
		}

		while ((deltaTime =((frameEndTime = SystemClock::now()) - lastFrameEnd)) < variableTargetDuration) {}
	}

	// Update last frame time
	lastFrameEnd = frameEndTime;
	realElapsed += deltaTime;
	// Update accumulated time
	accumulatedTime += deltaTime;
	if (accumulatedTime > maxAccumulatedTime) {
		accumulatedTime = maxAccumulatedTime; // Prevent excessive accumulation
	}
}
bool Imp::Clock::isFixed() const
{
	return accumulatedTime >= fixedTargetDuration;
}

bool Imp::Clock::fixedUpdate()
{
	if (accumulatedTime >= fixedTargetDuration) {
		accumulatedTime -= fixedTargetDuration;
		return true;
	}
	return false;
}
