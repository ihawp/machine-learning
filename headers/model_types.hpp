#include <string>
#include <map>

enum class ArgumentType {
    None = 0,
    LoadModel = 1,
    LinearRegression = 2,
    LogisticRegression = 3,
};

ArgumentType findModelType
(
    int &argc,
    char **argv
);