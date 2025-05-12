#include <filesystem>
#include <iostream>
#include <string>

#include "utils/dataset_manager.h"
#include "utils/sort_parameters.h"

int main(int argc, char* argv[]) {
  size_t maxMultiplier = 60;

  DatasetManager datasetManager;

  if (datasetManager.checkAllDatasetsExist(maxMultiplier)) {
    std::cout
        << "All required sequence files already exist. Skipping generation."
        << std::endl;
    return 0;
  }

  if (argc > 1) {
    try {
      maxMultiplier = std::stoul(argv[1]);
      if (maxMultiplier < 4) maxMultiplier = 4;
      if (maxMultiplier > 60) maxMultiplier = 60;
      maxMultiplier = (maxMultiplier / 4) * 4;
    } catch (...) {
      std::cerr << "Invalid max multiplier provided. Using default 60M."
                << std::endl;
      maxMultiplier = 60;
    }
  }

  datasetManager.generateExperimentDatasets(maxMultiplier);

  return 0;
}