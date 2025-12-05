#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <expected>
#include <chrono>

struct ArgumentData {
    std::filesystem::path f;    // filename
    std::string v;              // do
    int vc;                     // do count;
    std::string b;              // by
};

std::expected<int, std::string> checkArgv
(
    ArgumentData ad
)
{
    
    if (ad.f.string().length() == 0) {
        return std::unexpected("No filename submitted.");
    }

    if (ad.v.length() == 0) {
        return std::unexpected("No values submitted.");
    }

    if (!std::filesystem::exists(ad.f)) {
        std::cout << ad.f << "\n";
        return std::unexpected("Table does not exist.");
    }

    std::fstream f;

    f.open(ad.f, std::ios::app);
    
    if (!f.is_open()) {
        return std::unexpected("Failed to open table.");
    }

    f << ad.v << "," << ad.b << "," << 0 << "\n"; // Append the data to the end of the file.

    /*
    Read first line of file to get total arguments count for later 
    determination if the amount of values uploaded was incorrect.
    but i'm only doing this cause I am just learning how to do this, there is no point if the,
    database continues to be create from stack allocated values, instead of argc/argv or files.
    I mean a way to dynamically add a table to the group of tables tracked by this application.
    I added checking for this in the main function (for now) by checking the number of values
    found in the value count.
    So here it is: !
    std::string firstLine;
    std::getline(f, firstLine);
    std::cout << firstLine << "\n";
    */
    return 1;
}

// Poor implementation.
// Work on this.
ArgumentData parseArgv
(
    int argc,
    char **argv
)
{
    ArgumentData ad;
    std::string lastArgument = "";
    int indexStarted = -1;
    ad.vc = 0;

    for (int i = 0; i < argc; i++) {

        std::string arg = argv[i];

        // Get filename
        if (arg == "-f") {
            std::filesystem::path filename = argv[i + 1];
            filename += ".csv";
            ad.f = "database" / filename;
            
            lastArgument = "-f";
            indexStarted = i;
        }

        else if (arg == "-by") {
            lastArgument = "-by";
            indexStarted = i;
            std::string by = argv[i + 1];
            ad.b = by;
        }

        else if (arg == "-do") {
            lastArgument = "-do";
            indexStarted = i;
        }

        else if (lastArgument == "-do" && indexStarted != -1) {
            if (indexStarted + 1 == i) {
                ad.v += arg;
            } else {
                ad.v += " " + arg;
            }
            ad.vc += 1;
        }

    }

    return ad;
}

// Not used.
std::expected<int, std::string> updateTable
(
    std::string table,
    std::vector<std::string> thing
)
{
    std::cout << "Table: " << table << "\n";
    std::cout << "Thing: ";
    for (const auto& element : thing) {
        std::cout << element << " ";
    }
    return 1;
}

std::expected<int, std::string> createTable
(
    std::filesystem::path path, 
    std::vector<std::string> columns
)
{
    std::fstream s;

    if (std::filesystem::exists(path)) {
        return 1;
    }

    s.open(path, std::ios::app);

    if (!s.is_open()) {
        return std::unexpected("Unable to create table.");
    }

    for (int i = 0; i < columns.size(); i++) {
        if (i != 0) {
            s << ",";
        }
        s << columns[i];
    }
    s << "\n";

    // s.close(); // Not required due to RAII.

    return 1;
}

std::expected<int, std::string> createDirectory
(
    std::filesystem::path directory
)
{
    if (!std::filesystem::is_directory(directory)) {
        bool dbc = std::filesystem::create_directory(directory);

        if (!dbc) {
            return std::unexpected("Failed to create directory." + directory.string());
        }
    }

    return 1;
}

std::expected<int, std::string> initializeDatabase
(
    std::filesystem::path directory,
    std::unordered_map<std::string, std::vector<std::string>> files
)
{
    auto cd = createDirectory(directory);

    if (!cd) {
        std::cout << cd.error() << "\n";
        return std::unexpected("There was an error when initializing the \'database.\'");
    }

    for (const auto& file : files) {
        std::fstream s;
        std::filesystem::path db1p = directory / file.first;
        
        auto ct = createTable(db1p, file.second);
        
        if (!ct) {
            std::cerr << ct.error() << "\n";
            return std::unexpected("There was an error when creating the table " + file.first);
        }
    }

    return 1;
}

std::expected<std::vector<std::string>, std::string> getTableColumns
(
    std::unordered_map<std::string, std::vector<std::string>> fdv,
    std::string filename
)
{
    for (const auto& fd : fdv) {
        if (fd.first == filename) {
            return fd.second;
        }
    }

    return std::unexpected("Table does not exist!");
}

// Research to develop a better system for parsing argv
// AND
// executing commands in an ideal order
int main
(
    int argc,
    char **argv
)
{
    std::filesystem::path dbp = "database";
    std::unordered_map<std::string, std::vector<std::string>> fdv;
    fdv["todo.csv"] = { "do", "by", "completed" };

    auto id = initializeDatabase(dbp, fdv);

    if (!id) {
        std::cerr << id.error() << "\n";
        return 1;
    }

    ArgumentData parsed = parseArgv(argc, argv);
    auto checked = checkArgv(parsed);

    if (!checked) {
        std::cerr << "Error: " << checked.error() << "\n";
        return 1;
    }

    std::cout << "Successfully added new row to " << parsed.f.filename().string() << "." << "\n";

    return 0;
}