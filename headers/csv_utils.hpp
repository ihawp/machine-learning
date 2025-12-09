#include <filesystem>
#include <vector>
#include <string>

bool updateCSV
(
    std::filesystem::path path,
    std::vector<std::string> data
);

bool createCSV
(
    std::filesystem::path path,
    std::vector<std::string> columns
);

bool createDirectory
(
    std::filesystem::path path
);