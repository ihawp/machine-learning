#pragma once

#include <iostream>
#include <string>
#include <mlpack/core.hpp>

using namespace mlpack;

template <typename T>
bool saveModel
(
    T &model, // Generic model type because they all seem to want to be ::Save(ed)
    const std::string &directory,
    std::string &modelName
)
{
    std::filesystem::path modelsPath = "models";

    if (!createDirectory(modelsPath)) {
        std::cerr << "Failed to create models directory" << "\n";
        return false;
    }

    std::filesystem::path directoryPath = modelsPath / directory;

    if (!createDirectory(directoryPath)) {
        std::cerr << "Failed to create directory withing models/" << "\n";
        return false;
    }

    std::string filepath = directoryPath.string() + "/" + modelName + ".bin";

    try {
        bool modelSaved = mlpack::data::Save
        (
            filepath,
            modelName,
            model,
            true,
            mlpack::data::format::binary
        );
        
        if (!modelSaved) {
            throw std::runtime_error("Failed to save model.");
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return false;
    }

    return true;
}

template <typename T>
void loadData
(
    std::string &path,
    T &dataLocation // Generic model type because all models can be ::Load(ed)
)
{
    bool loaded = mlpack::data::Load
    (
        path,
        dataLocation,
        true
    ); 

    if (!loaded) {
        throw std::runtime_error("Unable to load data.");
    }
}