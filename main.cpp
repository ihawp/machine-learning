#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <mlpack/methods/linear_regression/linear_regression.hpp>

/*

Matrices:

    [  3,   1,   2,   4  ]
A = [  1,  -3,   4,   2  ]
    [  2,   6,  -7,   3  ]

    [  4,   12,   2,   4  ]
B = [  2,   -5,  -2,   1  ]
    [  16,   6,   2,   2  ]

A + B = possible
A - B = possible
B - A = possible
AB    = impossible
BA    = impossible

    [  3,   1,   2,   4  ]
A = [  1,  -3,   4,   2  ]
    [  2,   6,  -7,   3  ]

    [  4,   12,   2  ]
B = [  2,   -5,  -2  ]
    [  16,   6,   2  ]

A + B = impossible
A - B = impossible
B - A = impossible
AB    = impossible
BA    = possible (3x4)

(3x4) = rows by columns (RxC)

They can be divided by multiplying by the inverse of another...but it
sounds like that stuff will matter later. Or maybe not at all in this context.

I also don't yet know how to find the inverse :)

Complex, Hermitian, Unitary !?

*/

int updateCSV
(
    std::filesystem::path path,
    std::vector<std::string> data
)
{

    std::fstream s;
    s.open(path, std::ios::app);

    if (!s.is_open()) {
        std::cerr << "Unable to open file." << "\n";
        return 0;
    }

    for (int i = 0; i < data.size(); i++) {
        if (i != 0) {
            s << ",";
        }
        s << data[i];
    }
    s << "\n";

    // s.close(); // Not required due to RAII.

    return 1;
}

int createCSV
(
    std::filesystem::path path,
    std::vector<std::string> columns
)
{
    if (!std::filesystem::exists(path)) {
        if (!updateCSV(path, columns)) {
            return 0;
        }
    }

    return 1;
}

int createDirectory
(
    std::filesystem::path path
)
{
    if (!std::filesystem::is_directory(path)) {
        bool dbc = std::filesystem::create_directory(path);

        if (!dbc) {
            std::cerr << "Failed to create directory " << path << "\n";
            return 0;
        }
    }

    return 1;
}

int logRun
(
    std::vector<std::string> &data
)
{

    std::filesystem::path directory = "log";

    if (!createDirectory(directory)) {
        return 0;
    }

    std::filesystem::path filepath = directory / "run.csv";

    if (!std::filesystem::exists(filepath)) {
        createCSV(filepath, {
            "train",
            "test",
            "lambda",
            "training_mse",
            "test_mse"
        });
    }

    if (!updateCSV(filepath, data)) {
        return 0;
    }

    return 1;
}

void prepareFeaturesAndResponses
(
    const arma::fmat &fullData,
    arma::fmat &features,
    arma::frowvec &responses
)
{
    const int HAPPINESS_SCORE_INDEX = 1;

    // Y
    responses = fullData.row(HAPPINESS_SCORE_INDEX);

    // X
    features = arma::join_cols(
        fullData.rows(0, 0),
        fullData.rows(2, fullData.n_rows - 1)
    );
}

void prepareFeaturesAndResponses2
(
    const arma::fmat &fullData,
    arma::fmat &features,
    arma::frowvec &responses
)
{
    const int HAPPINESS_SCORE_INDEX = 0;

    // Y
    responses = fullData.row(HAPPINESS_SCORE_INDEX);

    // X
    features = fullData.rows(1, fullData.n_rows - 1);
}

int linearRegression
(
    std::string &directory,
    std::string &filename1,
    std::string &filename2,
    float &lambda
)
{
    std::string trainPath = directory + "/" + filename1;
    std::string testPath = directory + "/" + filename2;

    arma::fmat trainData;
    arma::fmat testData;

    // true for transpose
    bool trainDataLoaded = mlpack::data::Load(trainPath, trainData, true); 

    if (!trainDataLoaded) {
        std::cerr << "Unable to load training data." << "\n";
        return 0;
    }

    bool testDataLoaded = mlpack::data::Load(testPath, testData, true);
    
    if (!testDataLoaded) {
        std::cerr << "Unable to load test data." << "\n";
        return 0;
    }

    arma::frowvec trainResponses, testResponses;
    arma::fmat trainDataset, testDataset;

    prepareFeaturesAndResponses2(trainData, trainDataset, trainResponses);
    prepareFeaturesAndResponses2(testData, testDataset, testResponses);

    mlpack::LinearRegression<arma::fmat> lr;
    lr.Lambda() = lambda;
    lr.Train(trainDataset, trainResponses);

    // Mean Squared Error (How low can you go?)
    float trainingMSE = lr.ComputeError(trainDataset, trainResponses);
    float testMSE = lr.ComputeError(testDataset, testResponses);

    // Capture recorded data in log/run.csv
    std::vector<std::string> data = {
        filename1,
        filename2,
        std::to_string(lr.Lambda()),
        std::to_string(trainingMSE),
        std::to_string(testMSE)
    };

    if (!logRun(data)) {
        std::cerr << "Failed to log run." << "\n"; 
        return 0;
    }

    return 1;
}

int main
(
    int argc,
    char **argv
)
{
    if (argc < 4) {
        std::cerr << "Too few arguments" << "\n";
        return 1;
    };

    std::string directory = argv[1]; // directory containing the files
    std::string filename1 = argv[2];
    std::string filename2 = argv[3];
    float lambda = std::stof(argv[4]) / 100.0000f; // passed as integer value because batch is crazy compared to c++

    linearRegression(directory, filename1, filename2, lambda);

    return 0;
}