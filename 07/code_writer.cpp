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

CodeWriter::CodeWriter(std::ostream& output, const std::string& filename)
    : out(output), labelIndex(0), frameIndex(0), filename(filename)
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

void CodeWriter::writeLabel(const Command& command)
{
    write("(" + command.arg1 + ")");
};

void CodeWriter::writeGoto(const Command& command)
{

    write("@" + command.arg1);
    write("0;JEQ");
};

void CodeWriter::writeIf(const Command& command)
{
    pop("D");
    write("@" + command.arg1);
    write("D;JNE");
};

void CodeWriter::writeFunction(const Command& command)
{
    write("(" + command.arg1 + ")");
    for (std::size_t i = 0; i < command.arg2; i++) {
        writeToPointer("SP", "0");
        incrementPointer("SP");
    }
};

void CodeWriter::writeReturn(const Command& command)
{
    auto frame = filename + ".frame." + std::to_string(frameIndex++);
    write("@LCL");
    write("D=M");
    write("@"+frame);
    write("M=D");

    write("@5");
    write("D=A");
    write("@"+frame);
    write("A=M-D");
    write("D=M");
    write("@RET."+frame);
    write("M=D");

    pop("D");
    writeToPointer("ARG", "D");

    write("@ARG");
    write("A=M");
    write("D=A");
    write("@SP");
    write("M=D+1");
    // write("@SP");
    // write("AM=M-1");
    // write("D=M");
    // write("@ARG");
    // write("A=M");
    // write("M=D");
    // loadValue("ARG", "D");
    // write("@SP");
    // write("M=D+1");

    loadValue("1", "D");
    write("@"+frame); // THAT = *(FRAME-1)
    write("A=M-D");
    write("D=M");
    write("@THAT");
    write("M=D");

    loadValue("2", "D");
    write("@"+frame); // THAT = *(FRAME-1)
    write("A=M-D");
    write("D=M");
    write("@THIS");
    write("M=D");

    loadValue("3", "D");
    write("@"+frame); // THAT = *(FRAME-1)
    write("A=M-D");
    write("D=M");
    write("@ARG");
    write("M=D");

    loadValue("4", "D");
    write("@"+frame); // THAT = *(FRAME-1)
    write("A=M-D");
    write("D=M");
    write("@LCL");
    write("M=D");



    // loadValue("1", "D");
    // loadFromPointer(frame, "A");
    // write("A=A-D");
    // write("D=M");
    // writeToAddress("THAT", 0);

    // loadValue("2", "D");
    // loadFromPointer(frame, "A");
    // write("A=A-D");
    // write("D=M");
    // writeToAddress("THIS", 0);

    // loadValue("3", "D");
    // loadFromPointer(frame, "A");
    // write("A=A-D");
    // write("D=M");
    // writeToAddress("ARG", 0);

    // loadValue("4", "D");
    // loadFromPointer(frame, "A");
    // write("A=A-D");
    // write("D=M");
    // writeToAddress("LCL", 0);

    write("@RET." + frame);
    write("A=M");
    write("0;JEQ");
};

void CodeWriter::pop(const std::string& dest)
{
    decrementPointer("SP");
    loadFromPointer("SP", "A");
    if (dest == "D") {
        write("D=M");
    };
};

void CodeWriter::writeConstant(int value)
{
    loadValue(std::to_string(value), "D");
    writeToPointer("SP", "D");
};

void CodeWriter::writeFromSegment(const std::string& segment, int index)
{
    loadValue(std::to_string(index), "D");
    loadFromPointer(segment, "A");
    write("A=A+D");
    write("D=M");
    writeToPointer("SP", "D");
};

void CodeWriter::writeFromAddress(const std::string& address, int index)
{
    loadValue(std::to_string(index), "D");
    loadValue(address, "A");
    write("A=A+D");
    write("D=M");
    writeToPointer("SP", "D");
};

void CodeWriter::writeToSegment(const std::string& segment, int index)
{
    loadValue(std::to_string(index), "D");
    loadFromPointer(segment, "A");
    write("D=A+D");
    saveValueTo("R13");
    loadFromPointer("SP", "D");
    writeToPointer("R13", "D");
};

void CodeWriter::writeToAddress(const std::string& address, int index)
{
    loadValue(std::to_string(index), "D");
    loadValue(address, "A");
    write("D=A+D");
    saveValueTo("R13");
    loadFromPointer("SP", "D");
    writeToPointer("R13", "D");
};

void CodeWriter::incrementPointer(const std::string& address)
{
    loadValue(address, "A");
    write("M=M+1");
};

void CodeWriter::decrementPointer(const std::string& address)
{
    loadValue(address, "A");
    write("M=M-1");
};

void CodeWriter::writeToPointer(const std::string& address, const std::string& val)
{
    loadFromPointer(address, "A");
    write("M=" + val);
};

void CodeWriter::loadFromPointer(const std::string& address, const std::string& dest)
{
    loadValue(address, "A");
    write("A=M");
    if (dest == "D") {
        write("D=M");
    }
};

void CodeWriter::loadValue(const std::string& value, const std::string& dest)
{
    write("@" + value);
    if (dest == "D") {
        write("D=A");
    }
};

void CodeWriter::saveValueTo(const std::string& address)
{
    loadValue(address, "A");
    write("M=D");
};

void CodeWriter::compare(const std::string& comparison)
{
    std::string labelName = "JUMPPOINT" + std::to_string(labelIndex++);
    write("D=D-M");
    saveValueTo("R13");
    loadValue(labelName, "D");
    saveValueTo("R14");
    write("@" + comparison);
    write("0;JMP");
    write("(" + labelName + ")");
};

void CodeWriter::write(const std::string& arg)
{
    out << arg << std::endl;
};

void CodeWriter::writeBootstrap()
{
    // loadValue("256", "D");
    // saveValueTo("SP");
    write("@START");
    write("0;JEQ");
    equalityFn("(EQ)", "D;JEQ");
    equalityFn("(LT)", "D;JGT");
    equalityFn("(GT)", "D;JLT");
    write("(END)");
    loadFromPointer("R14", "A");
    write("0;JEQ");
    write("(START)");
};

void CodeWriter::equalityFn(const std::string& label, const std::string& comparison)
{
    write(label);
    write("@R13");
    write("D=M");
    write("@TRUE");
    write(comparison);
    write("@FALSE");
    write("0;JEQ");
    write("(TRUE)");
    writeToPointer("SP", "-1");
    write("@END");
    write("0;JEQ");
    write("(FALSE)");
    writeToPointer("SP", "0");
};
