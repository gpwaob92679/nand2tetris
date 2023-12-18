#include "commands.h"

#include "gtest/gtest.h"

TEST(CommandsTest, AddCommand) {
  EXPECT_EQ(AddCommand().ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D+M\n");
}

TEST(CommandsTest, SubCommand) {
  EXPECT_EQ(SubCommand().ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=M-D\n");
}

TEST(CommandsTest, AndCommand) {
  EXPECT_EQ(AndCommand().ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D&M\n");
}

TEST(CommandsTest, OrCommand) {
  EXPECT_EQ(OrCommand().ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D|M\n");
}

TEST(CommandsTest, NegCommand) {
  EXPECT_EQ(NegCommand().ToAssembly(),
            "@SP\n"
            "A=M-1\n"
            "M=-M\n");
}

TEST(CommandsTest, NotCommand) {
  EXPECT_EQ(NotCommand().ToAssembly(),
            "@SP\n"
            "A=M-1\n"
            "M=!M\n");
}

TEST(CommandsTest, EqCommand) {
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

TEST(CommandsTest, GtCommand) {
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

TEST(CommandsTest, LtCommand) {
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
