#include <string>
#include <map>

enum class ModelType {
    None = 0,
    LinearRegression = 1,
    LogisticRegression = 2,
};

ModelType findModelType
(
    int &argc,
    char **argv
);