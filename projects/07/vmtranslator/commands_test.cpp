#include "commands.h"

#include <memory>

#include "gtest/gtest.h"

#include "addressing.h"

TEST(ArithmeticsCommandTest, AddCommand) {
  EXPECT_EQ(AddCommand().ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D+M\n");
}

TEST(ArithmeticsCommandTest, SubCommand) {
  EXPECT_EQ(SubCommand().ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=M-D\n");
}

TEST(ArithmeticsCommandTest, AndCommand) {
  EXPECT_EQ(AndCommand().ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D&M\n");
}

TEST(ArithmeticsCommandTest, OrCommand) {
  EXPECT_EQ(OrCommand().ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D|M\n");
}

TEST(ArithmeticsCommandTest, NegCommand) {
  EXPECT_EQ(NegCommand().ToAssembly(),
            "@SP\n"
            "A=M-1\n"
            "M=-M\n");
}

TEST(ArithmeticsCommandTest, NotCommand) {
  EXPECT_EQ(NotCommand().ToAssembly(),
            "@SP\n"
            "A=M-1\n"
            "M=!M\n");
}

TEST(LogicalCommandTest, EqCommand) {
  EXPECT_EQ(EqCommand("Foo_123").ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n"
            "@Foo_123$eq_else\n"
            "D;JNE\n"
            "@SP\n"
            "A=M-1\n"
            "M=-1\n"
            "@Foo_123$eq_end\n"
            "0;JMP\n"
            "(Foo_123$eq_else)\n"
            "@SP\n"
            "A=M-1\n"
            "M=0\n"
            "(Foo_123$eq_end)\n");
}

TEST(LogicalCommandTest, GtCommand) {
  EXPECT_EQ(GtCommand("Foo_123").ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n"
            "@Foo_123$gt_else\n"
            "D;JLE\n"
            "@SP\n"
            "A=M-1\n"
            "M=-1\n"
            "@Foo_123$gt_end\n"
            "0;JMP\n"
            "(Foo_123$gt_else)\n"
            "@SP\n"
            "A=M-1\n"
            "M=0\n"
            "(Foo_123$gt_end)\n");
}

TEST(LogicalCommandTest, LtCommand) {
  EXPECT_EQ(LtCommand("Foo_123").ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n"
            "@Foo_123$lt_else\n"
            "D;JGE\n"
            "@SP\n"
            "A=M-1\n"
            "M=-1\n"
            "@Foo_123$lt_end\n"
            "0;JMP\n"
            "(Foo_123$lt_else)\n"
            "@SP\n"
            "A=M-1\n"
            "M=0\n"
            "(Foo_123$lt_end)\n");
}

TEST(PushCommandTest, ArgumentAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<ArgumentAddress>(5)).ToAssembly(),
            "@5\n"
            "D=A\n"
            "@ARG\n"
            "A=D+M\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(PushCommandTest, LocalAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<LocalAddress>(5)).ToAssembly(),
            "@5\n"
            "D=A\n"
            "@LCL\n"
            "A=D+M\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(PushCommandTest, ThisAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<ThisAddress>(5)).ToAssembly(),
            "@5\n"
            "D=A\n"
            "@THIS\n"
            "A=D+M\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(PushCommandTest, ThatAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<ThatAddress>(5)).ToAssembly(),
            "@5\n"
            "D=A\n"
            "@THAT\n"
            "A=D+M\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(PushCommandTest, StaticAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<StaticAddress>("Foo", 5)).ToAssembly(),
            "@Foo.5\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(PushCommandTest, ConstantAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<ConstantAddress>(5)).ToAssembly(),
            "@5\n"
            "D=A\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(PushCommandTest, PointerAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<PointerAddress>(0)).ToAssembly(),
            "@3\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(PushCommandTest, TempAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<TempAddress>(5)).ToAssembly(),
            "@10\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}
