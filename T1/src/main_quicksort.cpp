#include "external_quicksort.hpp"
#include <iostream>
#include <ctime>

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0]
                  << " input output memoryLimitBytes partitions\n";
        return 1;
    }
    std::srand(std::time(nullptr));
    externalQuicksort(
        argv[1],      // input file
        argv[2],      // output file
        std::stoll(argv[3]), // memory limit in bytes
        std::stoi(argv[4])   // number of partitions
    );
    return 0;
}