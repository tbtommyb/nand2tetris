#include <map>
#include "code_writer.hpp"

typedef std::map<std::string, std::string> CommandMap;

CommandMap addresses = {
    { "temp", "5" },
    { "pointer", "3" }
};

CommandMap segments = {
    { "argument", "ARG" },
    { "local", "LCL" },
    { "this", "THIS" },
    { "that", "THAT" }
};

CommandMap binaryArithmetic = {
    { "add", "M=D+M" },
    { "sub", "M=M-D" },
    { "and", "M=M&D" },
    { "or", "M=M|D" }
};

CommandMap unaryArithmetic = {
    { "neg", "M=-M" },
    { "not", "M=!M" }
};

CommandMap comparisons = {
    { "eq", "EQ" },
    { "lt", "LT" },
    { "gt", "GT" }
};

CodeWriter::CodeWriter(std::ostream& output, std::string filename)
    : out(output), labelIndex(0), filename(filename)
{
    writeBootstrap();
};

void CodeWriter::writePushPop(const Command& command)
{
    auto cmd = command.arg1;

    switch(command.type) {
    case CommandType::C_PUSH:
        if (cmd == "constant") {
            writeConstant(command.arg2);
        } else if (cmd == "static") {
            writeFromAddress(filename + ".", command.arg2);
        } else if (cmd == "temp" || cmd == "pointer") {
            writeFromAddress(addresses.find(cmd)->second, command.arg2);
        } else if (cmd == "argument" || cmd == "local" || cmd == "this" || cmd == "that") {
            writeFromSegment(segments.find(cmd)->second, command.arg2);
        }

        incrementPointer("SP");
        break;
    case CommandType::C_POP:
        pop("A");

        if (cmd == "static") {
            writeToAddress(filename + ".", command.arg2);
        } else if (cmd == "temp" || cmd == "pointer") {
            writeToAddress(addresses.find(cmd)->second, command.arg2);
        } else if (cmd == "argument" || cmd == "local" || cmd == "this" || cmd == "that") {
            writeToSegment(segments.find(cmd)->second, command.arg2);
        }

        break;
    }
};

void CodeWriter::writeArithmetic(const Command& command)
{
    pop("D");

    auto cmd = command.arg1;
    if (cmd == "neg" || cmd == "not") {
        write(unaryArithmetic.find(cmd)->second);
    } else {
        pop("A");
        if (cmd == "add" || cmd == "sub" || cmd == "and" || cmd == "or") {
            write(binaryArithmetic.find(cmd)->second);
        } else if (cmd == "eq" || cmd == "gt" || cmd == "lt") {
            compare(comparisons.find(cmd)->second);
        }
    }

    incrementPointer("SP");
};

void CodeWriter::pop(std::string dest)
{
    decrementPointer("SP");
    loadPointer("SP", "A");
    if (dest == "D") {
        write("D=M");
    };
};

void CodeWriter::writeConstant(int value)
{
    loadValue(std::to_string(value));
    writeToPointer("SP");
};

void CodeWriter::writeFromSegment(std::string segment, int index)
{
    loadValue(std::to_string(index));
    loadPointer(segment, "A");
    write("A=A+D");
    write("D=M");
    writeToPointer("SP");
};

void CodeWriter::writeFromAddress(std::string address, int index)
{
    loadValue(std::to_string(index));
    write("@" + address);
    write("A=A+D");
    write("D=M");
    writeToPointer("SP");
};

void CodeWriter::writeToSegment(std::string segment, int index)
{
    loadValue(std::to_string(index));
    loadPointer(segment, "A");
    write("D=A+D");
    saveValue("R13");
    loadPointer("SP", "D");
    writeToPointer("R13");
};

void CodeWriter::writeToAddress(std::string address, int index)
{
    loadValue(std::to_string(index));
    write("@" + address);
    write("D=A+D");
    saveValue("R13");
    loadPointer("SP", "D");
    writeToPointer("R13");
};

void CodeWriter::incrementPointer(std::string address)
{
    write("@" + address);
    write("M=M+1");
};

void CodeWriter::decrementPointer(std::string address)
{
    write("@" + address);
    write("M=M-1");
};

void CodeWriter::writeToPointer(std::string address)
{
    loadPointer(address, "A");
    write("M=D");
};

void CodeWriter::loadPointer(std::string address, std::string dest)
{
    write("@" + address);
    write("A=M");
    if (dest == "D") {
        write("D=M");
    }
};

void CodeWriter::loadValue(std::string address)
{
    write("@" + address);
    write("D=A");
};

void CodeWriter::saveValue(std::string address)
{
    write("@" + address);
    write("M=D");
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
