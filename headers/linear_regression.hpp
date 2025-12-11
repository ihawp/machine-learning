#pragma once

#include <string>
#include <armadillo>
#include <mlpack/core.hpp>
#include <mlpack/methods/linear_regression/linear_regression.hpp>

using namespace arma;
using namespace mlpack;

bool linearRegression
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
    std::string &filename1,
    std::string &filename2,
    arma::fmat &trainData,
    arma::fmat &testData
);

bool saveModel
(
    mlpack::LinearRegression<arma::fmat> &model,
    const std::string &directory,
    std::string &modelName
);

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
);