#include "commands.h"

#include "gtest/gtest.h"

TEST(CommandsTest, AddCommand) {
  EXPECT_EQ(AddCommand().ToString(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D+M\n");
}

TEST(CommandsTest, SubCommand) {
  EXPECT_EQ(SubCommand().ToString(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=M-D\n");
}

TEST(CommandsTest, AndCommand) {
  EXPECT_EQ(AndCommand().ToString(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D&M\n");
}

TEST(CommandsTest, OrCommand) {
  EXPECT_EQ(OrCommand().ToString(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D|M\n");
}

TEST(CommandsTest, NegCommand) {
  EXPECT_EQ(NegCommand().ToString(),
            "@SP\n"
            "A=M-1\n"
            "M=-M\n");
}

TEST(CommandsTest, NotCommand) {
  EXPECT_EQ(NotCommand().ToString(),
            "@SP\n"
            "A=M-1\n"
            "M=!M\n");
}

TEST(CommandsTest, EqCommand) {
  EXPECT_EQ(EqCommand("L1", "L2").ToString(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n"
            "@L1\n"
            "D;JNE\n"
            "@SP\n"
            "A=M-1\n"
            "M=-1\n"
            "@L2\n"
            "0;JMP\n"
            "(L1)\n"
            "@SP\n"
            "A=M-1\n"
            "M=0\n"
            "(L2)\n");
}

TEST(CommandsTest, GtCommand) {
  EXPECT_EQ(GtCommand("L1", "L2").ToString(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n"
            "@L1\n"
            "D;JLE\n"
            "@SP\n"
            "A=M-1\n"
            "M=-1\n"
            "@L2\n"
            "0;JMP\n"
            "(L1)\n"
            "@SP\n"
            "A=M-1\n"
            "M=0\n"
            "(L2)\n");
}

TEST(CommandsTest, LtCommand) {
  EXPECT_EQ(LtCommand("L1", "L2").ToString(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n"
            "@L1\n"
            "D;JGE\n"
            "@SP\n"
            "A=M-1\n"
            "M=-1\n"
            "@L2\n"
            "0;JMP\n"
            "(L1)\n"
            "@SP\n"
            "A=M-1\n"
            "M=0\n"
            "(L2)\n");
}
