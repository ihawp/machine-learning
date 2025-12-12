#pragma once

#include <string>
#include <armadillo>
#include <mlpack/core.hpp>
#include <mlpack/methods/linear_regression/linear_regression.hpp>
#include "arguments.hpp"

using namespace mlpack;
using namespace arma;

struct LRArguments {
    std::string directory;
    std::string filename;
    std::string modelName;
    int responsesColumn;
    std::optional<float> lambda;
};

void prepareFeaturesAndResponses
(
    const arma::fmat &fullData,
    arma::fmat &features,
    arma::frowvec &responses,
    int &responsesColumn
);

bool loadDataForLR
(
    std::string &directory,
    std::string &filename,
    arma::fmat &data
);

bool collectArgumentsForLR
(
    int &argc,
    char **argv,
    LRArguments &arguments
);

bool trainLinearRegression
(
    int &argc,
    char **argv,
    std::string path,
    std::string modelName
);

bool testLinearRegression
(
    int &argc,
    char **argv,
    std::string path,
    std::string modelName
);