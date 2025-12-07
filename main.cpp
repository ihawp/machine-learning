#include <iostream>
#include <vector>
#include <filesystem>
#include <mlpack/methods/linear_regression/linear_regression.hpp>

/*

Matrices:

    [  3,   1,   2,   4  ]
A = [  1,  -3,   4,   2  ]
    [  2,   6,  -7,   3  ]

    [  4,   12,   2,   4  ]
B = [  2,   -5,  -2,   1  ]
    [  16,   6,   2,   2  ]

A + B = possible
A - B = possible
B - A = possible
AB    = impossible
BA    = impossible

    [  3,   1,   2,   4  ]
A = [  1,  -3,   4,   2  ]
    [  2,   6,  -7,   3  ]

    [  4,   12,   2  ]
B = [  2,   -5,  -2  ]
    [  16,   6,   2  ]

A + B = impossible
A - B = impossible
B - A = impossible
AB    = impossible
BA    = possible (3x4)

(3x4) = rows by columns (RxC)

They can be divided by multiplying by the inverse of another...but it
sounds like that stuff will matter later. Or maybe not at all in this context.

I also don't yet know how to find the inverse :)

Complex, Hermitian, Unitary !?

*/

/*
    Transpose trueweights (flip columns and rows) for multiplication to be allowed between matrices.
    trueWeights.t() 100x1 => 1x100
    Even though it is vector, it is treated like matrix with 1 row in this library.
    This allows for multiplication with the 100x5000 dataset, where dataset is B
    AB is POSSIBLE:      1x100 * 100x5000
    BA is NOT possible:  100x1 * 5000x100
*/

void prepareFeaturesAndResponses
(
    const arma::fmat& fullData,
    arma::fmat& features,
    arma::frowvec& responses
)
{
    const int HAPPINESS_SCORE_INDEX = 1;

    // Y
    responses = fullData.row(HAPPINESS_SCORE_INDEX);

    // X
    features = arma::join_cols(
        fullData.rows(0, 0),                
        fullData.rows(2, fullData.n_rows - 1)  
    );
}

int useLinearRegression
(
    std::string trainPath,
    std::string testPath
)
{

    arma::fmat trainData;
    arma::fmat testData;

    mlpack::data::Load(trainPath, trainData, true); 
    mlpack::data::Load(testPath, testData, true);

    arma::fvec trueWeights(100, arma::fill::randu);
    
    const int HAPPINESS_SCORE_INDEX = 1;
    
    arma::frowvec trainResponses, testResponses;
    arma::fmat trainDataset, testDataset;

    prepareFeaturesAndResponses(trainData, trainDataset, trainResponses);
    prepareFeaturesAndResponses(testData, testDataset, testResponses);

    mlpack::LinearRegression<arma::fmat> lr;

    lr.Lambda() = 0.00;
    lr.Train(trainDataset, trainResponses);

    // Mean Squared Error (How low can you go?)
    std::cout << "MSE on training set: " << lr.ComputeError(trainDataset, trainResponses) << "." << "\n";
    std::cout << "MSE on test set: " << lr.ComputeError(testDataset, testResponses) << "." << "\n";

    return 1;
}

int main
(
    int argc,
    char **argv
)
{
    if (argc == 1) return 0;

    std::string filename = argv[1];
    std::string trainPath = "world_happiness_data/" + filename + "a.csv";
    std::string testPath = "world_happiness_data/" + filename + "b.csv";

    useLinearRegression(trainPath, testPath);

    return 0;
}