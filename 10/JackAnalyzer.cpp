#include <iostream>
#include <fstream>
#include "boost/filesystem.hpp"
#include "JackTokenizer.hpp"

namespace fs = boost::filesystem;

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "USAGE: JackAnalyser [file.jack|dir]" << std::endl;
        exit(1);
    }

    std::vector<fs::path> filesToProcess{};
    fs::path input{argv[1]};
    if (fs::is_directory(input)) {
        for (const auto& entry : fs::directory_iterator(input)) {
            filesToProcess.push_back(entry.path());
        }
    } else {
        filesToProcess.push_back(input);
    }

    for (const auto& filePath : filesToProcess) {
        std::ifstream file{filePath.string()};
        JackTokenizer tokenizer{file};
        auto tokens = tokenizer.getTokenList();
        for(const auto& token : tokens) {
            std::cout << token->toString() << std::endl;
        }
        std::ofstream outputFile{filePath.stem().string() + ".xml"};
    }

    return 0;
};
