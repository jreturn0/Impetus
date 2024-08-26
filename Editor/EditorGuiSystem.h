#pragma once
#include <functional>

#include "InputCodes.h"
#include "System.h"
#include "FileDirectoryHelper.h"
#include "Events/SystemEvents.h"
// Helper function to remove substrings
inline std::string RemoveSubstring(std::string input, const std::string& toRemove)
{
	size_t pos = input.find(toRemove);
	while (pos != std::string::npos) {
		input.erase(pos, toRemove.length());
		pos = input.find(toRemove);
	}
	return input;
}

// Function to clean up the type name
inline std::string CleanTypeName(const std::string& typeName)
{
	std::string result = typeName;

	// Remove "Component"
	result = RemoveSubstring(result, "Component");
	result = RemoveSubstring(result, "System");

	// Find the position of the last occurrence of "::"
	size_t pos = result.rfind("::");
	if (pos != std::string::npos) {
		// Remove everything before and including "::"
		result = result.substr(pos + 2);
	} else {
		// Find and remove "struct " if no "::" was found
		result = RemoveSubstring(result, "struct ");
		result = RemoveSubstring(result, "class ");
	}

	return result;
}

// Template function to get the cleaned type name
template<typename T>
std::string GetCleanTypeName()
{
	static const std::string cleanName = CleanTypeName(typeid(T).name());
	return cleanName;
}

class EditorGuiSystem final : public System
{
	//public types:
public:
	struct InputAction
	{
		Input::Key key;
		Input::State state;
		std::function<void()> action;

		bool operator==(const InputAction& other) const
		{
			return key == other.key && state == other.state;
		}

		void operator()() const
		{
			if (action) {
				action();
			}
		}


	};

private:
	std::vector<InputAction> actions;
	bool showWindow = true;
	bool dropTarget = false;
	entt::entity selectedEntity = entt::null;
	std::set<entt::id_type> componentFilter;
	bool showFileSelector = false;
	bool saveOnSelect = false;
	bool loadOnSelect = false;
	bool gui = true;
	std::set<std::string> nonActiveSystems;
	FileDirectoryHelper fileHelp;
	SystemsReorderedEvent systemOrder;
	void OnSystemsReorderedEvent(SystemsReorderedEvent& event);


	static bool EntityHasComponent(entt::registry& registry, entt::entity& entity, entt::id_type component);
	void entityIDWidget(entt::registry& registry, entt::entity& e, bool selectable = true);

	void drawEntityList(entt::registry& registry);
	void drawEntityInspector(entt::registry& registry);
	void drawMenuBar(entt::registry& registry);
	void drawFileSelector(entt::registry& registry);
	void drawEngineStats(entt::registry& registry);
	void drawGlobalSettings (entt::registry& registry);
	void drawSystemsStateEditor(entt::registry& registry);
	void drawInputState(entt::registry& registry);
	void drawAll(entt::registry& registry);
	
public:
	void initialize(entt::registry& registry) override;
	void update(entt::registry& registry, const float deltaTime) override;
	void cleanup(entt::registry& registry) override;

	void OnGuiEvent()
	{
		gui = !gui;

	}
};
