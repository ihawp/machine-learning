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
#include "headers/logistic_regression.hpp"
#include "headers/model_types.hpp"
#include "headers/csv_utils.hpp"
#include "headers/log.hpp"

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
    const std::string &directory,
    std::string &name,
    std::vector<std::string> &data,
    const std::vector<std::string> &columnNames
)
{

    std::filesystem::path standardDirectory("log");

    if (!createDirectory(standardDirectory)) return false;

    standardDirectory.append(directory);

    if (!createDirectory(standardDirectory)) return false;

    std::string filename = name + ".csv";
    std::filesystem::path filepath = standardDirectory / filename;

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

void prepareFeaturesAndResponsesForLR
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

bool loadDataForTrainLR
(
    std::string &directory,
    std::string &filename,
    arma::fmat &trainData
)
{
    std::string trainPath = directory + "/" + filename;

    try {
        bool trainDataLoaded = mlpack::data::Load
        (
            trainPath,
            trainData,
            true
        ); 

        if (!trainDataLoaded) {
            throw std::runtime_error("Unable to load training data.");
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return false;
    }

    return true;
}

bool loadDataForTrainLOR
(
    std::string &directory,
    std::string &filename,
    arma::mat &trainData,
    arma::Row<size_t> &trainLabels
)
{
    std::string trainDataPath = directory + "/" + filename + "a.csv";
    std::string trainLabelsPath = directory + "/" + filename + "a_threshold.csv";

    try {
        bool trainDataLoaded = mlpack::data::Load
        (
            trainDataPath,
            trainData,
            true
        ); 

        if (!trainDataLoaded) {
            throw std::runtime_error("Unable to load training data.");
        }

        bool trainLabelsLoaded = mlpack::data::Load
        (
            trainLabelsPath,
            trainLabels,
            true
        ); 

        if (!trainLabelsLoaded) {
            throw std::runtime_error("Unable to load training labels.");
        }

    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return false;
    }

    return true;

}


// Load a model from the models/ folders.
// The log function sets a base folder of log/
// I think I will do the same with the saveModels/ functions.
bool collectArgumentsForLM
(
    int &argc,
    char **argv,
    std::string &directory,
    std::string &modelName
)
{
    if (argc < 4) {
        std::cerr << "Too few arguments for loading a model." << "\n";
        return false;
    }

    try {
        directory = argv[2];
        modelName = argv[3];
        return true;
    }
    catch (const std::runtime_error& e) {
        std::cout << e.what() << "\n";
        return false;
    }
}

// SHARED core parser
struct LRConfig {
    std::string directory, filename, modelName;
    int responseColumn;
    std::optional<float> lambda;  // optional for test
};

LRConfig parseLRArgs(int argc, char** argv) {
    // Common parsing logic for dir, file, model, col
    // lambda is optional
    try {
        
    }
}

// TRAIN wrapper (needs lambda)
bool collectArgumentsForTrainLR(int argc, char** argv, /* put ouputs here */) {
    auto cfg = parseLRArgs(argc, argv);

    if (!cfg.lambda.has_value() || argc < 8) {
        std::cerr << "Train needs lambda\n";
        return false;
    }
    
    // put outputs here
    
    return true;
}

bool collectArgumentsForTestLR(int argc, char** argv, /* put ouputs here */) {
    auto cfg = parseLRArgs(argc, argv);
    
    if (cfg.lambda.has_value()) {
        std::cerr << "Test ignores lambda\n";
    }

    if (argc < 6) {
        std::cerr << "Test usage...\n";
        return false;
    }

    // put outputs here

    return true;
}

bool collectArgumentsForTrainLR
(
    int &argc,
    char **argv,
    std::string &directory,
    std::string &filename,
    std::string &modelName,
    int &responsesColumn,
    float &lambda
)
{

    if (argc < 8) {
        std::cerr << "Too few arguments" << "\n";
        std::cerr << "Usage: " << "a.exe -lr <dir: string> <file1: string> <file2: string> <modelName: string> <responseColumn: int> <lambdaInt: int>\n";
        return false;
    };

    try {
        directory = argv[2]; // directory containing the files
        filename = argv[3]; // name of file you want to use for training.
        modelName = argv[4]; // The name of the model for output in /models when the model is saved. Could be name of csv file as well.
        responsesColumn = std::stoi(argv[5]); // Choose response column for submitted files
        lambda = std::stof(argv[6]) / 100.0000f; // Pass as integer value (0 -> 100...200!?)
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

bool collectArgumentsForTrainLOR
(
    int &argc,
    char **argv,
    std::string &directory,
    std::string &filename,
    std::string &modelName,
    double &lambda
)
{

    if (argc < 6) {
        std::cerr << "Too few arguments." << "\n";
        std::cerr << "Usage: " << "a.exe -lor <dir: string> <file: string> <modelName: string> <lambdaInt: int>\n";
        return false;
    };

    try {
        directory = argv[2]; // directory containing the files
        filename = argv[3];
        modelName = argv[4]; // The name of the model for output in /models when the model is saved. Could be name of csv file as well.
        lambda = std::stod(argv[5]) / 100.00000f; // Pass as integer value (0 -> 100...200!?)
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

// Both this function and the logisticRegression function too long and should be split up.
// Both train a model with a certain lambda (and the required datasets)
// AND then test the model and output some results to the log/.../...csv file
// You should be able to TRAIN a model, and then TEST a model (by loading it).

// So we should create that pathway, where a model can be passed to a test function + some data.
// The current approach using argc and argv as arguments is okay, but this signature should
// not be called from the loadModel function, instead what I am considering an after halfway (through
// the below function) function should be called from the loadModel function that acts on a trained
// model of a certain type, whether that is from testing after training (in the same process), or testing
// after training in a follow up process.
bool trainLinearRegression
(
    int &argc,
    char **argv
)
{
    std::string directory;
    std::string filename;
    std::string modelName;
    int responsesColumn;
    float lambda;

    if (!collectArgumentsForTrainLR(
        argc,
        argv,
        directory,
        filename,
        modelName,
        responsesColumn,
        lambda
    ))
    {
        std::cerr << "Failed to collect arguments." << "\n";
        return false;
    }

    arma::fmat trainData;

    bool dataLoaded = loadDataForTrainLR
    (
        directory,
        filename,
        trainData
    );

    if (!dataLoaded) return false;

    arma::fmat trainDataset;
    arma::frowvec trainResponses;

    prepareFeaturesAndResponsesForLR
    (
        trainData,
        trainDataset,
        trainResponses,
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

    static const std::string modelTypeName = "linear_regression";

    bool modelSaved = saveModel
    (
        model,
        modelTypeName,
        modelName
    );

    if (!modelSaved) {
        std::cerr << "Unable to save model, but it did really well, congrats!" << "\n";
        return false;
    }

    return true;
}

bool trainLogisticRegression
(
    int &argc,
    char **argv
)
{
    std::string directory, filename, modelName;
    double lambda;

    if (!collectArgumentsForTrainLOR(
        argc,
        argv,
        directory,
        filename,
        modelName,
        lambda
    )) return false;

    arma::mat trainData;
    arma::Row<size_t> trainLabels;

    if (!loadDataForTrainLOR(
        directory,
        filename,
        trainData,
        trainLabels
    )) return false;

    arma::rowvec trainWeights(trainData.n_rows + 1, arma::fill::zeros);
    trainWeights(0) = 0.5;

    mlpack::LogisticRegression<arma::mat> lor;

    lor.Lambda() = lambda;
    lor.Parameters() = trainWeights;
    lor.Train(
        trainData,
        trainLabels
    );

    std::cout << "Model Trained." << "\n";

    static const std::string modelTypeName = "logistic_regression";

    bool modelSaved = saveModel
    (
        lor,
        modelTypeName,
        modelName
    );

    if (!modelSaved) {
        return false;
    }

    return true;
}

bool testLinearRegression
(
    int &argc,
    char **argc
)
{

    std::string &filename,
    std::string &modelName,
    const std::string &modelTypeName = "linear_regression";
    std::string modelName;
    mlpack::LinearRegression<arma::fmat> model;
    arma::fmat testDataset;
    arma::frowvec testResponses;

    // load the data from the file, prepare it on the proper index.

    // Mean Squared Error (How low can you go?)
    float testMSE = model.ComputeError
    (
        testDataset, 
        testResponses
    );
    
    std::string testMSEString = std::to_string(testMSE);
    std::string lambdaString = std::to_string(model.Lambda());
    
    std::vector<std::string> data = {
        filename,
        lambdaString,
        testMSEString,
    };

    static const std::vector<std::string> columnNames = {
        "type_of_run",
        "lambda",
        "mse"
    };

    // Capture recorded data in log/(modelTypeName)/(modelName).csv
    // data and columnNames should always be the same length.
    bool logged = log
    (
        modelTypeName,
        modelName,
        data,
        columnNames
    );

    if (!logged) {
        std::cerr << "Failed to log run." << "\n"; 
        return false;
    }

}

bool testLogisticRegression
(
    mlpack::LogisticRegression<arma::mat> &lor,
    arma::mat &testData,
    arma::Row<size_t> &testLabels,
    float &lambda,
    std::string &filename
)
{
    double accuracy = lor.ComputeAccuracy(testData, testLabels);

    arma::Row<size_t> predictions;
    lor.Classify(testData, predictions);

    // Data and columns should always be the same length.
    std::vector<std::string> data = {
        filename,
        std::to_string(lambda),
        std::to_string(accuracy)
    };

    static const std::vector<std::string> columnNames = {
        "name",
        "lambda",
        "accuracy",
    };

    static const std::string modelTypeName = "logistic_regression";

    if (!log(
        modelTypeName,
        filename,
        data,
        columnNames
    )) {
        std::cerr << "Unable to log logistic regression." << "\n";
        return false;
    }

    return true;
}


// Should be for multiple actions.
bool loadModel
(
    int &argc,
    char **argv
)
{
    std::cout << "Loading Model." << "\n";
    // there was so much text here before, jealous?
    return true;
}

bool saveModel
(
    mlpack::LinearRegression<arma::fmat> &model,
    const std::string &directory,
    std::string &modelName
)
{
    std::filesystem::path modelsPath = "models";

    if (!createDirectory(modelsPath)) {
        std::cerr << "Failed to create models directory" << "\n";
        return false;
    }

    std::filesystem::path directoryPath = modelsPath / directory;

    if (!createDirectory(directoryPath)) {
        std::cerr << "Failed to create directory withing models/" << "\n";
        return false;
    }

    try {
        bool modelSaved = mlpack::data::Save(
            directoryPath.string() + "/" + modelName + ".bin",
            modelName,
            model,
            true,
            mlpack::data::format::binary
        );
        
        if (!modelSaved) {
            throw std::runtime_error("Failed to save model.");
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
    mlpack::LogisticRegression<arma::mat> &model,
    const std::string &directory,
    std::string &modelName
)
{
    std::filesystem::path modelsPath = "models";

    if (!createDirectory(modelsPath)) {
        std::cerr << "Failed to create models directory" << "\n";
        return false;
    }

    std::filesystem::path directoryPath = modelsPath / directory;

    if (!createDirectory(directoryPath)) {
        std::cerr << "Failed to create directory withing models/" << "\n";
        return false;
    }

    try {
        bool modelSaved = mlpack::data::Save(
            directoryPath.string() + "/" + modelName + ".bin",
            modelName,
            model,
            true,
            mlpack::data::format::binary
        );
        
        if (!modelSaved) {
            throw std::runtime_error("Failed to save model.");
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return false;
    }

    return true;
}

ArgumentType findArgumentType
(
    std::string type
)
{
    static const std::map<std::string, ArgumentType> modelTypeMap = {
        { "-lm", ArgumentType::LoadModel },
        { "-lr", ArgumentType::LinearRegression },
        { "-lor", ArgumentType::LogisticRegression },
        { "-tlr", ArgumentType::TestLinearRegression },
        { "-tlor", ArgumentType::TestLogisticRegression }
    };

    auto modelTypeFound = modelTypeMap.find(type);

    if (modelTypeFound != modelTypeMap.end()) {
        return modelTypeFound->second;
    }

    return ArgumentType::None;
}

int main
(
    int argc,
    char **argv
)
{

    if (argc < 2) return 1;

    std::string type = argv[1];

    ArgumentType modelType = findArgumentType(type);

    switch (modelType) {

        case ArgumentType::LoadModel:
            if (!loadModel(argc, argv)) {
                std::cerr << "Failed to load model for testing." << "\n";
                return 1;
            }
            break;

        case ArgumentType::LinearRegression:
            if (!trainLinearRegression(argc, argv)) {
                std::cerr << "Linear Regression Training Failed." << "\n";
                return 1;
            }
            break;

        case ArgumentType::TestLinearRegression:
            if (!testLinearRegression(argc, argv)) {
                std::cerr << "Failed to test linear regression." << "\n";
                return 1;
            }
            break;

        case ArgumentType::LogisticRegression:
            if (!trainLogisticRegression(argc, argv)) {
                std::cerr << "Logistic Regression Training Failed." << "\n";
                return 1;
            }
            break;

        case ArgumentType::TestLogisticRegression:
            if (!testLogisticRegression(argc, argv)) {
                std::cerr << "Failed to test linear regression." << "\n";
                return 1;
            }
            break;

        case ArgumentType::None:
        default:
            std::cout << "No model type found." << "\n";
            return 1;
            break;
    }

    return 0;
}