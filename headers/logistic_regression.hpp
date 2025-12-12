#pragma once

#include <string>
#include <optional>
#include <armadillo>
#include <mlpack/core.hpp>
#include <mlpack/methods/logistic_regression/logistic_regression.hpp>

using namespace arma;
using namespace mlpack;

struct LORArguments {
    std::string directory;
    std::string filename;
    std::string modelName;
    std::optional<double> lambda;
};

struct LORData {
    LORArguments arguments;
    arma::mat data;
    arma::Row<size_t> labels;
};

bool loadDataForLOR
(
    std::string &directory,
    std::string &filename,
    arma::mat &data,
    arma::Row<size_t> &labels
);

bool collectArgumentsForLOR
(
    int &argc,
    char **argv,
    LORArguments arguments
);

bool fillArgumentsForLOR
(
    int &argc,
    char **argv,
    LORData data
);

bool trainLogisticRegression
(
    int &argc,
    char **argv
);

bool testLogisticRegression
(
    int &argc,
    char **argv,
    std::string &path,
    std::string &modelName
);