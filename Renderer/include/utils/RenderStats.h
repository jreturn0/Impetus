#pragma once
#include <unordered_map>
#include <unordered_map>
#include "TimerStats.h"
#include "StringHash.h"
#include "StringUtils.h"

#ifndef IMP_ENABLE_PROFILING
#ifdef _DEBUG
#define IMP_ENABLE_PROFILING 1
#endif // DEBUG
#endif


namespace imp::gfx {
    struct RenderStats
    {
        uint64_t triangleCount{ 0 };
        uint64_t drawCallCount{ 0 };

        std::unordered_map<utl::StringHash, utl::TimerStats, utl::StringHashOp> timeStatsMap{};
        inline void reset() noexcept {
            triangleCount = 0;
            drawCallCount = 0;
            for (auto& [_, stats] : timeStatsMap) {
                stats.reset();
            }
        }
        inline utl::ScopeTimer time(const utl::StringHash nameHash) noexcept {
            return { timeStatsMap[nameHash] };
        }
    };
}

#if defined(IMP_ENABLE_PROFILING)
#define RENDERSTATS_SCOPED_TIMER(statsRef, name)                                   \
        constexpr auto UTL_CONCAT(scopedTimer, __LINE__) = name##_hash;                  \
        utl::ScopeTimer UTL_CONCAT(scopedTimerObj, __LINE__){                          \
            (statsRef).time(UTL_CONCAT(scopedTimer, __LINE__))                         \
        };
#else
#define RENDERSTATS_SCOPED_TIMER(statsRef, name) ;
#endif