#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <armadillo>
#include <mlpack/core.hpp>
#include <mlpack/methods/linear_regression/linear_regression.hpp>
#include <mlpack/methods/logistic_regression/logistic_regression.hpp>
#include "headers/linear_regression.hpp"
#include "headers/model_types.hpp"
#include "headers/csv_utils.hpp"
#include "headers/log.hpp"
#include "headers/regression_models.hpp"

using namespace arma;
using namespace mlpack;

bool updateCSV
(
    std::filesystem::path path,
    std::vector<std::string> data
)
{

    std::fstream s;
    s.open(path, std::ios::app);

    if (!s.is_open()) {
        std::cerr << "Unable to open file." << "\n";
        return false;
    }

    for (int i = 0; i < data.size(); i++) {
        if (i != 0) {
            s << ",";
        }
        s << data[i];
    }
    s << "\n";

    // s.close(); // Not required due to RAII.

    return true;
}

bool createCSV
(
    std::filesystem::path path,
    std::vector<std::string> columns
)
{
    if (!std::filesystem::exists(path)) {
        if (!updateCSV(path, columns)) {
            return false;
        }
    }

    return true;
}

bool createDirectory
(
    std::filesystem::path path
)
{
    if (!std::filesystem::is_directory(path)) {
        if (!std::filesystem::create_directory(path)) {
            std::cerr << "Failed to create directory " << path << "\n";
            return false;
        }
    }

    return true;
}

bool log
(
    std::string &name,
    std::vector<std::string> &data,
    std::vector<std::string> &columnNames
)
{

    std::filesystem::path directory = "log";

    if (!createDirectory(directory)) return false;

    std::string filename = name + ".csv";
    std::filesystem::path filepath = directory / filename;

    // Create the csv file first if it does not exist.
    if (!createCSV(filepath, columnNames)) {
        std::cerr << "Unable to create non-existent CSV in log()" << "\n";
        return false;
    }

    if (!updateCSV(filepath, data)) {
        std::cerr << "Unable to update CSV in log()" << "\n";
        return false;
    }

    return true;
}

void prepareFeaturesAndResponses
(
    const arma::fmat &fullData,
    arma::fmat &features,
    arma::frowvec &responses,
    int &responsesColumn
)
{
    // Y
    responses = fullData.row
    (
        responsesColumn
    );

    // X
    if (responsesColumn == 0) {

        features = fullData.rows
        (
            1,
            fullData.n_rows - 1
        );

    } else {

        features = arma::join_cols
        (
            fullData.rows(0, responsesColumn - 1),
            fullData.rows(responsesColumn + 1, fullData.n_rows - 1)
        );
    
    }
}

