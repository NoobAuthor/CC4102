#include "utils/file_manager.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <regex>

FileManager& FileManager::getInstance() {
  static FileManager instance;
  return instance;
}

FileManager::FileManager() {
  directoryCache["data"] = std::filesystem::exists("data");
  directoryCache["data/sequences"] = std::filesystem::exists("data/sequences");
  directoryCache["data/results"] = std::filesystem::exists("data/results");
  directoryCache["results"] = std::filesystem::exists("results");
}

bool FileManager::directoryExists(const std::string& path) {
  auto it = directoryCache.find(path);
  if (it != directoryCache.end()) {
    return it->second;
  }

  bool exists =
      std::filesystem::exists(path) && std::filesystem::is_directory(path);
  directoryCache[path] = exists;
  return exists;
}

void FileManager::createDirectoryIfNotExists(const std::string& path) {
  if (!directoryExists(path)) {
    std::filesystem::create_directories(path);
    directoryCache[path] = true;
  }
}

bool FileManager::fileExists(const std::string& path) {
  auto it = fileCache.find(path);
  if (it != fileCache.end()) {
    return it->second;
  }

  bool exists =
      std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
  fileCache[path] = exists;
  return exists;
}

std::vector<std::string> FileManager::listFiles(const std::string& directory,
                                                const std::string& pattern) {
  std::vector<std::string> files;

  if (!directoryExists(directory)) {
    return files;
  }

  std::regex patternRegex;
  bool usePattern = !pattern.empty();

  if (usePattern) {
    std::string regexPattern = pattern;
    size_t pos = 0;
    while ((pos = regexPattern.find(".", pos)) != std::string::npos) {
      regexPattern.replace(pos, 1, "\\.");
      pos += 2;
    }
    pos = 0;
    while ((pos = regexPattern.find("*", pos)) != std::string::npos) {
      regexPattern.replace(pos, 1, ".*");
      pos += 2;
    }
    patternRegex = std::regex(regexPattern);
  }

  for (const auto& entry : std::filesystem::directory_iterator(directory)) {
    if (entry.is_regular_file()) {
      std::string filename = entry.path().filename().string();

      if (!usePattern || std::regex_match(filename, patternRegex)) {
        files.push_back(entry.path().string());
        fileCache[entry.path().string()] = true;
      }
    }
  }

  return files;
}

std::string FileManager::findLargestArityTestFile() {
  for (int m = 60; m >= 4; m -= 4) {
    std::string candidateFile =
        "data/sequences/arity_test_" + std::to_string(m) + "M.bin";
    if (fileExists(candidateFile)) {
      return candidateFile;
    }
  }

  return "";
}

std::vector<std::string> FileManager::findSequenceFiles(size_t multiplier) {
  std::vector<std::string> sequenceFiles;
  std::string directory = "data/sequences";

  if (!directoryExists(directory)) {
    return sequenceFiles;
  }

  std::string pattern;
  if (multiplier > 0) {
    pattern = "seq_" + std::to_string(multiplier) + "M_*.bin";
  } else {
    pattern = "seq_*.bin";
  }

  sequenceFiles = listFiles(directory, pattern);

  std::sort(sequenceFiles.begin(), sequenceFiles.end());

  return sequenceFiles;
}

void FileManager::clearCache() {
  fileCache.clear();
  directoryCache.clear();
}