#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>

class Loader {
private:

public:

    std::shared_ptr<std::vector<std::string>> load(const std::string& file_location) {
        std::ifstream file(file_location);
        if (!file.is_open())
            throw std::runtime_error("failed to open: " + file_location);

        auto lines = std::make_shared<std::vector<std::string>>();
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == ';')
                continue;
            lines->push_back(line);
        }
        return lines;
    }
};