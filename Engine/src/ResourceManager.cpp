#include "ResourceManager.h"

 ID imp::ResourceManager::GetPathID(const std::string& path)
{
	if (auto it = pathIDMap.find(path); it != pathIDMap.end()) {
		return it->second;
	} else {
		ID newID = idCounter++;
		pathIDMap[path] = newID;
		return newID;
	}
}
