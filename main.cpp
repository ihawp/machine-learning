#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <mlpack/methods/linear_regression/linear_regression.hpp>

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

int log
(
    std::string &name,
    std::vector<std::string> &data,
    std::vector<std::string> &columnNames
)
{

    std::filesystem::path directory = "log";

    if (!createDirectory(directory)) return 0;

    std::string filename = name + ".csv";
    std::filesystem::path filepath = directory / filename;

    if (!std::filesystem::exists(filepath)) {
        if (!createCSV(filepath, columnNames)) {
            std::cerr << "Unable to create CSV in log()" << "\n";
            return 0;
        }
    }

    if (!updateCSV(filepath, data)) {
        std::cerr << "Unable to update CSV in log()" << "\n";
        return 0;
    }

    return 1;
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
    responses = fullData.row(responsesColumn);

    // X
    if (responsesColumn == 0) {
        features = fullData.rows(
            1,
            fullData.n_rows - 1
        );
    } else {
        features = arma::join_cols(
            fullData.rows(0, responsesColumn - 1),
            fullData.rows(responsesColumn + 1, fullData.n_rows - 1)
        );
    }
}

int loadData
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

    // true for transpose
    try {
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
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return 0;
    }
    catch (const std::logic_error& e) {
        std::cerr << e.what() << " yikes" "\n";
        return 0;
    }

    return 1;
}

int saveModel
(
    mlpack::LinearRegression<arma::fmat> model,
    std::string modelName
)
{
    std::filesystem::path modelsPath = "models";

    if (!createDirectory(modelsPath)) {
        std::cerr << "Failed to create models directory" << "\n";
        return 0;
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
            return 0;
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return 0;
    }

    return 1;
}

std::string formatFloatScientific
(
    float value,
    int precision = 6
)
{
    std::stringstream ss;
    
    // Force scientific notation
    ss << std::scientific; 
    
    // Set the number of digits after the decimal point
    // scientific precision is usually the number of *fractional* digits, 
    // the digit before the decimal is always 1 digit.
    ss << std::setprecision(precision); 
    ss << value;
    
    return ss.str();
}

void trainLinearRegression
(
    mlpack::LinearRegression<arma::fmat> &model,
    arma::fmat &trainDataset,
    arma::frowvec &trainResponses,
    float &lambda
)
{
    model.Lambda() = lambda;
    model.Train(trainDataset, trainResponses);
}

bool collectArguments
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

    if (argc < 7) {
        std::cerr << "Too few arguments" << "\n";
        std::cerr << "Usage: " << "a.exe <dir: string> <file1: string> <file2: string> <modelName: string> <responseColumn: int> <lambdaInt: int>\n";
        return 1;
    };

    try {
        directory = argv[1]; // directory containing the files
        filename1 = argv[2];
        filename2 = argv[3];
        modelName = argv[4]; // The name of the model for output in /models when the model is saved. Could be name of csv file as well.
        responsesColumn = std::stoi(argv[5]); // Choose response column for submitted files
        lambda = std::stof(argv[6]) / 100.0000f; // Pass as integer value (0 -> 100...200!?)
    }
    catch (const std::out_of_range& e) {
        std::cerr << "" << "\n";
        return false;
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "" << "\n";
        return false;
    }

    return true;
}

int main
(
    int argc,
    char **argv
)
{

    std::string directory;
    std::string filename1;
    std::string filename2;
    std::string modelName;
    int responsesColumn;
    float lambda;

    if (!collectArguments(
        argc,
        argv,
        directory,
        filename1,
        filename2,
        modelName,
        responsesColumn,
        lambda
    )) {
        std::cerr << "Failed to collect arguments." << "\n";
        return 1;
    }

    arma::fmat trainData, testData;

    bool dataLoaded = loadData(
        directory, 
        filename1, 
        filename2, 
        trainData, 
        testData
    );

    if (!dataLoaded) return 1;

    arma::frowvec trainResponses, testResponses;
    arma::fmat trainDataset, testDataset;

    prepareFeaturesAndResponses(
        trainData,
        trainDataset,
        trainResponses,
        responsesColumn
    );

    prepareFeaturesAndResponses(
        testData,
        testDataset, 
        testResponses, 
        responsesColumn
    );

    mlpack::LinearRegression<arma::fmat> model;

    trainLinearRegression(
        model,
        trainDataset,
        trainResponses,
        lambda
    );

    // Mean Squared Error (How low can you go?)
    float trainingMSE = model.ComputeError(
        trainDataset, 
        trainResponses
    );
    
    float testMSE = model.ComputeError(
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
    if (!log(modelName, data, columnNames)) {
        std::cerr << "Failed to log run." << "\n"; 
        return 1;
    }

    if (!saveModel(model, modelName)) {
        std::cerr << "Unable to save model, but it did really well, congrats!" << "\n";
        return 1;
    }

    return 0;
}