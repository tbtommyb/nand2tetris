#include <fstream>
#include <iostream>
#include <sstream>
#include "boost/filesystem.hpp"
#include "parser.hpp"
#include "code_writer.hpp"

namespace fs = boost::filesystem;

void process(fs::path input, fs::path output)
{
    std::ifstream inputFile{input.string()};
    std::ofstream outputFile{output.string(), std::ios_base::app};

    Parser parser{inputFile};
    CodeWriter writer{outputFile, input.stem().string()};

    while (parser.hasMoreCommands()) {
        parser.advance();

        Command command = parser.parse();
        switch(command.type) {
        case CommandType::C_PUSH:
        case CommandType::C_POP:
            writer.writePushPop(command);
            break;
        case CommandType::C_ARITHMETIC:
            writer.writeArithmetic(command);
            break;
        case CommandType::C_LABEL:
            writer.writeLabel(command);
            break;
        case CommandType::C_GOTO:
            writer.writeGoto(command);
            break;
        case CommandType::C_IF:
            writer.writeIf(command);
            break;
        case CommandType::C_FUNCTION:
            writer.writeFunction(command);
            break;
        case CommandType::C_RETURN:
            writer.writeReturn(command);
            break;
        }
    }
};

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "USAGE: vm input.vm" << std::endl;
        exit(1);
    }

    fs::path input{argv[1]};

    if (fs::is_directory(input)) {
        for (const auto& entry : fs::directory_iterator(input)) {
            const auto& file{entry.path()};

            if (file.extension() == ".vm") {
                std::cout << "processing: " << file.filename().string() << std::endl;
                process(file, fs::path{input.parent_path().string() + ".asm"});
            }
        }
    } else {
        process(input, fs::path{input.stem().string() + ".asm"});
    }

    return 0;
};
