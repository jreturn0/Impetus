#pragma once
#include <chrono>

namespace Imp {
	class Clock
	{
	private:

		using SystemClock = std::chrono::steady_clock;
		using TimePoint = std::chrono::time_point<SystemClock>;
		using Duration = std::chrono::duration<long long, std::nano>;


		//variable fps members
		unsigned  short variableTarget; // 0 means no cap
		Duration variableTargetDuration; // target time per variableFrame


		unsigned  short fixedTarget; // fixed fps with no variance
		Duration fixedTargetDuration; // target time per fixedFrame



		TimePoint lastFrameEnd; // time point of last frame end
		Duration deltaTime; // time since last frame
		Duration realElapsed; // time since start of clock


		Duration accumulatedTime;
		Duration maxAccumulatedTime;
		bool fixedFrame;


	public:
		   Clock(const Clock&) = delete;
        Clock(Clock&&) = delete;
        Clock& operator=(const Clock&) = delete;
        Clock& operator=(Clock&&) = delete;

        explicit Clock(unsigned short variableFpsCap, unsigned short fixedFpsCap=60, unsigned short maxAccumulated = 2);
        void update();
		bool isFixed() const;
		bool fixedUpdate();

        inline bool isFixedFrame() const { return fixedFrame; }
		inline double getDelta() const { return deltaTime.count() / static_cast<double>(1'000'000) / 1000; }
		inline  double getElapsed() const { return realElapsed.count() / static_cast<double>(1'000'000) / 1000; }
		inline  double getFixedStep() const { return fixedTargetDuration.count() / static_cast<double>(1'000'000) / 1000; }
		inline  double getAccumulatedTime() const { return accumulatedTime.count() / static_cast<double>(1'000'000) / 1000; }
	};


}