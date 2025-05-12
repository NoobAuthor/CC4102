#ifndef EXPERIMENT_PARAMETERS_H
#define EXPERIMENT_PARAMETERS_H

#include <cstddef>
#include <string>
#include <vector>

/**
 * @brief The ExperimentParameters class is responsible for managing
 */
class ExperimentParameters {
 public:
  std::size_t dataSize = 0;
  std::string dataType = "random";
  double partialSortFraction = 0.7;

  std::size_t memoryLimit = 50 * 1024 * 1024;

  std::size_t arity = 4;
  bool useAutoTuning = true;

  std::string inputFile = "";
  std::string outputFile = "";
  std::string resultDir = "data/results";

  int sequenceNumber = 1;
  int experimentId = 0;

  std::string getDataFileName() const {
    if (!inputFile.empty()) {
      return inputFile;
    }
    return "data/" + dataType + "_" + std::to_string(dataSize) + ".bin";
  }

  std::string getResultsFileName(const std::string& algorithm) const {
    return resultDir + "/" + algorithm + "_results.csv";
  }
};

#endif