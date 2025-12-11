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
            throw std::runtime_error("Unable to load training data.");
        }

        bool testDataLoaded = mlpack::data::Load
        (
            testPath,
            testData,
            true
        );
        
        if (!testDataLoaded) {
            throw std::runtime_error("Unable to load test data.");
        }
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

        bool testDataLoaded = mlpack::data::Load
        (
            testDataPath,
            testData,
            true
        );
        
        if (!testDataLoaded) {
            throw std::runtime_error("Unable to load test data.");
        }

        bool testLabelsLoaded = mlpack::data::Load
        (
            testLabelsPath,
            testLabels,
            true
        ); 

        if (!testLabelsLoaded) {
            throw std::runtime_error("Unable to load test labels.");
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
        std::cerr << "Usage: " << "a.exe -lr <dir: string> <file1: string> <file2: string> <modelName: string> <responseColumn: int> <lambdaInt: int>\n";
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

    static const std::vector<std::string> columnNames = {
        "train",
        "test",
        "lambda",
        "training_mse",
        "test_mse"
    };

    // Capture recorded data in log/(modelTypeName)/(modelName).csv
    // data and columnNames should always be the same length.
    static const std::string modelTypeName = "linear_regression";
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

bool logisticRegression
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

    std::vector<std::string> columnNames = {
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

bool loadModel
(
    int &argc,
    char **argv
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
        std::cout << "Unable to collect arguments for loading a model." << "\n";
        return false;
    }

    std::string path = "models/" + directory + "/" + modelName + ".bin";

    std::cout << path << "\n";

    mlpack::LogisticRegression<arma::mat> lor;

    mlpack::data::Load(path, modelName, lor, false, data::format::binary);

    std::cout << lor.Lambda() << "\n";

    // Use the model.
    // I need to ensure I know what type of model it is I am using.
    // This information could be passed as a flag alongside -lm
    // Above I have just saved some logistic regression models and am trying
    // to load them.

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

ArgumentType findModelType
(
    int &argc,
    char **argv
)
{
    if (argc < 2) return ArgumentType::None;

    std::string type = argv[1];

    static const std::map<std::string, ArgumentType> modelTypeMap = {
        { "-lm", ArgumentType::LoadModel},
        { "-lr", ArgumentType::LinearRegression },
        { "-lor", ArgumentType::LogisticRegression }
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
    ArgumentType modelType = findModelType(argc, argv);

    switch (modelType) {

        case ArgumentType::LoadModel:
            if (!loadModel(argc, argv)) {
                std::cerr << "Failed to load model." << "\n";
                return 1;
            }
            break;

        case ArgumentType::LinearRegression:
            if (!linearRegression(argc, argv)) {
                std::cerr << "Linear Regression Failed" << "\n";
                return 1;
            }
            break;

        case ArgumentType::LogisticRegression:
            if (!logisticRegression(argc, argv)) {
                std::cerr << "Logistic Regression Failed" << "\n";
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