#pragma once

#include <string>
#include "arguments.hpp"

struct LMArguments {
    std::string directory;
    std::string modelName;
    std::string path;
};

bool collectArgumentsForLM
(
    int &argc,
    char **argv,
    std::string &directory,
    std::string &modelName
);

bool fillArgumentsForLM
(
    int &argc,
    char **argv,
    LMArguments &arguments
);

bool testModel
(
    int &argc,
    char **argv,
    ModelType &modelType
);

bool trainModel
(
    int &argc,
    char **argv,
    ModelType &modelType
);