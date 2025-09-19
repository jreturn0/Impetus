#include "TimerStats.h"
#include <unordered_map>
#include "StringHash.h"
#include "StringUtils.h"
#include "fmt/core.h"
#include "nanobench.h"
#include <unordered_set>

struct DummyObject {
    int value{ 0 };
    void doWork() {
        for (volatile int i = 0; i < 1000; ++i) {
            value += i;
        }
    }
};


struct CacheTestBaseExample {
    bool add(const utl::StringHash nameHash, const DummyObject& obj) {};
    DummyObject* get(const utl::StringHash nameHash) { return nullptr; };
    bool remove(const utl::StringHash nameHash) { return false; };
    size_t size() const noexcept { return 0; };
};

struct CacheTestOne {

    std::unordered_map<utl::StringHash, DummyObject, utl::StringHashOp> resourceMap{};

    // Original StringHash API
    bool add(const utl::StringHash nameHash, const DummyObject& obj) {
        if (resourceMap.contains(nameHash)) {
            return false;
        }
        resourceMap[nameHash] = obj;
        return true;
    }

    DummyObject* get(const utl::StringHash nameHash) {
        auto it = resourceMap.find(nameHash);
        return it != resourceMap.end() ? &it->second : nullptr;
    }

    bool remove(const utl::StringHash nameHash) {
        if (!resourceMap.contains(nameHash)) {
            return false;
        }
        resourceMap.erase(nameHash);
        return true;
    }

    size_t size() const noexcept {
        return resourceMap.size();
    }

    // Extra inputs
    bool add(std::string_view name, const DummyObject& obj) { return add(utl::StringHash{ name }, obj); }
    DummyObject* get(std::string_view name) { return get(utl::StringHash{ name }); }
    bool remove(std::string_view name) { return remove(utl::StringHash{ name }); }

    bool add(const char* name, const DummyObject& obj) { return add(std::string_view{ name }, obj); }
    DummyObject* get(const char* name) { return get(std::string_view{ name }); }
    bool remove(const char* name) { return remove(std::string_view{ name }); }
};

struct CacheTestTwo {
    std::unordered_map <uint64_t, DummyObject> m_resources;
    std::vector<std::string> m_resourceNames;

    // Original StringHash API
    bool add(const utl::StringHash nameHash, const DummyObject& obj) {
        if (m_resources.contains(nameHash)) {
            return false;
        }
        m_resources[nameHash] = obj;
        m_resourceNames.emplace_back(nameHash.strView);
        return true;
    }

    DummyObject* get(const utl::StringHash nameHash) {
        auto it = m_resources.find(nameHash);
        return it != m_resources.end() ? &it->second : nullptr;
    };
    bool remove(const utl::StringHash nameHash) {
        auto it = m_resources.find(nameHash);
        if (it == m_resources.end()) {
            return false;
        }
        m_resources.erase(it);
        auto&& itName = std::find_if(m_resourceNames.begin(), m_resourceNames.end(), [&nameHash](const std::string& n) { return utl::StringHash(n) == nameHash; });
        if (itName != m_resourceNames.end())
            std::erase(m_resourceNames, *itName);
        return true;
    };
    size_t size() const noexcept { return m_resources.size(); };

    // Extra inputs
    bool add(std::string_view name, const DummyObject& obj) { return add(utl::StringHash{ name }, obj); }
    DummyObject* get(std::string_view name) { return get(utl::StringHash{ name }); }
    bool remove(std::string_view name) { return remove(utl::StringHash{ name }); }

    bool add(const char* name, const DummyObject& obj) { return add(std::string_view{ name }, obj); }
    DummyObject* get(const char* name) { return get(std::string_view{ name }); }
    bool remove(const char* name) { return remove(std::string_view{ name }); }
};

struct CacheTestThree {
    std::unordered_map <uint64_t, DummyObject> m_resources;
    std::unordered_set<std::string> m_resourceNames;

    // Original StringHash API
    bool add(const utl::StringHash nameHash, const DummyObject& obj) {
        if (m_resources.contains(nameHash)) {
            return false;
        }
        m_resources[nameHash] = obj;
        m_resourceNames.emplace(nameHash.strView);
        return true;
    }

