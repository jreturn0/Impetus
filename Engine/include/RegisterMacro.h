#pragma once
// Helper macros to generate unique identifiers
#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)
// Uses both __LINE__ and __COUNTER__ to avoid collisions across different headers having the same line numbers
#define UNIQUE_NAME(base) CONCATENATE(CONCATENATE(base, __LINE__), __COUNTER__)

