#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include <expected>
#include <chrono>

struct FileData {
    std::filesystem::path filename;
    std::vector<std::string> columns;
};

struct ArgumentData {
    std::filesystem::path f; // filename
    std::string v; // values
    int vc; // values count;
};

class ArgumentHandler {
    private:
        ArgumentData ad;
    public:
        ArgumentHandler() { std::cout << "Arguments Initialized" << "\n"; }
        ~ArgumentHandler() { std::cout << "Arguments Destroyed" << "\n"; }
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
        return std::unexpected("Table does not exist.");
    }

    std::fstream f;

    f.open(ad.f, std::ios::app);
    
    if (!f.is_open()) {
        return std::unexpected("Failed to open table.");
    }

    f << ad.v << "\n"; // Append the data to the end of the file.

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

        else if (arg == "-v") {
            lastArgument = "-v";
            indexStarted = i;
        }

        else if (lastArgument == "-v" && indexStarted != -1) {
            if (indexStarted + 1 == i) {
                ad.v += arg;
            } else {
                ad.v += "," + arg;
            }
            ad.vc += 1;
        }

    }

    return ad;
}

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

int initializeDatabase
(
    std::filesystem::path directory,
    std::vector<FileData> filenames
)
{
    auto cd = createDirectory(directory);

    if (!cd) {
        std::cout << cd.error() << "\n";
        return 0;
    }

    for (const auto& file : filenames) {
        std::fstream s;
        std::filesystem::path db1p = directory / file.filename;
        
        auto ct = createTable(db1p, file.columns);
        
        if (!ct) {
            std::cerr << ct.error() << "\n";
            return 0;
        }
    }

    return 1;
}

std::expected<std::vector<std::string>, std::string> getTableColumns
(
    std::vector<FileData> fdv,
    std::string filename
)
{
    for (const auto& fd : fdv) {
        if (fd.filename == filename) {
            return fd.columns;
        }
    }

    return std::unexpected("Table does not exist!");
}

int main
(
    int argc,
    char **argv
)
{
    std::filesystem::path dbp = "database";
    std::vector<FileData> fdv = {
        { "todo.csv", { "do", "by" } }
    };

    if (!initializeDatabase(dbp, fdv)) {
        std::cerr << "There was an error when initializing the \'database.\'" << "\n";
        return 1;
    }

    ArgumentData parsed = parseArgv(argc, argv);
    auto columns = getTableColumns(fdv, parsed.f.filename().string());

    if (!columns) {
        std::cerr << columns.error() << "\n";
        return 1;
    }

    int columnSize = columns.value().size();

    std::cout << columnSize << " " << parsed.vc << "\n";

    if (columnSize != parsed.vc) {
        if (columnSize > parsed.vc) {
            std::cerr << "Error: Too few arguments passed." << "\n";
        } else if (columnSize < parsed.vc) {
            std::cerr << "Error: Too many arguments passed." << "\n";
        }
        return 1;
    }

    auto checked = checkArgv(parsed);

    if (!checked) {
        std::cerr << "Error: " << checked.error() << "\n";
        return 1;
    }

    return 0;
}