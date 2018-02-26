#include <fstream>
#include <iostream>
#include <sstream>
#include "boost/filesystem.hpp"
#include "parser.hpp"
#include "code_writer.hpp"

namespace fs = boost::filesystem;

void process(CodeWriter& writer, fs::path input)
{
    std::ifstream inputFile{input.string()};
    writer.setCurrentFile(input.stem().string());

    Parser parser{inputFile};

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
        case CommandType::C_CALL:
            writer.writeCall(command);
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
    std::ofstream outputFile{input.stem().string() + ".asm"};
    CodeWriter writer{outputFile};

    if (fs::is_directory(input)) {
        for (const auto& entry : fs::directory_iterator(input)) {
            const auto& file{entry.path()};

            if (file.extension() == ".vm") {
                process(writer, file);
            }
        }
    } else {
        process(writer, input);
    }

    return 0;
};
