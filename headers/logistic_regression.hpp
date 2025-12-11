#pragma once

#include <string>
#include <armadillo>
#include <mlpack/core.hpp>
#include <mlpack/methods/linear_regression/linear_regression.hpp>

using namespace arma;
using namespace mlpack;

bool logisticRegression
(
    int &argc,
    char **argv
);

bool collectArgumentsForLOR
(
    int &argc,
    char **argv,
    std::string &directory,
    std::string &filename,
    std::string &modelName,
    double &lambda
);

bool loadDataForLOR
(
    std::string &directory,
    std::string &filename,
    arma::mat &trainData,
    arma::mat &testData,
    arma::Row<size_t> &trainLabels,
    arma::Row<size_t> &testLabels
);

bool saveModel
(
    mlpack::LogisticRegression<arma::mat> &model,
    const std::string &directory,
    std::string &modelName
);