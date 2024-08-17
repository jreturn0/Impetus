
#include "FileDirectoryHelper.h"

FileDirectoryHelper::FileDirectoryHelper(const std::string& path)
{
	gotoDirectory(path);
}

const std::set<fs::path>& FileDirectoryHelper::getFiles() const
{
	return files;
}

const std::set<fs::path>& FileDirectoryHelper::getDirectories() const
{
	return directories;
}

const std::optional<fs::path>& FileDirectoryHelper::getSelectedFile() const
{
	return selectedFile;
}

const fs::path& FileDirectoryHelper::getCurrentPath() const
{
	return currentPath;
}

bool FileDirectoryHelper::checkForFile(const std::string& filepath) const
{
	return files.contains(filepath);
}

bool FileDirectoryHelper::checkForFile(const fs::path& filepath) const
{
	return files.contains(filepath);
}


bool FileDirectoryHelper::checkForDirectory(const fs::path& dir) const
{
	return directories.contains(dir);
}

fs::path FileDirectoryHelper::getFile(const std::string& file) const
{
	const auto it = files.find(file); return it != files.end() ? currentPath / *it : "";
}

void FileDirectoryHelper::updatePathDirectory()
{
	files.clear();
	directories.clear();

	try {
		if (!fs::exists(currentPath) || !fs::is_directory(currentPath)) {
			throw std::runtime_error("Invalid directory path.");
		}

		for (const auto& entry : fs::directory_iterator(currentPath)) {
			if (entry.is_directory()) {
				directories.insert(entry);
			} else {

				files.insert(entry);
			}
		}
	} catch (const std::exception& e) {
		throw std::runtime_error("Failed to update directory " + currentPath.string() + ": " + e.what());
	}
}

std::string FileDirectoryHelper::getFilePath(const std::string& file) const
{
	auto it = files.find(file);
	if (it != files.end()) {
		return (currentPath / *it).string();
	}

	return currentPath.string() + "/does not exist: " + file;
}

bool FileDirectoryHelper::checkForLocalFile(const std::string& file) const
{
	const auto path = currentPath / file;
	return checkForFile(path);
}

bool FileDirectoryHelper::checkForLocalFile(const fs::path& filename) const
{
	const auto path = currentPath / filename;
	return checkForFile(path);
}

void FileDirectoryHelper::gotoParentDirectory()
{
	currentPath = currentPath.parent_path();
	updatePathDirectory();
}

void FileDirectoryHelper::gotoDirectory(const std::string& path)
{
	currentPath = fs::path(path);
	updatePathDirectory();
}

void FileDirectoryHelper::gotoChildDirectory(const std::string& childPath)
{
	currentPath /= childPath;
	updatePathDirectory();
}

void FileDirectoryHelper::setSelectedFile(const std::string& filename)
{
	if (files.contains(filename)) {
		selectedFile = currentPath / filename;
	} else {
		selectedFile.reset();
		throw std::runtime_error("File not found in the current directory.");
	}
}

void FileDirectoryHelper::setSelectedLocalFile(const std::string& filename)
{
	auto path = currentPath / fs::path(filename);
	if (files.contains(path)) {
		selectedFile = path;
	} else {
		selectedFile.reset();
		throw std::runtime_error("File not found in the current directory.");
	}
}
