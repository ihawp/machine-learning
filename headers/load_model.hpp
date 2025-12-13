#pragma once

#include <string>
#include "arguments.hpp"

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