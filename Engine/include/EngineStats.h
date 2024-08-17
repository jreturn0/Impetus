#pragma once
#include <string>
#include <unordered_map>


namespace Imp {
	//Statistics for a single frame stored in microseconds
	struct TimeStats
	{
		long long frameTime=0;
		long long minFrameTime= LONG_MAX;
		long long maxFrameTime = LONG_MIN;
		long long avgFrameTime;
		void update(const long long time)
		{
			frameTime = time;
			minFrameTime = std::min(minFrameTime, time);
			maxFrameTime = std::max(maxFrameTime, time);
			avgFrameTime = (avgFrameTime + time) / 2;
		}
	};

	struct EngineStats
	{
		TimeStats frameTime;
		TimeStats updateTime;
		TimeStats eventDispatchTime;


		std::unordered_map<std::string, TimeStats> systemsTime;

		void updateSystemTime(const std::string_view& name, const long long time)
		{
			systemsTime[name.data()].update(time);
		}

	};

#ifdef _DEBUG
#define RECORD_STATS_VARIABLES auto start = std::chrono::high_resolution_clock::now(); auto end = std::chrono::high_resolution_clock::now();
#define RECORD_STATS_START  start = std::chrono::high_resolution_clock::now();
#define RECORD_SYSTEM_STATS_END(stats, system)  end = std::chrono::high_resolution_clock::now(); (stats)->updateSystemTime(system, std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
#define RECORD_SYSTEM_STATS_END_P(stats, system)  end = std::chrono::high_resolution_clock::now(); (stats).updateSystemTime(system, std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
#define RECORD_STATS_END(stats, stat)  end = std::chrono::high_resolution_clock::now(); (stats)->stat.update(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());


#define RECORD_STATS_VARIABLES_N(n) auto start##n = std::chrono::high_resolution_clock::now(); auto end##n = std::chrono::high_resolution_clock::now();
#define RECORD_STATS_START_N(n)  start##n = std::chrono::high_resolution_clock::now();
#define RECORD_SYSTEM_STATS_END_N(stats, system,n)  end##n = std::chrono::high_resolution_clock::now(); (stats)->updateSystemTime(system, std::chrono::duration_cast<std::chrono::microseconds>(end##n - start##n).count());
#define RECORD_STATS_END_N(stats, stat,n)  end##n = std::chrono::high_resolution_clock::now(); (stats)->stat.update(std::chrono::duration_cast<std::chrono::microseconds>(end##n - start##n).count());
#endif

}
