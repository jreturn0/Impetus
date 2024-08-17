#pragma once

// Helper macros to generate unique identifiers
#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)
#define UNIQUE_NAME(base) CONCATENATE(base, __LINE__)








// Register component macro









