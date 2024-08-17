#pragma once
#include <chrono>

namespace Imp {
	class Clock
	{
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> start;
		std::chrono::time_point<std::chrono::high_resolution_clock> last;
		double realElapsed;
		double deltaTime;
		int fpsCap;
		double targetFrameTime;
		double fixedStep;
		double accumulatedTime;
		double maxAccumulatedTime;
		bool fixedFrame;
	public:
		   Clock(const Clock&) = delete;
        Clock(Clock&&) = delete;
        Clock& operator=(const Clock&) = delete;
        Clock& operator=(Clock&&) = delete;

        explicit Clock(int fpsCap, double fixedFps, double maxAccum = 0.075);
        void update();
		bool isFixed() const;
		bool fixedUpdate();

        inline bool isFixedFrame() const { return fixedFrame; }
        inline double getDelta() const { return deltaTime; }
        inline double getElapsed() const { return realElapsed; }
        inline double getFixedStep() const { return fixedStep; }
        inline double getAccumulatedTime() const { return accumulatedTime; }
	};


}