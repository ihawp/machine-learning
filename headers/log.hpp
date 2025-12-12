#pragma once

#include <filesystem>
#include <string>
#include <vector>

bool log
(
    const std::string &directory,
    std::string &name,
    std::vector<std::string> &data,
    std::vector<std::string> &columnNames
);

bool createDirectory
(
    std::filesystem::path &path
);

bool createCSV
(
    std::filesystem::path &path,
    std::vector<std::string> &columns
);

bool updateCSV
(
    std::filesystem::path &path,
    std::vector<std::string> &data
);