bool loadDataForLR
(
    std::string &directory,
    std::string &filename1,
    std::string &filename2,
    arma::fmat &trainData,
    arma::fmat &testData
)
{
    std::string trainPath = directory + "/" + filename1;
    std::string testPath = directory + "/" + filename2;

    try {
        bool trainDataLoaded = mlpack::data::Load
        (
            trainPath,
            trainData,
            true
        ); 

        if (!trainDataLoaded) {
            std::cerr << "Unable to load training data." << "\n";
            return false;
        }

        bool testDataLoaded = mlpack::data::Load
        (
            testPath,
            testData,
            true
        );
        
        if (!testDataLoaded) {
            std::cerr << "Unable to load test data." << "\n";
            return false;
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return false;
    }

    return true;
}

bool saveModel
(
    mlpack::LinearRegression<arma::fmat> model,
    std::string modelName
)
{
    std::filesystem::path modelsPath = "models";

    if (!createDirectory(modelsPath)) {
        std::cerr << "Failed to create models directory" << "\n";
        return false;
    }
    
    try {
        bool modelSaved = mlpack::data::Save(
            "models/" + modelName + ".bin",
            modelName,
            model,
            true,
            mlpack::data::format::binary
        );
        
        if (!modelSaved) {
            std::cerr << "Failed to save model." << "\n";
            return false;
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return false;
    }

    return true;
}

bool collectArgumentsForLR
(
    int &argc,
    char **argv,
    std::string &directory,
    std::string &filename1,
    std::string &filename2,
    std::string &modelName,
    int &responsesColumn,
    float &lambda
)
{

    if (argc < 8) {
        std::cerr << "Too few arguments" << "\n";
        std::cerr << "Usage: " << "a.exe <dir: string> <file1: string> <file2: string> <modelName: string> <responseColumn: int> <lambdaInt: int>\n";
        return false;
    };

    try {
        directory = argv[2]; // directory containing the files
        filename1 = argv[3];
        filename2 = argv[4];
        modelName = argv[5]; // The name of the model for output in /models when the model is saved. Could be name of csv file as well.
        responsesColumn = std::stoi(argv[6]); // Choose response column for submitted files
        lambda = std::stof(argv[7]) / 100.0000f; // Pass as integer value (0 -> 100...200!?)
    }
    catch (const std::out_of_range& e) {
        std::cerr << e.what() << "\n";
        return false;
    }
    catch (const std::invalid_argument& e) {
        std::cerr << e.what() << "\n";
        return false;
    }

    return true;
}

bool linearRegression
(
    int &argc,
    char **argv
)
{
    
    std::string directory;
    std::string filename1;
    std::string filename2;
    std::string modelName;
    int responsesColumn;
    float lambda;

    if (!collectArgumentsForLR(
        argc,
        argv,
        directory,
        filename1,
        filename2,
        modelName,
        responsesColumn,
        lambda
    ))
    {
        std::cerr << "Failed to collect arguments." << "\n";
        return false;
    }

    arma::fmat trainData, testData;

    bool dataLoaded = loadDataForLR
    (
        directory,
        filename1,
        filename2,
        trainData,
        testData
    );

    if (!dataLoaded) return false;

    arma::fmat trainDataset, testDataset;
    arma::frowvec trainResponses, testResponses;

    prepareFeaturesAndResponses
    (
        trainData,
        trainDataset,
        trainResponses,
        responsesColumn
    );

    prepareFeaturesAndResponses
    (
        testData,
        testDataset,
        testResponses,
        responsesColumn
    );

    mlpack::LinearRegression<arma::fmat> model;

    model.Train
    (
        trainDataset,
        trainResponses,
        lambda,
        true
    );

    // Mean Squared Error (How low can you go?)
    float trainingMSE = model.ComputeError
    (
        trainDataset,
        trainResponses
    );

    float testMSE = model.ComputeError
    (
        testDataset, 
        testResponses
    );

    std::string lambdaString = std::to_string(lambda);
    std::string trainingMSEString = std::to_string(trainingMSE);
    std::string testMSEString = std::to_string(testMSE);

    std::vector<std::string> data = {
        filename1,
        filename2,
        lambdaString,
        trainingMSEString,
        testMSEString
    };

    std::vector<std::string> columnNames = {
        "train",
        "test",
        "lambda",
        "training_mse",
        "test_mse"
    };

    // Capture recorded data in log/(modelName).csv
    // data and columnNames should always be the same length.
    bool logged = log
    (
        modelName, 
        data, 
        columnNames
    );

    if (!logged) {
        std::cerr << "Failed to log run." << "\n"; 
        return false;
    }

    bool modelSaved = saveModel
    (
        model, 
        modelName
    );

    if (!modelSaved) {
        std::cerr << "Unable to save model, but it did really well, congrats!" << "\n";
        return false;
    }

    return true;
}

bool logisticRegression
(
    int &argc,
    char **argv
)
{
    std::cout << "Logistically Regressing" << "\n";

    arma::mat data;
    arma::Row<size_t> labels;
    arma::rowvec initialPoint;
    // ensmallen optimizer
    double lambda;
    // ensmallen callbacks

    mlpack::LogisticRegression<arma::dmat> lor;

    lor.Train(
        data,
        labels
    );


    return true;
}

ModelType findModelType
(
    int &argc,
    char **argv
)
{
    if (argc < 2) return ModelType::None;

    std::string type = argv[1];

    // Initialize once.
    static const std::map<std::string, ModelType> modelTypeMap = {
        { "-lr", ModelType::LinearRegression },
        { "-lor", ModelType::LogisticRegression }
    };

    auto modelTypeFound = modelTypeMap.find(type);

    if (modelTypeFound != modelTypeMap.end()) {
        return modelTypeFound->second;
    }

    return ModelType::None;
}

int main
(
    int argc,
    char **argv
)
{

    ModelType modelType = findModelType(argc, argv);

    switch (modelType) {
        case ModelType::LinearRegression: {
            if (!linearRegression(argc, argv)) {
                std::cerr << "Linear Regression Failed" << "\n";
                return 1;
            }
            break;
        }
        case ModelType::LogisticRegression: {
            if (!logisticRegression(argc, argv)) {
                std::cerr << "Logistic Regression Failed" << "\n";
                return 1;
            }
            break;
        }
        default: {
            std::cout << "No model type found." << "\n";
            return 1;
        }
    }

    return 0;
}