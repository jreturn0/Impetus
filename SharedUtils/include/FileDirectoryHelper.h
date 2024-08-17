#pragma once

#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <set>
#include <stdexcept>

namespace fs = std::filesystem;


class FileDirectoryHelper
{
private:
	fs::path currentPath;
	std::set<fs::path> files;
	std::set<fs::path> directories;
	std::optional<fs::path> selectedFile;

public:
	explicit FileDirectoryHelper(const std::string& path = fs::current_path().string()); //constructor default current path

	const std::set<fs::path>& getFiles() const; //get files in current directory
	const std::set<fs::path>& getDirectories() const; //get directories in current directory
	const std::optional<fs::path>& getSelectedFile() const; //get optionally selected file
	std::string getFilePath(const std::string& file) const; //get file path from file name
	const fs::path& getCurrentPath() const; //get current path
	fs::path getFile(const std::string& file) const; //get filepath from file name

	void gotoParentDirectory();//go up a directory i.e. parent
	void gotoDirectory(const std::string& path);//goto directory from a full path
	void gotoChildDirectory(const std::string& childPath);//specify directory in currentPath

	void setSelectedFile(const std::string& filename);//optional set selectedFIle
	void setSelectedLocalFile(const std::string& filename);//optional set selectedFIle

	void updatePathDirectory();//updates files and directories

	bool checkForFile(const std::string& filepath) const; //check for file in current directory
	bool checkForFile(const fs::path& filepath) const; //check for file in current directory

	bool checkForLocalFile(const std::string& filename) const; //check for file in current directory
	bool checkForLocalFile(const fs::path& filename) const; //check for file in current directory

	//bool checkForDirectory(const std::string& dir) const; //check for directory in current directory
	bool checkForDirectory(const fs::path& dir) const; //check for directory in current directory


};
