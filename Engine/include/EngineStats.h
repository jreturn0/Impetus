#pragma once
#include "StringHash.h"
#include "TimerStats.h"
#include <ranges>
#include <string>
#include <unordered_map>

#ifndef IMP_ENABLE_PROFILING
#ifdef _DEBUG
#define IMP_ENABLE_PROFILING 1
#endif // DEBUG
#endif

namespace imp {
    struct EngineStats
    {
        std::unordered_map<utl::StringHash, utl::TimerStats, utl::StringHashOp> timeStatsMap{};
        inline void reset() noexcept {
            for (auto& stats : timeStatsMap | std::views::values) {
                stats.reset();
            }
        }
        inline utl::ScopeTimer time(const utl::StringHash nameHash) noexcept {
            return { timeStatsMap[nameHash] };
        }

    };
}
#if defined(IMP_ENABLE_PROFILING)
#define ENGINESTATS_SCOPED_TIMER(statsRef, name)                                   \
        constexpr auto UTL_CONCAT(scopedTimer, __LINE__) = name##_hash;                  \
        utl::ScopeTimer UTL_CONCAT(scopedTimerObj, __LINE__){                          \
            (statsRef).time(UTL_CONCAT(scopedTimer, __LINE__))                         \
        };
#define ENGINESTATS_SCOPED_TIMER_STR(statsRef, name)                                   \
        utl::ScopeTimer UTL_CONCAT(scopedTimerObj, __LINE__){                          \
            (statsRef).time(utl::StringHash{name})                         \
        };
#else
#define RENDERSTATS_SCOPED_TIMER(statsRef, name) ;
#endif