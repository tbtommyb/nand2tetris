#include "code_writer.hpp"

CodeWriter::CodeWriter(std::ostream& output) : out(output), labelIndex(0)
{
    writeBootstrap();
};

void CodeWriter::output(const Command& command)
{
    switch(command.type) {
    case CommandType::C_PUSH:
        writeToStack(command.arg2);
        incrementStackPtr();
        break;
    case CommandType::C_ARITHMETIC:
        decrementStackPtr();
        readStackToD();
        if (command.arg1 == "add") {
            decrementStackPtr();
            readStackToA();
            write("M=D+M");
        } else if (command.arg1 == "sub") {
            decrementStackPtr();
            readStackToA();
            write("M=M-D");
        } else if (command.arg1 == "neg") {
            write("M=-M");
        } else if (command.arg1 == "eq") {
            decrementStackPtr();
            readStackToA();
            compare("EQ");
        } else if (command.arg1 == "gt") {
            decrementStackPtr();
            readStackToA();
            compare("GT");
        } else if (command.arg1 == "lt") {
            decrementStackPtr();
            readStackToA();
            compare("LT");
        } else if (command.arg1 == "and") {
            decrementStackPtr();
            readStackToA();
            write("M=M&D");
        } else if (command.arg1 == "or") {
            decrementStackPtr();
            readStackToA();
            write("M=M|D");
        } else if (command.arg1 == "not") {
            write("M=!M");
        }
        incrementStackPtr();
        break;
    }
};

void CodeWriter::writeToStack(int value)
{
    write("@" + std::to_string(value));
    write("D=A");
    write("@SP");
    write("A=M");
    write("M=D");
};

void CodeWriter::incrementStackPtr()
{
    write("@SP");
    write("M=M+1");
};

void CodeWriter::decrementStackPtr()
{
    write("@SP");
    write("M=M-1");
};

void CodeWriter::readStackToD()
{
    readStackToA();
    write("D=M");
};

void CodeWriter::readStackToA()
{
    write("A=M");
};

void CodeWriter::compare(std::string comparison)
{
    std::string labelName = "JUMPPOINT" + std::to_string(labelIndex++);
    write("D=D-M");
    write("@R13");
    write("M=D");
    write("@" + labelName);
    write("D=A");
    write("@R14");
    write("M=D");
    write("@" + comparison);
    write("0;JMP");
    write("(" + labelName + ")");
};

void CodeWriter::write(std::string arg)
{
    out << arg << std::endl;
};

void CodeWriter::writeBootstrap()
{
    write("@START");
    write("0;JEQ");
    equalityFn("(EQ)", "D;JEQ");
    equalityFn("(LT)", "D;JGT");
    equalityFn("(GT)", "D;JLT");
    write("(END)");
    write("@R14");
    write("A=M");
    write("0;JEQ");
    write("(START)");
};

void CodeWriter::equalityFn(std::string label, std::string comparison)
{
    write(label);
    write("@R13");
    write("D=M");
    write("@TRUE");
    write(comparison);
    write("@FALSE");
    write("0;JEQ");
    write("(TRUE)");
    write("@SP");
    write("A=M");
    write("M=-1");
    write("@END");
    write("0;JEQ");
    write("(FALSE)");
    write("@SP");
    write("A=M");
    write("M=0");
};
