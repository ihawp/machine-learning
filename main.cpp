#include <iostream>
#include <vector>
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

They can be divided by multiplying by the inverse of another...but it
sounds like that stuff will matter later. Or maybe not at all in this context.

I also don't yet know how to find the inverse :)

Complex, Hermitian, Unitary !?

*/

int main
(
  int argc,
  char **argv
)
{
  arma::sp_fmat dataset;
  dataset.sprandu(100, 5000, 0.3);

  arma::fvec trueWeights(100, arma::fill::randu);
  
  arma::frowvec responses = trueWeights.t() * dataset + 0.01f * arma::randu<arma::frowvec>(5000);
  
  mlpack::LinearRegression<arma::fmat> lr;

  lr.Lambda() = 0.01;
  lr.Train(dataset, responses);

  arma::sp_fmat testDataset;
  testDataset.sprandu(100, 1000, 0.3);

  arma::frowvec testResponses = trueWeights.t() * testDataset + 0.01f * arma::randu<arma::frowvec>(1000);

  // Mean Squared Error
  std::cout << "MSE on training set: " << lr.ComputeError(dataset, responses) << "." << "\n";
  std::cout << "MSE on test set: " << lr.ComputeError(testDataset, testResponses) << "." << "\n";

  return 0;
}