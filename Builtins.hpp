#ifndef BUILTINS_HPP
#define BUILTINS_HPP

#include <string>

// Reads an entire file into a string.
std::string readFile(const std::string &filename);

// Writes the provided content into a file.
void writeFile(const std::string &filename, const std::string &content);

#endif // BUILTINS_HPP