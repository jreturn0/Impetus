#include "ConsoleVariableSystem.h"

#include <shared_mutex>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <array>
#include <functional>
#include <span>

#include "StackedSliceArray.h"
#include "StringHash.h"

namespace CVarDetails {
	enum class CVarType : uint32_t
	{
		Bool,
		Int,
		Int2,
		Int3,
		Int4,
		UInt,
		UInt2,
		UInt3,
		UInt4,
		Float,
		Float2,
		Float3,
		Float4,
		String,
	};

	using Bool = bool;
	using UInt = uint64_t;
	using UInt2 = std::array<uint64_t, 2>;
	using UInt3 = std::array<uint64_t, 3>;
	using UInt4 = std::array<uint64_t, 4>;
	using Int = int64_t;
	using Int2 = std::array<int64_t, 2>;
	using Int3 = std::array<int64_t, 3>;
	using Int4 = std::array<int64_t, 4>;
	using Float = double;
	using Float2 = std::array<double, 2>;
	using Float3 = std::array<double, 3>;
	using Float4 = std::array<double, 4>;
	using String = std::string;

	using Variable = std::variant<Bool, UInt, UInt2, UInt3, UInt4, Int, Int2, Int3, Int4, Float, Float2, Float3, Float4, String>;

	struct CVarInt
	{
		int64_t initial;
		int64_t defaultValue;
	};
	struct CVarInt2 : CVarInt
	{
		int64_t initial2;
		int64_t defaultValue2;
	};



	template <typename... Ts>
	constexpr size_t max_sizeof()
	{
		return std::max({ sizeof(Ts)... });
	}
	constexpr auto size = sizeof(Variable);

	//using CVarValue = std::variant<bool, uint64_t, uint64_t[2], uint64_t[3], uint64_t[4], int64_t, int64_t[2], int64_t[3], int64_t[4], double, double[2], double[3], double[4], std::string>;




	template <typename T, typename Variant, std::size_t Index = 0>
	consteval bool TypeIsInVariant()
	{
		if constexpr (Index == std::variant_size_v<Variant>) {
			return false;
		} else if constexpr (std::is_same_v<T, std::variant_alternative_t<Index, Variant>>) {
			return true;
		} else {
			return TypeIsInVariant<T, Variant, Index + 1>();
		}

	}
	//works
	constexpr bool inVariant = TypeIsInVariant<char, Variable>();

	template <typename T>
	consteval bool TypeInVariable()
	{
		Variable v = 1ll;
		return TypeIsInVariant<T, Variable>();
	}

	//doesn't work
	constexpr bool inVariant2 = TypeInVariable<char>();

	struct Info
	{
		size_t arrayIndex;
		CVarType type;
		uint32_t flags;
		std::string name;
		std::string description;
	};

	template<typename T>
	struct Property
	{
		T initial;
		T current;
		Info* info{ nullptr };
	};



	template<typename T, size_t N>
	struct PropertyArray
	{

		std::array<Property<T>, N> properties;
		//std::array<Property<T>, S> properties;
		size_t lastCVar{ 0 };
		PropertyArray() = default;

		auto& operator [] (size_t index) const { return properties[index]; }
		Property<T>* GetCurrentStorage(size_t index) { return &properties[index]; }
		T* GetCurrentPtr(size_t index) { return &properties[index].value; }
		T GetCurrent(size_t index) { return properties[index].value; }
		void SetCurrent(size_t index, const T& value) { properties[index].value = value; }

		int Add(const T& initial, const T& current, Info* info)
		{
			if (lastCVar >= N) {
				throw std::runtime_error("Too many CVars");
			}
			properties[lastCVar].initial = initial;
			properties[lastCVar].current = current;
			properties[lastCVar].info = info;
			info->arrayIndex = lastCVar;
			++lastCVar;
			return lastCVar - 1ll;
		}
		int Add(const T& value, Info* info)
		{
			return Add(value, value, info);
		}



	};


	class CVarSystemImpl
	{
	private:
		std::shared_mutex mutex;
		std::unordered_map<size_t, Info> propertyInfoMap;
		std::vector< Info* > cachedEditProperties;
		Info* initVariable(std::string_view name, std::string_view description);
	public:
		constexpr static size_t MAX_PROPERTIES = 1024;

		Info* GetInfo(StringHash<> hash);

