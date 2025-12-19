#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <optional>
#include <armadillo>
#include <mlpack/core.hpp>
#include <mlpack/methods/linear_regression/linear_regression.hpp>
#include <mlpack/methods/logistic_regression/logistic_regression.hpp>
#include "headers/arguments.hpp"
#include "headers/linear_regression.hpp"
#include "headers/load_model.hpp"
#include "headers/log.hpp"
#include "headers/logistic_regression.hpp"
#include "headers/template.hpp"

using namespace arma;
using namespace mlpack;

// ##############################################################################
// Filesystem

bool updateCSV
(
    std::filesystem::path &path,
    std::vector<std::string> &data
)
{
    std::fstream s;
    s.open(path, std::ios::app);

    if (!s.is_open()) {
        std::cerr << "Unable to open file." << "\n";
        return false;
    }

    for (int i = 0; i < data.size(); i++) {
        if (i) s << ",";
        s << data[i];
    }
    s << "\n";

    return true;
}

bool createCSV
(
    std::filesystem::path &path,
    std::vector<std::string> &columns
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
    std::filesystem::path &path
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
    std::vector<std::string> &columnNames
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

// ##############################################################################
// Linear Regression

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
    std::string &filename,
    arma::fmat &data
)
{
    std::string path = directory + "/" + filename;

    try {
        loadData(path, data);
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
    LRArguments &arguments
)
{

    if (argc < 7) {
        std::cerr << "Too few arguments" << "\n";
        std::cerr << "Usage: " << "a.exe -lr <dir: string> <file1: string> <file2: string> <modelName: string> <responseColumn: int> <lambdaInt: int>\n";
        return false;
    };

    try {
        arguments.directory = argv[3]; // directory containing the files
        arguments.filename = argv[4];
        arguments.modelName = argv[5]; // The name of the model for output in /models when the model is saved. Could be name of csv file as well.
        arguments.responsesColumn = std::stoi(argv[6]); // Choose response column for submitted files

        if (argc > 7) {
            arguments.lambda = std::stof(argv[7]) / 100.0000f; // Pass as integer value (0 -> 100...200!?)
        }
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

bool fillArgumentsForLR
(
    int &argc,
    char **argv,
    arma::fmat &dataset,
    arma::frowvec &responses,
    LRArguments &arguments
)
{
    // No points in collecting directory or filename on this or LOR version. I will change later <3
    if (!collectArgumentsForLR(
        argc,
        argv,
        arguments
    )) {
        std::cerr << "Unable to collect arguments for Linear Regression." << "\n";
        return false;
    }

    arma::fmat data;

    if (!loadDataForLR(
        arguments.directory,
        arguments.filename,
        data
    )) return false;

    prepareFeaturesAndResponses
    (
        data,
        dataset,
        responses,
        arguments.responsesColumn
    );

    return true;
}

bool trainLinearRegression
(
    int &argc,
    char **argv
)
{
    arma::fmat dataset;
    arma::frowvec responses;

    LRArguments arguments;
    
    if (!fillArgumentsForLR(
        argc,
        argv,
        dataset,
        responses,
        arguments
    )) {
        std::cerr << "Unable to fill arguments for testing Linear Regression." << "\n";
        return false;
    }

    mlpack::LinearRegression<arma::fmat> model;

    // will explicitly check
    if (!arguments.lambda.has_value()) {
        std::cerr << "Unable to find lambda value." << "\n";
        return false;
    }

    model.Train
    (
        dataset,
        responses,
        arguments.lambda,
        true
    );

    bool modelSaved = saveModel
    (
        model,
        "linear_regression",
        arguments.modelName
    );

    if (!modelSaved) {
        std::cerr << "Unable to save model, but it did really well, congrats!" << "\n";
        return false;
    }

    return true;
}


bool testLinearRegression
(
    int &argc,
    char **argv
)
{

    arma::fmat dataset;
    arma::frowvec responses;
    LRArguments arguments;

    if (!fillArgumentsForLR(
        argc,
        argv,
        dataset,
        responses,
        arguments
    )) {
        std::cerr << "Unable to fill arguments for testing Linear Regression." << "\n";
        return false;
    }

    mlpack::LinearRegression<arma::fmat> model;
    std::string path = "models/linear_regression/" + arguments.modelName + ".bin";

    if (!mlpack::data::Load(
        path,
        arguments.modelName,
        model,
        false,
        data::format::binary
    )) {
        std::cerr << "Unable to load model." << "\n";
        return false;
    }

    // Mean Squared Error (How low can you go?)
    float MSE = model.ComputeError
    (
        dataset,
        responses
    );

    std::string lambdaString = std::to_string(arguments.lambda.value());
    std::string MSEString = std::to_string(MSE);

    std::vector<std::string> logData = {
        arguments.filename,
        lambdaString,
        MSEString
    };

    std::vector<std::string> columnNames = {
        "file",
        "lambda",
        "mse"
    };

    if (!log(
        "linear_regression",
        arguments.modelName,
        logData,
        columnNames
    )) std::cerr << "Failed to log run." << "\n"; 

    return true;
}

// ##############################################################################
// Logistic Regression

bool loadDataForLOR
(
    std::string &directory,
    std::string &filename,
    arma::mat &data,
    arma::Row<size_t> &labels
)
{
    std::string dataPath = directory + "/" + filename + ".csv";
    std::string labelsPath = directory + "/" + filename + "_threshold.csv";

    try {
        loadData(dataPath, data);
        loadData(labelsPath, labels);
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return false;
    }

    return true;
}

bool collectArgumentsForLOR
(
    int &argc,
    char **argv,
    LORArguments &arguments
)
{
    if (argc < 6) {
        std::cerr << "Too few arguments." << "\n";
        std::cerr << "Usage: " << "a.exe -lor <dir: string> <file: string> <modelName: string> <lambdaInt: int>\n";
        return false;
    };

    try {
        arguments.directory = argv[3]; // directory containing the files
        arguments.filename = argv[4];
        arguments.modelName = argv[5]; // The name of the model for output in /models when the model is saved. Could be name of csv file as well.
        
        if (argc > 6) {
            arguments.lambda = std::stod(argv[6]) / 100.00000f; // Pass as integer value (0 -> 100...200!?)
        }
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

bool fillArgumentsForLOR
(
    int &argc,
    char **argv,
    LORData &data
)
{
    if (!collectArgumentsForLOR(
        argc,
        argv,
        data.arguments
    )) return false;

    if (!loadDataForLOR(
        data.arguments.directory,
        data.arguments.filename,
        data.data,
        data.labels
    )) return false;

    return true;
}

bool trainLogisticRegression
(
    int &argc,
    char **argv
)
{
    // Collect arguments.
    LORData data;

    if (!fillArgumentsForLOR(
        argc,
        argv,
        data
    )) return false;

    if (!data.arguments.lambda.has_value()) {
        std::cerr << "Unable to find lambda value." << "\n";
        return false;
    }

    // Train the model.
    mlpack::LogisticRegression<arma::mat> lor;
    arma::rowvec trainWeights(data.data.n_rows + 1, arma::fill::zeros);
    
    trainWeights(0) = 0.5;

    lor.Lambda() = data.arguments.lambda.value();
    lor.Parameters() = trainWeights;
    lor.Train(
        data.data,
        data.labels
    );

    // Save the model.
    if (!saveModel(
        lor,
        "logistic_regression",
        data.arguments.modelName
    )) return false;

    return true;
}

bool testLogisticRegression
(
    int &argc,
    char **argv
)
{
    LORData data;

    // doesnt need to collect directory / filename
    if (!fillArgumentsForLOR(
        argc,
        argv,
        data
    )) return false;

    mlpack::LogisticRegression<arma::mat> lor;

    std::string path = "models/logistic_regression/" + data.arguments.modelName + ".bin";

    if (!mlpack::data::Load(
        path,
        data.arguments.modelName,
        lor,
        false,
        data::format::binary
    )) {
        std::cerr << "Unable to load model." << "\n";
        return false;
    }

    // Determine and log results.
    double accuracy = lor.ComputeAccuracy
    (
        data.data,
        data.labels
    );

    std::vector<std::string> logData = {
        data.arguments.filename,
        std::to_string(lor.Lambda()),
        std::to_string(accuracy)
    };

    // I would love for this to be a static const.
    std::vector<std::string> columnNames = {
        "file",
        "lambda",
        "accuracy",
    };

    static const std::string modelTypeName = "logistic_regression";

    if (!log(
        modelTypeName,
        data.arguments.modelName,
        logData,
        columnNames
    )) std::cerr << "Unable to log logistic regression." << "\n";

    return true;

}

// ##############################################################################
// Load Model

bool testModel
(
    int &argc,
    char **argv,
    ModelType &modelType
)
{
    switch (modelType) {
        case ModelType::LinearRegression:
            if (!testLinearRegression(
                argc,
                argv
            )) {
                std::cerr << "Failed to test Linear Regression model." << "\n";
                return false;
            }
            break;

        case ModelType::LogisticRegression:
            if (!testLogisticRegression(
                argc,
                argv
            )) {
                std::cerr << "Failed to test Logistic Regression model." << "\n";
                return false;
            }
            break;

        case ModelType::None:
        default:
            std::cerr << "Who knows? Nothing broke, try again!" << "\n";
            return false;
            break;
    }

    return true;
}

bool trainModel
(
    int &argc,
    char **argv,
    ModelType &modelType
)
{
    switch (modelType) {
        case ModelType::LinearRegression:
            if (!trainLinearRegression(
                argc,
                argv
            )) {
                std::cerr << "Failed to train Linear Regression model." << "\n";
                return false;
            }
            break;

        case ModelType::LogisticRegression:
            if (!trainLogisticRegression(
                argc,
                argv
            )) {
                std::cerr << "Failed to train Logistic Regression model." << "\n";
                return false;
            }
            break;

        default:
            std::cerr << "Who knows? Nothing broke, try again!" << "\n";
            return false;
            break;
    }

    return true;
}

// ##############################################################################
// Direct program.

ArgumentType findArgumentType
(
    std::string argumentTypeString
)
{
    static const std::map<std::string, ArgumentType> argumentTypeMap = {
        { "-tem", ArgumentType::TestModel },
        { "-trm", ArgumentType::TrainModel }
    };

    auto argumentTypeFound = argumentTypeMap.find(argumentTypeString);

    if (argumentTypeFound != argumentTypeMap.end()) {
        return argumentTypeFound->second;
    }

    return ArgumentType::None;
}

ModelType findModelType
(
    std::string modelTypeString
)
{
    static const std::map<std::string, ModelType> modelTypeMap = {
        { "-lr", ModelType::LinearRegression },
        { "-lor", ModelType::LogisticRegression }
    };

    auto modelTypeFound = modelTypeMap.find(modelTypeString);

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
    if (argc < 3) {
        std::cerr << "Too few arguments." << "\n";
        return 1;
    }

    std::string argumentTypeString = argv[1];
    ArgumentType argumentType = findArgumentType
    (
        argumentTypeString
    );

    std::string modelTypeString = argv[2];
    ModelType modelType = findModelType
    (
        modelTypeString
    );

    switch (argumentType) {
        case ArgumentType::TrainModel:
            if (!trainModel(
                argc,
                argv,
                modelType
            )) {
                std::cerr << "Unable to train model :(" << "\n";
                return false;
            }
            break;
        
        case ArgumentType::TestModel:
            if (!testModel(
                argc,
                argv,
                modelType
            )) {
                std::cerr << "Unable to test model :(" << "\n";
                return false;
            }
            break;

        case ArgumentType::None:
        default:
            std::cerr << "Oh no!" << "\n";
            return false;
            break;
    }

    return 0;
}