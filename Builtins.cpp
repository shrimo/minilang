#include "Builtins.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string readFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const std::string &filename, const std::string &content) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Could not write to file: " + filename);
    }
    file << content;
}