#pragma once

#include <string>
#include <armadillo>
#include <mlpack/core.hpp>
#include <mlpack/methods/linear_regression/linear_regression.hpp>

using namespace arma;
using namespace mlpack;

struct LRArguments {
    std::string directory;
    std::string filename;
    std::string modelName;
    int responsesColumn;
    std::optional<float> lambda;
};

bool trainLinearRegression
(
    int &argc,
    char **argv
);

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
    arma::fmat &trainData
);

bool collectArgumentsForLR
(
    int &argc,
    char **argv,
    LRArguments &arguments
);