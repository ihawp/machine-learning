#include <string>
#include <armadillo>
#include <mlpack/methods/linear_regression/linear_regression.hpp>

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
    mlpack::LinearRegression<arma::fmat> model,
    std::string modelName
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