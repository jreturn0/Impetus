#pragma once
#include <unordered_map>
namespace Imp::Render {





    struct RenderStats
    {
		struct TimeStats
		{
			long long frameTime;
			long long minFrameTime;
			long long maxFrameTime;
			long long avgFrameTime;
			void update(const long long time)
			{
				frameTime = time;
				minFrameTime = std::min(minFrameTime, time);
				maxFrameTime = std::max(maxFrameTime, time);
				avgFrameTime = (avgFrameTime + time) / 2;
			}
		};
        unsigned long long triangleCount;   
        unsigned long long drawCallCount;

		std::unordered_map<std::string, TimeStats> timeStatsMap;


    };
}