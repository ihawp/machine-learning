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
#include "headers/linear_regression.hpp"
#include "headers/logistic_regression.hpp"
#include "headers/model_types.hpp"
#include "headers/csv_utils.hpp"
#include "headers/log.hpp"
#include "headers/save_model.hpp"

using namespace arma;
using namespace mlpack;

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
    std::string &directory,
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
    arma::fmat &trainData
)
{
    std::string trainPath = directory + "/" + filename;

    try {
        loadData(trainPath, trainData);
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return false;
    }

    return true;
}

bool loadDataForLOR
(
    std::string &directory,
    std::string &filename,
    arma::mat &trainData,
    arma::mat &testData,
    arma::Row<size_t> &trainLabels,
    arma::Row<size_t> &testLabels
)
{
    std::string trainDataPath = directory + "/" + filename + "a.csv";
    std::string testDataPath = directory + "/" + filename + "b.csv";
    std::string trainLabelsPath = directory + "/" + filename + "a_threshold.csv";
    std::string testLabelsPath = directory + "/" + filename + "b_threshold.csv";

    try {
        loadData(trainDataPath, trainData);
        loadData(trainLabelsPath, trainLabels);
        loadData(testDataPath, testData);
        loadData(testLabelsPath, testLabels);
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return false;
    }

    return true;

}

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
        directory = argv[3];
        modelName = argv[4];
    }
    catch (const std::runtime_error& e) {
        std::cout << e.what() << "\n";
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

        if (argc < 8) {
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

bool collectArgumentsForLOR
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
        directory = argv[3]; // directory containing the files
        filename = argv[4];
        modelName = argv[5]; // The name of the model for output in /models when the model is saved. Could be name of csv file as well.
        lambda = std::stod(argv[6]) / 100.00000f; // Pass as integer value (0 -> 100...200!?)
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

// starting point
bool trainLinearRegression
(
    int &argc,
    char **argv
)
{

    arma::fmat data, dataset;
    arma::frowvec responses;

    LRArguments arguments;
    
    if (!collectArgumentsForLR(
        argc,
        argv,
        arguments
    ))
    {
        std::cerr << "Failed to collect arguments." << "\n";
        return false;
    }

    bool dataLoaded = loadDataForLR
    (
        arguments.directory,
        arguments.filename,
        data
    );

    if (!dataLoaded) return false;

    prepareFeaturesAndResponses
    (
        data,
        dataset,
        responses,
        arguments.responsesColumn
    );

    return true;

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

    // Mean Squared Error (How low can you go?)
    float trainingMSE = model.ComputeError
    (
        dataset,
        responses
    );

    std::string lambdaString = std::to_string(arguments.lambda.value());
    std::string trainingMSEString = std::to_string(trainingMSE);

    std::vector<std::string> logData = {
        arguments.modelName,
        arguments.filename,
        lambdaString,
        trainingMSEString
    };

    static const std::vector<std::string> columnNames = {
        "modelName",
        "train",
        "lambda",
        "training_mse"
    };

    static const std::string modelTypeName = "linear_regression";
    bool logged = log
    (
        modelTypeName,
        arguments.modelName,
        logData,
        columnNames
    );

    if (!logged) {
        std::cerr << "Failed to log run." << "\n"; 
        return false;
    }

    bool modelSaved = saveModel
    (
        model,
        modelTypeName,
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
    char **argv,
    std::string path,
    std::string modelName
)
{

    LRArguments arguments;

    if (!collectArgumentsForLR(
        argc,
        argv,
        arguments
    )) {
        std::cerr << "Unable to collect arguments for Linear Regression." << "\n";
        return false;
    }

    mlpack::LinearRegression<arma::fmat> lr;

    if (!mlpack::data::Load(
        path,
        modelName,
        lr,
        false,
        data::format::binary
    )) {
        std::cerr << "Unable to load model." << "\n";
        return false;
    }

    arma::fmat data, dataset;
    arma::frowvec responses;

}

// starting point
bool trainLogisticRegression
(
    int &argc,
    char **argv
)
{
    std::string directory, filename, modelName;
    double lambda;

    if (!collectArgumentsForLOR(
        argc,
        argv,
        directory,
        filename,
        modelName,
        lambda
    )) return false;

    arma::mat trainData;
    arma::mat testData;
    arma::Row<size_t> trainLabels;
    arma::Row<size_t> testLabels;

    if (!loadDataForLOR(
        directory,
        filename,
        trainData,
        testData,
        trainLabels,
        testLabels
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

    double accuracy = lor.ComputeAccuracy(testData, testLabels);

    arma::Row<size_t> predictions;
    lor.Classify(testData, predictions);

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

// starting points
bool testLogisticRegression
(
    int &argc,
    char **argv,
    std::string &path,
    std::string &modelName
)
{
    mlpack::LogisticRegression<arma::mat> lor;

    if (!mlpack::data::Load(
        path,
        modelName,
        lor,
        false,
        data::format::binary
    )) {
        std::cerr << "Unable to load model." << "\n";
        return false;
    }

    std::string directory;
    std::string filename;
    double lambda;

}

// OVERALL starting point
bool testModel
(
    int &argc,
    char **argv,
    ModelType modelType
)
{
    std::cout << "Loading Model." << "\n";

    std::string directory;
    std::string modelName;

    if (!collectArgumentsForLM(
        argc,
        argv,
        directory,
        modelName
    )) {
        std::cout << "Unable to collect arguments for testing a model." << "\n";
        return false;
    }

    std::string path = "models/" + directory + "/" + modelName + ".bin";

    switch (modelType) {
        case ModelType::LinearRegression:
            if (!testLinearRegression(
                argc,
                argv,
                path,
                modelName
            )) {
                std::cerr << "Failed to test Linear Regression model." << "\n";
                return false;
            }
            break;
        case ModelType::LogisticRegression:
            if (!testLogisticRegression(
                argc,
                argv,
                path,
                modelName
            )) {
                std::cerr << "Failed to test Logistic Regression model." << "\n";
                return false;
            }
            break;
        default:
            std:cerr << "Who knows? Nothing broke, try again!" << "\n";
            return false;
            break;
    }

    return true;
}

// OVERALL starting point
bool trainModel
(
    int &argc,
    char **argv,
    ModelType modelType
)
{
    // Use switch to lead to trainLinearRegression...trainLogisticRegression, etc.
    return true;
}

bool findArgumentType
(
    std::string argumentTypeString,
    ArgumentType argumentType
)
{

    static const std::map<std::string, ArgumentType> argumentTypeMap = {
        { "-tem", ArgumentType::TestModel },
        { "-trm", ArgumentType::TrainModel }
    };

    auto argumentTypeFound = argumentTypeMap.find(argumentTypeString);

    if (argumentTypeFound != argumentTypeMap.end()) {
        argumentType = argumentTypeFound->second;
        return true;
    }

    return false;
}

bool findModelType
(
    std::string modelTypeString,
    ModelType modelType
)
{
    static const std::map<std::string, ModelType> modelTypeMap = {
        { "-lr", ModelType::LinearRegression },
        { "-lor", ModelType::LogisticRegression }
    };

    auto modelTypeFound = modelTypeMap.find(modelTypeString);

    if (modelTypeFound != modelTypeMap.end()) {
        modelType = modelTypeFound->second;
        return true;
    }

    return false;
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
    ArgumentType argumentType;

    if (!findArgumentType(
        argumentTypeString,
        argumentType
    )) {
        std::cerr << "Unable to determine primary argument type selection (train/test). Use -trm to 'train model' or -tem to 'test model.'" << "\n";
        return 1;
    }

    std::string modelTypeString = argv[2];
    ModelType modelType;

    if (!findModelType(
        modelTypeString,
        modelType
    )) {
        std::cerr << "Unable to determine model type selection." << "\n";
        return 1;
    }

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
    }

    return 0;
}