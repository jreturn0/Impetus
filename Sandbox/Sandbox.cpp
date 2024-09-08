// Sandbox.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <iostream>
#include <shared_mutex>

#include "StackedSliceArray.h"
#include "AnsiCodes.h"
#include "CVarSystem.h"
#include "StringHash.h"



class ConsoleDisplay : public ICVarDisplay
{
public:
	void display(const CVarValue& initial, CVarValue& current,
				 std::string_view name, std::string_view description, std::string_view category,
				 CVarFlags flags, CVarMinMaxType min, CVarMinMaxType max) override
	{

		std::visit([&](auto& value) {
            using T = std::decay_t<decltype(value)>;

            // Display basic information
            std::cout << "Name: " << name << "\n";
            std::cout << "\tDescription: " << description << "\n";
            std::cout << "\tCategory: " << category << "\n";

            // Check if min and max are of the same type as the value type
            if (std::holds_alternative<std::monostate>(min) && std::holds_alternative<std::monostate>(max)) {
                std::cout << "\tNo Min/Max\n";
            } else {
                if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, double>) {
                    auto minValue = std::holds_alternative<T>(min) ? std::get<T>(min) : std::numeric_limits<T>::lowest();
                    auto maxValue = std::holds_alternative<T>(max) ? std::get<T>(max) : std::numeric_limits<T>::max();
                    std::cout << "\tMin: " << minValue << ", Max: " << maxValue << "\n";
                }
            }

            // Display the current value based on type
            if constexpr (std::is_same_v<T, bool>) {
                std::cout  << "\tValue(bool): " << (value ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                std::cout  << "\tValue(int64_t): " << value << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                std::cout  << "\tValue(double): " << value << "\n";
            } else if constexpr (std::is_same_v<T, std::string>) {
                std::cout  << "\tValue(string): " << value << "\n";
            } else if constexpr (std::is_same_v<T, std::array<double, 2>>) {
                std::cout  << "\tValue(Vec2): [" << value[0] << ", " << value[1] << "]\n";
            } else if constexpr (std::is_same_v<T, std::array<double, 3>>) {
                std::cout  << "\tValue(Vec3): [" << value[0] << ", " << value[1] << ", " << value[2] << "]\n";
            } else if constexpr (std::is_same_v<T, std::array<double, 4>>) {
                std::cout  << "\tValue(Vec4): [" << value[0] << ", " << value[1] << ", " << value[2] << ", " << value[3] << "]\n";
            }
                   }, current);
	}
};


int main()
{

	auto&& cvarSystem = *CVarSystem::Get();

	std::initializer_list<bool> test = { true,false,true };


	AutoCVar_Float floatCVar("FloatTest", "It Floatin all over the place", "Testing Cat One", 1.657, CVarFlags::None, 0.0, 10.0);

    AutoCVar_String stringCVar("StringTest", "It's a string", "Testing Cat One", "Hello World", CVarFlags::None);

    AutoCVar_Vec4 vec4CVar( "Vec4Test", "It's a Vec4", "Testing Cat Two", { 1.534, 2.1256, 3.1234, 4.654 }, CVarFlags::None, 0.0, 100.0);

	ConsoleDisplay display{};
    std::cout << "\nDisplaying CVar\n";

	cvarSystem.renderIGUIDisplay(display);

    floatCVar.set(466165.343);

    stringCVar.set("Hello Test!");
    std::cout << "\nDisplaying CVar\n";

    cvarSystem.renderIGUIDisplay(display);


}