		constexpr static size_t MAX_INT_PROPERTIES = 1024;
		PropertyArray<int64_t, MAX_INT_PROPERTIES> intProperties;
		PropertyArray< std::array<int64_t, 2>, MAX_INT_PROPERTIES> int2Properties;
		PropertyArray< std::array<int64_t, 3>, MAX_INT_PROPERTIES> int3Properties;
		PropertyArray< std::array<int64_t, 4>, MAX_INT_PROPERTIES> int4Properties;
		constexpr static size_t MAX_UINT_PROPERTIES = 1024;
		PropertyArray<uint64_t, MAX_UINT_PROPERTIES> uintProperties;
		PropertyArray< std::array<uint64_t, 2>, MAX_UINT_PROPERTIES> uint2Properties;
		PropertyArray< std::array<uint64_t, 3>, MAX_UINT_PROPERTIES> uint3Properties;
		PropertyArray< std::array<uint64_t, 4>, MAX_UINT_PROPERTIES> uint4Properties;
		constexpr static size_t MAX_FLOAT_PROPERTIES = 1024;
		PropertyArray<double, MAX_FLOAT_PROPERTIES> floatProperties;
		PropertyArray< std::array<double, 2>, MAX_FLOAT_PROPERTIES> float2Properties;
		PropertyArray< std::array<double, 3>, MAX_FLOAT_PROPERTIES> float3Properties;
		PropertyArray< std::array<double, 4>, MAX_FLOAT_PROPERTIES> float4Properties;
		constexpr static size_t MAX_BOOL_PROPERTIES = 1024;
		PropertyArray<bool, MAX_BOOL_PROPERTIES> boolProperties;
		constexpr static size_t MAX_STRING_PROPERTIES = 256;
		PropertyArray<std::string, MAX_STRING_PROPERTIES> stringProperties;



		Info* CreateFloatVar(std::string_view name, std::string_view description, double initial, double current);

		Info* CreateFloat2Var(std::string_view name, std::string_view description, std::array<double, 2> initial, std::array<double, 2> current);

		Info* CreateFloat3Var(std::string_view name, std::string_view description, std::array<double, 3> initial, std::array<double, 3> current);

		Info* CreateFloat4Var(std::string_view name, std::string_view description, std::array<double, 4> initial, std::array<double, 4> current);

		Info* CreateIntVar(std::string_view name, std::string_view description, int64_t initial, int64_t current);

		Info* CreateInt2Var(std::string_view name, std::string_view description, std::array<int64_t, 2> initial, std::array<int64_t, 2> current);

		Info* CreateInt3Var(std::string_view name, std::string_view description, std::array<int64_t, 3> initial, std::array<int64_t, 3> current);

		Info* CreateInt4Var(std::string_view name, std::string_view description, std::array<int64_t, 4> initial, std::array<int64_t, 4> current);

		Info* CreateUIntVar(std::string_view name, std::string_view description, uint64_t initial, uint64_t current);

		Info* CreateUInt2Var(std::string_view name, std::string_view description, std::array<uint64_t, 2> initial, std::array<uint64_t, 2> current);

		Info* CreateUInt3Var(std::string_view name, std::string_view description, std::array<uint64_t, 3> initial, std::array<uint64_t, 3> current);

		Info* CreateUInt4Var(std::string_view name, std::string_view description, std::array<uint64_t, 4> initial, std::array<uint64_t, 4> current);

		Info* CreateBoolVar(std::string_view name, std::string_view description, bool initial, bool current);

		Info* CreateStringVar(std::string_view name, std::string_view description, std::string_view initial, std::string_view current);




	};

	Info* CVarSystemImpl::initVariable(const std::string_view name, const std::string_view description)
	{
		std::unique_lock lock(mutex);
		auto hash = StringHash(name);
		propertyInfoMap[hash] = Info{};
		Info& info = propertyInfoMap[hash];
		info.name = name;
		info.description = description;
		return &info;
	}

	Info* CVarSystemImpl::GetInfo(StringHash<> hash)
	{
		std::shared_lock lock(mutex);
		const auto it = propertyInfoMap.find(hash);
		if (it == propertyInfoMap.end()) {
			return nullptr;
		}
		return &it->second;

	}

	Info* CVarSystemImpl::CreateFloatVar(std::string_view name, std::string_view description, double initial,
										 double current)
	{
		return nullptr;
	}


	void TestFunc()
	{
		PropertyArray<uint64_t, 10> testUInt{};
		auto* info = new Info{};
		testUInt.Add(1ll, info);

	}

} // namespace CVarDetails