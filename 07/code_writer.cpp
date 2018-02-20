#include "code_writer.hpp"

CodeWriter::CodeWriter(std::ostream& output, std::string filename)
    : out(output), labelIndex(0), filename(filename)
{
    writeBootstrap();
};

void CodeWriter::writePushPop(const Command& command)
{
    switch(command.type) {
    case CommandType::C_PUSH:
        if (command.arg1 == "constant") {
            writeConstant(command.arg2);
        } else if (command.arg1 == "argument") {
            writeFromSegment("ARG", command.arg2);
        } else if (command.arg1 == "local") {
            writeFromSegment("LCL", command.arg2);
        } else if (command.arg1 == "this") {
            writeFromSegment("THIS", command.arg2);
        } else if (command.arg1 == "that") {
            writeFromSegment("THAT", command.arg2);
        } else if (command.arg1 == "temp") {
            writeFromAddress("5", command.arg2);
        } else if (command.arg1 == "pointer") {
            writeFromAddress("3", command.arg2);
        } else if (command.arg1 == "static") {
            writeFromAddress(filename + ".", command.arg2);
        }
        incrementStackPtr();
        break;
    case CommandType::C_POP:
        decrementStackPtr();
        readStackToA();
        if (command.arg1 == "argument") {
            writeToSegment("ARG", command.arg2);
        } else if (command.arg1 == "local") {
            writeToSegment("LCL", command.arg2);
        } else if (command.arg1 == "this") {
            writeToSegment("THIS", command.arg2);
        } else if (command.arg1 == "that") {
            writeToSegment("THAT", command.arg2);
        } else if (command.arg1 == "temp") {
            writeToAddress("5", command.arg2);
        } else if (command.arg1 == "pointer") {
            writeToAddress("3", command.arg2);
        } else if (command.arg1 == "static") {
            writeToAddress(filename + ".", command.arg2);
        }
        break;
    }
};

void CodeWriter::writeArithmetic(const Command& command)
{
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
};

void CodeWriter::writeConstant(int value)
{
    write("@" + std::to_string(value));
    write("D=A");
    write("@SP");
    write("A=M");
    write("M=D");
};

void CodeWriter::writeFromSegment(std::string segment, int index)
{
    write("@" + std::to_string(index));
    write("D=A");
    write("@" + segment);
    write("A=M");
    write("A=A+D");
    write("D=M");
    write("@SP");
    write("A=M");
    write("M=D");
};

void CodeWriter::writeFromAddress(std::string address, int index)
{
    write("@" + std::to_string(index));
    write("D=A");
    write("@" + address);
    write("A=A+D");
    write("D=M");
    write("@SP");
    write("A=M");
    write("M=D");
};

void CodeWriter::writeToSegment(std::string segment, int index)
{
    write("D=M");
    write("@R13");
    write("M=D");
    write("@" + std::to_string(index));
    write("D=A");
    write("@" + segment);
    write("A=M");
    write("D=A+D");
    write("@R14");
    write("M=D");
    write("@R13");
    write("D=M");
    write("@R14");
    write("A=M");
    write("M=D");
};

void CodeWriter::writeToAddress(std::string address, int index)
{
    write("D=M");
    write("@R13");
    write("M=D");
    write("@" + std::to_string(index));
    write("D=A");
    write("@" + address);
    write("D=A+D");
    write("@R14");
    write("M=D");
    write("@R13");
    write("D=M");
    write("@R14");
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
