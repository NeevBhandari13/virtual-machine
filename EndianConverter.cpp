#include <iostream>
#include <vector>
#include <stdio.h>
#include <fstream>

void ConvertEndianness(std::string& fileName) {
    // read in file
    std::ifstream file(fileName, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file");
    }

    
}