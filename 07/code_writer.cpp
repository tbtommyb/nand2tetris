#include "code_writer.hpp"

CodeWriter::CodeWriter(std::ostream& output) : out(output), labelIndex(0)
{
    writeEqualityBlock();
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
        decrementStackPtr();
        readStackToA();
        if (command.arg1 == "add") {
            sum();
        } else if (command.arg1 == "eq") {
            eq();
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

void CodeWriter::sum()
{
    write("M=D+M");
};

void CodeWriter::eq()
{
    std::string labelName = "JUMPPOINT" + std::to_string(labelIndex++);
    write("D=D-M");
    write("@R13");
    write("M=D");
    write("@" + labelName);
    write("D=A");
    write("@R14");
    write("M=D");
    write("@EQUALITY");
    write("0;JMP");
    write("(" + labelName + ")");
};

void CodeWriter::write(std::string arg)
{
    out << arg << std::endl;
};

void CodeWriter::writeEqualityBlock()
{
    write("@START");
    write("0;JEQ");
    write("(EQUALITY)");
    write("@R13");
    write("D=M");
    write("@TRUE");
    write("D;JEQ");
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
    write("(END)");
    write("@R14");
    write("A=M");
    write("0;JEQ");
    write("(START)");
};
