#include "VMWriter.hpp"

VMWriter::VMWriter(std::ostream& out) : out(out) { };

void VMWriter::writePush(const Segment::Enum& segment, int index)
{
    write("push", Segment::toString(segment), std::to_string(index));
};

void VMWriter::writePop(const Segment::Enum& segment, int index)
{
    write("pop", Segment::toString(segment), std::to_string(index));
};

void VMWriter::writeArithmetic(const Command::Enum& cmd)
{
    write(Command::toString(cmd));
};

void VMWriter::writeLabel(const std::string& label)
{
    write("label", label);
};

void VMWriter::writeGoto(const std::string& label)
{
    write("goto", label);
};

void VMWriter::writeIf(const std::string& label)
{
    write("if-goto", label);
};

void VMWriter::writeCall(const std::string& name, int nArgs)
{
    write("call", name, std::to_string(nArgs));
};

void VMWriter::writeFunction(const std::string& name, int nLocals)
{
    write("function", name, std::to_string(nLocals));
};

void VMWriter::writeReturn()
{
    write("return");
};

bool VMWriter::write(const std::string& cmd, const std::string& arg1, const std::string& arg2)
{
    out << cmd << " " << arg1 << " " << arg2 << std::endl;
    return out.good();
};