    DummyObject* get(const utl::StringHash nameHash) {
        auto it = m_resources.find(nameHash);
        return it != m_resources.end() ? &it->second : nullptr;
    };
    bool remove(const utl::StringHash nameHash) {
        auto it = m_resources.find(nameHash);
        if (it == m_resources.end()) {
            return false;
        }
        m_resources.erase(it);

        if (m_resourceNames.contains(std::string(nameHash.strView)))
            m_resourceNames.erase(std::string(nameHash.strView));
        return true;
    };
    size_t size() const noexcept { return m_resources.size(); };

    // Extra inputs
    bool add(std::string_view name, const DummyObject& obj) { return add(utl::StringHash{ name }, obj); }
    DummyObject* get(std::string_view name) { return get(utl::StringHash{ name }); }
    bool remove(std::string_view name) { return remove(utl::StringHash{ name }); }

    bool add(const char* name, const DummyObject& obj) { return add(std::string_view{ name }, obj); }
    DummyObject* get(const char* name) { return get(std::string_view{ name }); }
    bool remove(const char* name) { return remove(std::string_view{ name }); }
};

struct CacheTestFour {
    std::unordered_map<std::string, DummyObject, utl::TransparentStringHash, std::equal_to<>> m_resources;

    // Native string/string_view API
    bool add(const std::string_view name, const DummyObject& obj) {
        if (m_resources.contains(name)) {
            return false;
        }
        m_resources[std::string(name)] = obj;
        return true;
    }

    DummyObject* get(const std::string_view  name) {
        auto it = m_resources.find(name);
        return it != m_resources.end() ? &it->second : nullptr;
    };

    bool remove(const std::string_view  name) {
        auto it = m_resources.find(name);
        if (it == m_resources.end()) {
            return false;
        }
        m_resources.erase(it);
        return true;
    };

    size_t size() const noexcept { return m_resources.size(); };

    // Support StringHash as input too
    bool add(const utl::StringHash nameHash, const DummyObject& obj) { return add(nameHash.strView, obj); }
    DummyObject* get(const utl::StringHash nameHash) { return get(nameHash.strView); }
    bool remove(const utl::StringHash nameHash) { return remove(nameHash.strView); }

    // Also accept const char*
    bool add(const char* name, const DummyObject& obj) { return add(std::string_view{ name }, obj); }
    DummyObject* get(const char* name) { return get(std::string_view{ name }); }
    bool remove(const char* name) { return remove(std::string_view{ name }); }
};

template<typename CacheType>
void benchmarkCache(const std::string& cacheName, int itemCount) {
    CacheType cache;
    std::vector<std::string> keys;
    keys.reserve(itemCount);
    for (int i = 0; i < itemCount; ++i) {
        keys.emplace_back(fmt::format("Item{}", i));
    }
    ankerl::nanobench::Bench bench;
    bench.title(fmt::format("{} Benchmark with {} items", cacheName, itemCount));
    bench.performanceCounters(true);
    bench.minEpochIterations(10);
    bench.run(fmt::format("{} Add", cacheName), [&]() {
        for (const auto& key : keys) {
            // Pass std::string (works for StringHash overloads via std::string_view)
            cache.add(key, DummyObject{});
        }
        });
    bench.run(fmt::format("{} Get", cacheName), [&]() {
        for (const auto& key : keys) {
            auto obj = cache.get(key);
            if (obj) {
                obj->doWork();
            }
        }
        });
    bench.run(fmt::format("{} Remove", cacheName), [&]() {
        for (const auto& key : keys) {
            cache.remove(key);
        }
        });
}
#include <cstdlib> 
int main() {
    constexpr size_t numItems = 2'000;
    benchmarkCache<CacheTestOne>("CacheTestOne", numItems);
    benchmarkCache<CacheTestTwo>("CacheTestTwo", numItems);
    benchmarkCache<CacheTestThree>("CacheTestThree", numItems);
    benchmarkCache<CacheTestFour>("CacheTestFour", numItems);
    system("cls");
    benchmarkCache<CacheTestOne>("CacheTestOne", numItems);
    benchmarkCache<CacheTestTwo>("CacheTestTwo", numItems);
    benchmarkCache<CacheTestThree>("CacheTestThree", numItems);
    benchmarkCache<CacheTestFour>("CacheTestFour", numItems);

    return 0;
}