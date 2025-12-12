#pragma once

#include <string>

enum class ArgumentType {
    None,
    TrainModel,
    TestModel
};

enum class ModelType {
    None,
    LinearRegression,
    LogisticRegression
};

ArgumentType findArgumentType
(
    std::string argumentTypeString
);

ModelType findModelType
(
    std::string modelTypeString
);