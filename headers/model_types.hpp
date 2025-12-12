#include <string>
#include <map>

enum class ArgumentType {
    TrainModel,
    TestModel
};

enum class ModelType {
    LinearRegression,
    LogisticRegression
};

bool findArgumentType
(
    std::string argumentTypeString,
    ArgumentType argumentType
);

bool findModelType
(
    std::string modelTypeString,
    ModelType modelType
);